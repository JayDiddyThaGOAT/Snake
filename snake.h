#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

enum class DIRECTION { LEFT, RIGHT, UP, DOWN, NONE};

class Snake
{
private:
	std::vector<sf::RectangleShape> body;

	DIRECTION direction = DIRECTION::NONE;
	float speed = 5;

public:
	Snake(int x, int y)
	{
		sf::RectangleShape head;
		head.setSize(sf::Vector2f(16.f, 16.f));
		head.setFillColor(sf::Color::Green);
		head.setOrigin(sf::Vector2f(8.f, 8.f));
		head.setPosition(sf::Vector2f(x, y));

		body.push_back(head);
	}

	sf::RectangleShape getHead()
	{
		return body[body.size() - 1];
	}

	void grow()
	{
		sf::RectangleShape part;

		part.setSize(body[0].getSize());
		part.setFillColor(body[0].getFillColor());
		part.setOrigin(body[0].getOrigin());
		part.setPosition(body[body.size() - 1].getPosition());

		body.push_back(part);
	}

	void draw(sf::RenderWindow& window)
	{
		for (auto& part : body)
		{
			window.draw(part);
		}
	}

	void update(sf::RenderWindow& window)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			direction = DIRECTION::LEFT;
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			direction = DIRECTION::RIGHT;
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			direction = DIRECTION::UP;
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			direction = DIRECTION::DOWN;

		for (int i = 0; i < body.size() - 1; i++)
		{
			body[i].setPosition(body[i + 1].getPosition());
		}

		switch (direction)
		{
		case DIRECTION::UP:
			body[body.size() - 1].move(0, -speed);
			break;
		case DIRECTION::DOWN:
			body[body.size() - 1].move(0, speed);
			break;
		case DIRECTION::LEFT:
			body[body.size() - 1].move(-speed, 0);
			break;
		case DIRECTION::RIGHT:
			body[body.size() - 1].move(speed, 0);
			break;
		default:
			body[body.size() - 1].move(0, 0);
			break;
		}
	}
};