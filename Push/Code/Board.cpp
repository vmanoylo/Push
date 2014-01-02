#include "Board.h"
#include <algorithm>

bool operator==(action a1, action a2)
{
	if (a1.move == a2.move &&
		a1.position == a2.position &&
		a1.type == a2.type)
		return true;
	return false;
}
bool operator!=(action a1, action a2)
{
	return (! (a1==a2));
}
bool comparePieces(Piece p1, Piece p2)
{
	return ((int)p1.getPieceType() > (int)p2.getPieceType());
}

sf::Vector2i screenToBoardCoord(sf::Vector2f position)
{
	sf::Vector2i board;
	board.x = (int)position.x - MARGIN_LEFT;
	board.y = (int)position.y - MARGIN_TOP;

	board.x /= 80;
	board.y /= 80;

	return board;
}
bool isOnBoard(sf::Vector2i position)
{
	if (position.x >= 0 && position.x < BOARD_WIDTH &&
		position.y >= 0 && position.y < BOARD_HEIGHT)
		return true;
	return false;
}
int pieceDistance(Piece one, Piece two)
{
	int distanceX = one.getPositionOnBoard().x - two.getPositionOnBoard().x;
	int distanceY = one.getPositionOnBoard().y - two.getPositionOnBoard().y;

	distanceX = abs(distanceX);
	distanceY = abs(distanceY);

	return (distanceX > distanceY)? distanceX:distanceY;
}


int Board::occupyingSpace(sf::Vector2i position)
{
	for (unsigned int c = 0; c < onBoard_.size(); c++)
		if (onBoard_[c].checkSpace(position))
			return c;

	return -1;
}
std::vector<int> Board::intersectingPieces(Piece check, direction d)
{
	if (check.isCharacter())
		return std::vector<int> (1, occupyingSpace(check.getPositionOnBoard()));

	else
	{
		switch (check.getPieceType())
		{
		case TOWER:
			return intersectingTower(check, d);
		case WALL:
			return intersectingWall(check, d);
		case RAM:
			return intersectingRam(check, d);
		}
	}
	return std::vector<int>();
}
std::vector<int> Board::intersectingTower(Piece piece, direction d)
{
	std::vector<int> check;
	while (piece.alive())
	{
		if (occupyingSpace(piece.getPositionOnBoard()) != -1)
			check.push_back(occupyingSpace(piece.getPositionOnBoard()));
		piece.moveOnBoard(d);
	}

	std::reverse(check.begin(), check.end());
	return check;
}
std::vector<int> Board::intersectingWall(Piece piece, direction d)
{
	std::vector<int> check;

	d = turnRight(d, 2);
	while (piece.alive())
		piece.moveOnBoard(d);

	d = turnRight(d, 4);
	do
	{
		if (occupyingSpace(piece.getPositionOnBoard()) != -1)
			check.push_back(occupyingSpace(piece.getPositionOnBoard()));
		piece.moveOnBoard(d);		
	}while (piece.alive());

	//std::sort(check.begin(), check.end(), comparePieces);
	return check;
}
std::vector<int> Board::intersectingRam(Piece piece, direction d)
{
	while (piece.alive())
	{
		if (occupyingSpace(piece.getPositionOnBoard()) != -1)
			return std::vector<int>(1, occupyingSpace(piece.getPositionOnBoard()));
		piece.moveOnBoard(d);
	}
	return std::vector<int>();
}
void Board::movePiece(int piece, direction d, int teamPushing, int strength)
{
	if (piece < 0 || strength < 0)
		return;

	Piece temp = onBoard_[piece];
	temp.moveOnBoard(d);

	std::vector<int> moving = intersectingPieces(temp, d);

	if (temp.isCharacter())
	{
		if (temp.getPlayer() == teamPushing)
			movePiece(occupyingSpace(temp.getPositionOnBoard()), d, teamPushing, strength+1);
		else
			movePiece(occupyingSpace(temp.getPositionOnBoard()), d, -1, 1);
	}

	else if (temp.isMachine())
	{
		switch (temp.getPieceType())
		{
		case TOWER:
		case WALL:
			for (unsigned int c = 0; c < moving.size(); c++)
				movePiece(moving[c], d, -1, 1);
			break;

		case RAM:
			if (moving.size() <= 0)
				onBoard_[piece].setPositionOnBoard(-1,-1);
			else
			{
				int distance = pieceDistance(onBoard_[piece], onBoard_[moving.front()]);
				onBoard_[piece].setPositionOnBoard( onBoard_[moving.front()].getPositionOnBoard() );
				movePiece(moving.front(), d, -1, distance);
			}
			break;
		}
	}

	if (temp.getPieceType() != RAM)
	{
		onBoard_[piece].moveOnBoard(d);
		if (strength > 1 && (temp.getPlayer() != teamPushing || temp.isMachine()))
			movePiece(piece, d, -1, strength-1);
	}
}

