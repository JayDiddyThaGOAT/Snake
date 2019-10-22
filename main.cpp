#include <SFML/Graphics.hpp>

const int SIZE = 32;
const int ROWS = 16;
const int COLS = 16;

int width = ROWS * SIZE;
int height = COLS * SIZE;

int main()
{
	sf::RenderWindow window(sf::VideoMode(width, height), "Snake", sf::Style::Titlebar | sf::Style::Close);

	sf::Texture emptyTexture;
	emptyTexture.loadFromFile("Sprites/Empty.png");

	sf::Sprite emptySprite;
	emptySprite.setTexture(emptyTexture);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				emptySprite.setPosition(i * SIZE, j * SIZE);
				window.draw(emptySprite);
			}
		}

		window.display();
	}

	return 0;
}