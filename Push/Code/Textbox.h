#ifndef PIECE_H
#define PIECE_H

#include<SFML\Graphics.hpp>

#define BOARD_WIDTH 9
#define BOARD_HEIGHT 8
#define MARGIN_LEFT 0
#define MARGIN_TOP 0

#define FILE_PIECES	"Images/Pieces.png"
#define FILE_BOARD	"Images/Board.png"

enum pieceType{ANON = -1, SOLDIER, SCOUT, GENERAL, TOWER, RAM, WALL};
enum direction{SET = -1, N, NE, E, SE, S, SW, W, NW};
direction turnRight(direction d, int n);

class Piece : public sf::Sprite
{
	int player_;
	pieceType type_;
	sf::Vector2i position_;

	void findTexture();
	void setFrame(int x, int y);
public:
	void setPlayer(int set);
	void setPiecetype(pieceType set);
	void setPositionOnBoard(int x =0, int y =0);
	void setPositionOnBoard(sf::Vector2i position);

	bool isCharacter() const;
	bool isMachine() const;
	bool checkSpace(int x, int y) const;
	bool checkSpace(sf::Vector2i position) const;
	bool alive() const;
	
	int getPlayer() const;
	pieceType getPieceType() const;
	sf::Vector2i getPositionOnBoard() const;


	void moveOnBoard(direction d);
	void moveOnBoard(int x, int y);

	Piece(sf::Texture&, int player, pieceType type, int x=0, int y=0);
	Piece(sf::Texture&, int player, pieceType type, sf::Vector2i position = sf::Vector2i());
	Piece(sf::Texture& = sf::Texture());
};

#endif