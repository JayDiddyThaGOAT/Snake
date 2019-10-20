#include <SFML/Graphics.hpp>

int main()
{
	srand(time(0));
	sf::RenderWindow window(sf::VideoMode(512, 512), "Snake");

	sf::RectangleShape apple(sf::Vector2f(16.f, 16.f));
	apple.setFillColor(sf::Color::Red);
	int appleStartX = static_cast<int>(rand() % window.getSize().x);
	int appleStartY = static_cast<int>(rand() % window.getSize().y);
	apple.setPosition(sf::Vector2f(appleStartX, appleStartY));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(apple);
		window.display();
	}

	return 0;
}