#include "Piece.h"

direction turnRight(direction d, int n)
{
	if (d == SET)
		return d;
	int x = (int)d + n;
	x %= 8;
	return (direction)x;
}


void Piece::setPlayer(int set)
{
	player_ = set;
	findTexture();
}
void Piece::setPiecetype(pieceType set)
{
	type_ = set;
	findTexture();
}
void Piece::setPositionOnBoard(int x, int y)
{
	setPositionOnBoard(sf::Vector2i(x, y));
}
void Piece::setPositionOnBoard(sf::Vector2i position)
{
	position_ = position;
	setPosition(float(position.x * 80 + MARGIN_LEFT), float(position.y * 80 + MARGIN_TOP));
}

bool Piece::isCharacter() const
{
	if ((int)type_ < 3)
		return true;
	return false;
}
bool Piece::isMachine() const
{
	return !isCharacter();
}
bool Piece::checkSpace(int x, int y) const
{
	return checkSpace(sf::Vector2i(x, y));
}
bool Piece::checkSpace(sf::Vector2i position) const
{
	if (position == position_)
		return true;
	return false;
}
bool Piece::alive() const
{
	if (position_.x >= 0 && position_.x < BOARD_WIDTH &&
		position_.y >= 0 && position_.y < BOARD_HEIGHT)
		return true;
	return false;
}
	
int Piece::getPlayer() const
{
	return player_;
}
pieceType Piece::getPieceType() const
{
	return type_;
}

sf::Vector2i Piece::getPositionOnBoard() const
{
	return position_;
}


void Piece::moveOnBoard(direction d)
{
	switch (d)
	{
	case N:
		moveOnBoard(0,-1);
		break;
	case NE:
		moveOnBoard(1,-1);
		break;
	case E:
		moveOnBoard(1,0);
		break;
	case SE:
		moveOnBoard(1,1);
		break;
	case S:
		moveOnBoard(0,1);
		break;
	case SW:
		moveOnBoard(-1,1);
		break;
	case W:
		moveOnBoard(-1,0);
		break;
	case NW:
		moveOnBoard(-1,-1);
		break;
	}
}
void Piece::moveOnBoard(int x, int y)
{
	setPositionOnBoard(position_.x + x, position_.y + y);
}

Piece::Piece(sf::Texture& texture, int player, pieceType type, int x, int y)
	:sf::Sprite(texture)
{
	player_ = player;
	type_ = type;
	setPositionOnBoard (x, y);
	findTexture();
}
Piece::Piece(sf::Texture& texture, int player, pieceType type, sf::Vector2i position)
	:sf::Sprite(texture)
{
	player_ = player;
	type_ = type;
	setPositionOnBoard (position);
	findTexture();
}
Piece::Piece(sf::Texture& texture)
	:sf::Sprite(texture)
{
	player_ = 0;
	type_ = SOLDIER;
	setPositionOnBoard();
}


void Piece::findTexture()
{
	setTextureRect(sf::IntRect(0,0,80,80));
	setFrame(type_, player_ -1);
}
void Piece::setFrame(int x, int y)
{
	int width = getTextureRect().width;
	int height = getTextureRect().height;
	setTextureRect(sf::IntRect(x * width, y * height, width, height));
}