int Board::getPlayer() const
{
	return player_;
}
	
bool Board::boardSpaceOccupied(int x, int y)
{
	return boardSpaceOccupied(sf::Vector2i(x, y));
}
bool Board::boardSpaceOccupied(sf::Vector2i position)
{
	for (unsigned int c = 0; c < onBoard_.size(); c++)
	{
		if (onBoard_[c].checkSpace(position))
			return true;
	}
	return false;
}

int Board::selectedPiece() const
{
	return selected_;
}
Piece Board::pieceInfo(int pieceCheck) const
{
	if (pieceCheck == -1)
	{
		pieceCheck = selected_;
		if (selected_ == -1)
			return Piece();
	}
	return onBoard_[pieceCheck];
}
std::vector<bool> Board::legalMoves(int pieceCheck) const
{
	if (pieceCheck == -1)
		pieceCheck = selected_;

	std::vector<bool> legal(8, false);
	if (pieceCheck == -1)
		return legal;

	Piece piece = onBoard_[pieceCheck];

	if (piece.isMachine() || piece.getPlayer() != player_)
		return legal;


	switch(piece.getPieceType())
	{
	case SOLDIER:
		for (unsigned int c = 0; c < 8; c+=2)
			legal[c] = true;
		break;

	case SCOUT:
		for (unsigned int c = 1; c < 8; c+=2)
			legal[c] = true;
		break;

	case GENERAL:
		for (unsigned int c = 0; c < 8; c++)
			legal[c] = true;
		break;
	}

	//checking repetition rule
	if (actions_.size() > 4)
	{
		action lastAction = actions_[actions_.size() - 2];
		if (piece.getPieceType() == lastAction.type  &&  piece.getPositionOnBoard() == lastAction.position)
		{
			action check1, check2;
			check1 = actions_[actions_.size() -1];
			check2 = actions_[actions_.size() -3];

			if (check1 == check2)
			{
				for (unsigned int c = 0; c < legal.size(); c++)
					if (direction(c) == lastAction.move)
						legal[c] = false;
			}
		}
	}

	return legal;
}
int Board::graveSize() const
{
	int count = 0;
	for (unsigned int c = 0; c < taken_.size(); c++)
		if (taken_[c].getPlayer() != player_  &&  taken_[c].isCharacter())
			count++;
	return count;
}
bool Board::canPlaceMachine() const
{
	if (graveSize() == 0)
		return false;
	for (unsigned int c = 0; c < onBoard_.size(); c++)
		if (onBoard_[c].isMachine()  &&  onBoard_[c].getPlayer() == player_)
			return false;

	return true;
}

std::vector<action> Board::player1Actions() const
{
	std::vector<action> player1;
	for (unsigned int c = 0; c < actions_.size(); c+=2)
		player1.push_back(actions_[c]);

	return player1;
}
std::vector<action> Board::player2Actions() const
{
	std::vector<action> player2;
	for (unsigned int c = 1; c < actions_.size(); c+=2)
		player2.push_back(actions_[c]);

	return player2;
}

int Board::checkGame() const
{
	int player1=0, player2=0;
	for (unsigned int c = 0; c < onBoard_.size(); c++)
	{
		if (onBoard_[c].isCharacter())
		{
			if (onBoard_[c].getPlayer() == 1)
				player1++;
			else
				player2++;
		}
	}

	if (player1 == 0 && player2 == 0)
		return -1;
	else if (player1 == 0)
		return 2;
	else if (player2 == 0)
		return 1;

	if (turnsWithoutIncident_ >= 30)
		return -1;
	
	return 0;
}

