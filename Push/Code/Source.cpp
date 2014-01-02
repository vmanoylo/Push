#include <SFML\Graphics.hpp>
#include <string>
#include "Piece.h"
#include "Board.h"
#include "Interface.h"

int main()
{
#pragma region initialize
	sf::RenderWindow window(sf::VideoMode(1040, 700), "Push"/*, sf::Style::Fullscreen*/); //interface : 280 x 700
	window.setPosition(sf::Vector2i(0,0));
	Board board;
	Interface ui;
	ui.update(board);

	sf::Font font;
	font.loadFromFile("font.ttf");

	sf::Event input;
	sf::Vector2f click;
		
	sf::Vector2f center = window.getView().getCenter();
	sf::Text message("", font, 40);
	message.setPosition(center);
	message.setStyle(sf::Text::Bold);
	
	sf::RectangleShape box;
	box.setPosition(center);
	box.setSize(sf::Vector2f(300, 150));
	box.setFillColor(sf::Color(0,0,0, 150));
		
	sf::Text yes("YES", font, 30), no ("NO", font, 30);
	yes.setPosition(center);
	no.setPosition(center);
	yes.move(75, 100);
	no.move(175, 100);
#pragma endregion

	while (window.isOpen())
	{

#pragma region playGame
		while (board.checkGame() == 0 && window.isOpen())
		{
			while (window.pollEvent(input))
			{
				if (input.type == sf::Event::MouseButtonPressed)
				{
					click.x = (float)input.mouseButton.x;
					click.y = (float)input.mouseButton.y;
					ui.click(board, click);
					//ui.update(board);
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
				board.resetGame();
			
			window.clear(sf::Color(255,230,200));
			window.draw(board);
			window.draw(ui);
			window.display();
		}
#pragma endregion

#pragma region playAgain?
		message.setString("Player " + std::to_string(board.checkGame()) + " wins");
		if (board.checkGame() == -1)
			message.setString("Stalemate");
		message.setString(message.getString() + "\n Play Again?");
		
		window.draw(box);
		window.draw(message);
		window.draw(yes);
		window.draw(no);
		window.display();

		while (window.isOpen()  &&  board.checkGame() != 0)
		{
			while (window.pollEvent(input))
			{
				if (input.type == sf::Event::MouseButtonPressed)
				{
					click.x = (float)input.mouseButton.x;
					click.y = (float)input.mouseButton.y;
					if (yes.getGlobalBounds().contains(click))
					{
						board.resetGame();
						ui.resetActions();
					}
					if (no.getGlobalBounds().contains(click))
						window.close();
				}
			}
		}
#pragma endregion

	}
}
