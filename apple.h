#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Apple : public sf::RectangleShape
{
public:
	Apple()
	{
		setSize(sf::Vector2f(16.f, 16.f));
		setFillColor(sf::Color::Red);
		setOrigin(sf::Vector2f(getSize().x / 2, getSize().y / 2));
	}

	void spawn(sf::RenderWindow& window)
	{
		srand(time(0));
		int maxX = static_cast<int>(window.getSize().x - getSize().x);
		int maxY = static_cast<int>(window.getSize().y - getSize().y);
		int minX = static_cast<int>(getOrigin().x);
		int minY = static_cast<int>(getOrigin().y);

		int appleStartX = rand() % maxX + minX;
		int appleStartY = rand() % maxY + minY;;
		setPosition(sf::Vector2f(appleStartX, appleStartY));
	}
};