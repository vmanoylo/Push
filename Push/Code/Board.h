#ifndef BOARD_H
#define BOARD_H

#include <SFML\Graphics.hpp>
#include <vector>
#include "Piece.h"

struct action{
	pieceType type;
	sf::Vector2i position; 
	direction move;

	friend bool operator==(action, action);
	friend bool operator!=(action, action);

};

sf::Vector2i screenToBoardCoord(sf::Vector2f position);
bool isOnBoard(sf::Vector2i position);
int pieceDistance(Piece one, Piece two);

class Board : public sf::Drawable
{
	sf::Texture texturePieces_;
	sf::Texture textureBoard_;
	sf::Sprite board_;

	int player_;
	int selected_;
	int turnsWithoutIncident_;
	
	std::vector<Piece> taken_;
	std::vector<Piece> onBoard_;
	std::vector<action> actions_;

	int occupyingSpace(sf::Vector2i position);

	std::vector<int> intersectingPieces(Piece check, direction d);

	std::vector<int> intersectingTower(Piece piece, direction d);
	std::vector<int> intersectingWall(Piece piece, direction d);
	std::vector<int> intersectingRam(Piece piece, direction d);

	void movePiece(int piece, direction d, int teamPushing, int strength);
public:
	int getPlayer() const;
	
	bool boardSpaceOccupied(int x, int y);
	bool boardSpaceOccupied(sf::Vector2i position);

	int selectedPiece() const;
	Piece pieceInfo(int pieceCheck = -1) const;
	std::vector<bool> legalMoves(int pieceCheck = -1) const;
	int graveSize() const;
	bool canPlaceMachine() const;

	std::vector<action> player1Actions() const;
	std::vector<action> player2Actions() const;

	int checkGame() const;
	/* 
	-1 = draw
	0 = continue
	1 = player 1 wins
	2 = player 2 wins*/

	bool click(sf::Vector2f mousePosition);
	void moveSelectedPiece(direction d);
	bool placeMachine(int type, sf::Vector2f mousePosition);
	void switchPlayers();

	virtual void draw (sf::RenderTarget &target, sf::RenderStates states) const;

	void resetGame();
	Board();
};

#endif