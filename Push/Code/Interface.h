#ifndef INTERFACE_H
#define INTERFACE_H

#include <SFML\Graphics.hpp>
#include <vector>
#include <string>
#include "Board.h"

#define ACTIONS_ON_SCREEN 15

std::string directionToString(direction d);
direction stringToDirection(std::string s);
std::string actionToString(action a);
action stringToAction(std::string s);

class Interface : public sf::Drawable
{
	sf::CircleShape arrows_[8];
	bool legal_[8];
	
	sf::Font font_;
	sf::Texture texture_;

	Piece piece_;
	Piece addMachine_[3];
	int machine_;
	bool canPlaceMachine_;

	std::vector<sf::Text> actions1;
	std::vector<sf::Text> actions2;
	sf::Text action_;
	sf::Text print_;

	void moveActions();
	void updateActions(Board& board);
	void updatePiece(Board& board);
	void updateArrows(Board& board);
public:
	void update(Board& board);
	void click(Board& board, sf::Vector2f mousePos);
	void resetActions();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	Interface();
};

#endif