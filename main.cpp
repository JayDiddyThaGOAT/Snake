#include "snake.h"
#include "apple.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(512, 512), "Snake");
	window.setFramerateLimit(60);

	Apple apple;
	apple.spawn(window);

	Snake snake(window.getSize().x / 2, window.getSize().y / 2);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		window.draw(apple);;

		snake.update(window);
		snake.draw(window);

		float distance = sqrt(pow(snake.getHead().getPosition().x - apple.getPosition().x, 2) + pow(snake.getHead().getPosition().y - apple.getPosition().y, 2));

		if (distance <= 16.f)
		{
			snake.grow();
			apple.spawn(window);
		}

		window.display();
	}

	return 0;
}