bool Board::click(sf::Vector2f mousePosition)
{
	sf::Vector2i space = screenToBoardCoord(mousePosition);
	for (unsigned int c = 0; c < onBoard_.size(); c++)
	{
		if (onBoard_[c].checkSpace(space))
		{
			selected_ = c;
			return true;
		}
	}
	return false;
}
void Board::moveSelectedPiece(direction d)
{
	int count = onBoard_.size();

	action add;
	add.move = d;
	add.type = onBoard_[selected_].getPieceType();
	add.position = onBoard_[selected_].getPositionOnBoard();
	actions_.push_back(add);
	turnsWithoutIncident_++;

	movePiece(selected_, d, player_, 0);
	selected_ = -1;
	switchPlayers();

	for (int c = count -1; c >= 0; c--){
		if (!onBoard_[c].alive()){
			taken_.push_back(onBoard_[c]);
			onBoard_.erase(onBoard_.begin() + c);
		}
	}

	if (onBoard_.size() != count)
		turnsWithoutIncident_ = 0;
}
bool Board::placeMachine(int type, sf::Vector2f mousePosition)
{
	if (!canPlaceMachine())
		return false;

	sf::Vector2i boardPosition = screenToBoardCoord(mousePosition);
	if (!boardSpaceOccupied(boardPosition)  &&  isOnBoard(boardPosition))
	{
		for (unsigned int c = 0; c < taken_.size(); c++)
		{
			if (taken_[c].getPlayer() != player_)
			{
				taken_.erase(taken_.begin() + c);
				break;
			}
		}

		action add;
		add.move = SET;
		add.position = boardPosition;
		add.type = pieceType(type);
		actions_.push_back(add);
		turnsWithoutIncident_++;

		onBoard_.push_back(Piece(texturePieces_, player_, pieceType(type), boardPosition));
		switchPlayers();
		return true;
	}
	return false;
}
void Board::switchPlayers()
{
	if (player_ == 1)
		player_ = 2;
	else
		player_ = 1;
}

void Board::draw (sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(board_, states);
	for (unsigned int c = 0; c < onBoard_.size(); c++)
		if (onBoard_[c].alive())
			target.draw(onBoard_[c]);
}

void Board::resetGame()
{
	player_ = 1;
	selected_ = -1;
	turnsWithoutIncident_ = 0;

	onBoard_.clear();
	taken_.clear();
	actions_.clear();


	Piece temp(texturePieces_);
	
	//player 1
	temp.setPlayer(1);
	//Soldiers
	temp.setPiecetype(SOLDIER);
	temp.setPositionOnBoard(1, 5);
	for (int c = 0; c < 4; c++)
	{
		onBoard_.push_back(temp);
		temp.moveOnBoard(2,0);
	}
	//Scouts
	temp.setPiecetype(SCOUT);
	temp.setPositionOnBoard(0, 6);
	for (int c = 0; c < 5; c++)
	{
		onBoard_.push_back(temp);
		temp.moveOnBoard(2,0);
	}
	//Generals
	temp.setPiecetype(GENERAL);
	temp.setPositionOnBoard(3, 7);
	onBoard_.push_back(temp);
	temp.setPositionOnBoard(5, 7);
	onBoard_.push_back(temp);

	//player 2
	temp.setPlayer(2);
	//Soldiers
	temp.setPiecetype(SOLDIER);
	temp.setPositionOnBoard(1, 2);
	for (int c = 0; c < 4; c++)
	{
		onBoard_.push_back(temp);
		temp.moveOnBoard(2,0);
	}
	//Scouts
	temp.setPiecetype(SCOUT);
	temp.setPositionOnBoard(0, 1);
	for (int c = 0; c < 5; c++)
	{
		onBoard_.push_back(temp);
		temp.moveOnBoard(2,0);
	}
	//Generals
	temp.setPiecetype(GENERAL);
	temp.setPositionOnBoard(3, 0);
	onBoard_.push_back(temp);
	temp.setPositionOnBoard(5, 0);
	onBoard_.push_back(temp);
}
Board::Board()
{	

	texturePieces_.loadFromFile(FILE_PIECES);
	textureBoard_.loadFromFile(FILE_BOARD);
	
	board_.setTexture(textureBoard_);

	resetGame();
}