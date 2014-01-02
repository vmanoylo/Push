#include "Interface.h"

std::string directionToString(direction d)
{
	switch (d)
	{
	case N:
		return "N";
	case NE:
		return "NE";
	case E:
		return "E";
	case SE:
		return "SE";
	case S:
		return "S";
	case SW:
		return "SW";
	case W:
		return "W";
	case NW:
		return "NW";
	default:
		return "SET_";
	}
}
direction stringToDirection(std::string s)
{
	if (s == "N")
		return N;
	else if (s == "NE")
		return NE;
	else if (s == "E")
		return E;
	else if (s == "SE")
		return SE;
	else if (s == "S")
		return S;
	else if (s == "SW")
		return SW;
	else if (s == "W")
		return W;
	else if (s == "NW")
		return NW;
	else
		return SET;
}
std::string actionToString(action a)
{
	std::string translate ="";

	translate += (a.position.x + 'A');
	translate += ((BOARD_HEIGHT - a.position.y) + '0'); 

	translate += ':';

	translate += directionToString(a.move);
	if (a.move == SET)
	{
		switch (a.type)
		{
		case TOWER:
			translate += "T";
			break;
		case RAM:
			translate += "R";
			break;
		case WALL:
			translate += "W";
			break;
		}
	}
	return translate;
}
action stringToAction(std::string s)
{
	action translate;

	translate.position.x = (s[0] - 'A');
	translate.position.y =  (BOARD_HEIGHT - (s[1] - '0')); 

	int find = s.find(' ');
	std::string direction = s.substr(3, find-3);
	translate.move = stringToDirection(direction);

	if (translate.move != SET)
		translate.type = ANON;
	else
	{
		switch(s.back())
		{
		case 'T':
			translate.type = TOWER;
		case 'R':
			translate.type = RAM;
		case 'W':
			translate.type = WALL;
		}
	}

	return translate;
}

void Interface::moveActions()
{
	float move = (float)action_.getCharacterSize() * float(3.0/2.0);

	int difference = 0;
	if (actions1.size() > ACTIONS_ON_SCREEN)
		difference = actions1.size() - ACTIONS_ON_SCREEN;

	for (unsigned int c = 0; c < actions1.size(); c++)
	{
		actions1[c].setPosition(action_.getPosition());
		actions1[c].move(0, move * (c - difference));
	}
	
	difference = 0;
	if (actions2.size() > ACTIONS_ON_SCREEN)
		difference = actions2.size() - ACTIONS_ON_SCREEN;

	for (unsigned int c = 0; c < actions2.size(); c++)
	{
		actions2[c].setPosition(action_.getPosition());
		actions2[c].move(80, move * (c - difference));
	}
}
void Interface::updateActions(Board& board)
{
	std::vector<action> update;

	update = board.player1Actions();
	for (unsigned int c = actions1.size(); c < update.size(); c++)
	{
		action_.setString(actionToString(update[c]));
		actions1.push_back(action_);
	}
	
	update = board.player2Actions();
	for (unsigned int c = actions2.size(); c < update.size(); c++)
	{
		action_.setString(actionToString(update[c]));
		actions2.push_back(action_);
	}	
	moveActions();
}
void Interface::updatePiece(Board& board)
{
	piece_.setPlayer(board.pieceInfo().getPlayer());
	piece_.setPiecetype(board.pieceInfo().getPieceType());
	if (machine_ != -1)
		piece_.setPiecetype((pieceType)machine_);
}
void Interface::updateArrows(Board& board)
{
	std::vector<bool> check;
	check = board.legalMoves();
	for (int c = 0; c < 8; c++)
		legal_[c] = check[c];
}

void Interface::update(Board& board)
{
	updatePiece(board);
	updateActions(board);
	updateArrows(board);
	print_.setString("Player " + std::to_string(board.getPlayer()) + '\n'
		+ "Taken " + std::to_string(board.graveSize()));

	canPlaceMachine_ = board.canPlaceMachine();
}
void Interface::click(Board& board, sf::Vector2f mousePos)
{
	if (machine_ != -1)
		board.placeMachine(machine_, mousePos);
	machine_ = -1;

	if (!board.click(mousePos))
	{
		for (int c = 0; c < 8; c++)
			if (legal_[c] && (arrows_[c].getGlobalBounds().contains(mousePos)))
				board.moveSelectedPiece(direction(c));
		for (int c = 0; c < 3; c++)
			if (addMachine_[c].getGlobalBounds().contains(mousePos))
				machine_ = (int)addMachine_[c].getPieceType();
	}

	update(board);
}
void Interface::resetActions()
{
	actions1.clear();
	actions2.clear();
}

void Interface::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (int c = 0; c < 8; c++)
		if (legal_[c])
			target.draw(arrows_[c], states);
	target.draw(piece_, states);

	int start = (int)actions1.size() - ACTIONS_ON_SCREEN;
	if (start < 0)
		start = 0;
	int end = actions1.size();

	for (int c = start; c < end; c++)
		target.draw(actions1[c], states);

	start = actions2.size() - ACTIONS_ON_SCREEN;
	if (start < 0)
		start = 0;
	end = actions2.size();

	for (int c = start; c < end; c++)
		target.draw(actions2[c], states);	

	if (canPlaceMachine_)
		for (unsigned int c = 0; c < 3; c++)
			target.draw(addMachine_[c], states);

	target.draw(print_, states);
}
Interface::Interface()
{
	texture_.loadFromFile("Images/pieces.png");
	piece_.setTexture(texture_);
	piece_.setPosition(880, 75);
	for (int c = 0; c < 3; c++)
	{
		addMachine_[c].setTexture(texture_);
		addMachine_[c].setPlayer(1);
		addMachine_[c].setPosition(850, 240);
		addMachine_[c].move(float(c*30), 0);
		addMachine_[c].setScale(.5, .5);
	}

	addMachine_[0].setPiecetype(TOWER);
	addMachine_[1].setPiecetype(RAM);
	addMachine_[2].setPiecetype(WALL);

	canPlaceMachine_ = false;

	font_.loadFromFile("font.ttf");
	action_.setFont(font_);
	action_.setColor(sf::Color::Black);
	action_.setCharacterSize(15);
	action_.setPosition(830, 280);

	print_.setFont(font_);
	print_.setColor(sf::Color::Black);
	print_.setCharacterSize(20);
	print_.setPosition(800, 0);
	

	for (int c = 0; c < 8; c++)
	{
		arrows_[c].setPointCount(3);
		arrows_[c].setRadius(25);
		arrows_[c].setPosition(BOARD_WIDTH*80 + 200, 120);
		arrows_[c].setOrigin(25, 100);
		arrows_[c].rotate((float)c * 360/8);

		arrows_[c].setFillColor(sf::Color(255, 255, 255, 200));
		arrows_[c].setOutlineColor(sf::Color(255, 200, 10, 200));
		arrows_[c].setOutlineThickness(2);
	}

	for (int c = 0; c < 8; c++)
		legal_[c] = false;
}