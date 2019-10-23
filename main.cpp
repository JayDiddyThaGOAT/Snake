#include <SFML/Graphics.hpp>

const int SIZE = 32;
const int ROWS = 17;
const int COLS = 17;

int width = ROWS * SIZE;
int height = COLS * SIZE;

char direction = 'S';
int snakeLength = 4;
bool dead = false;

struct Snake
{
	int x, y;
};
Snake snake[ROWS * COLS];

struct Apple
{
	int x, y;
};
Apple apple;

sf::Texture emptyTexture, appleTexture, snakeHeadTexture, snakeBodyTexture;
sf::Sprite emptySprite, appleSprite, snakeHeadSprite, snakeBodySprite;

void Update()
{
	if (dead)
		return;

	for (int i = snakeLength; i > 0; --i)
	{
		snake[i].x = snake[i - 1].x;
		snake[i].y = snake[i - 1].y;
	}

	switch (direction)
	{
	case 'N':
		if (snake[0].y == 0)
			snake[0].y = ROWS;
		else
			snake[0].y -= 1;
		break;
	case 'S':
		snake[0].y = (snake[0].y + 1) % ROWS;
		break;
	case 'W':
		if (snake[0].x == 0)
			snake[0].x = COLS;
		else
			snake[0].x -= 1;
		break;
	case 'E':
		snake[0].x = (snake[0].x + 1) % COLS;
		break;
	}

	if (snake[0].x == apple.x && snake[0].y == apple.y)
	{
		apple.x = rand() % COLS;
		apple.y = rand() % ROWS;

		snakeLength++;
	}

	for (int i = 1; i < snakeLength; i++)
	{
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
		{
			dead = true;
		}
	}
}

int main()
{
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(width, height), "Snake", sf::Style::Titlebar | sf::Style::Close);

	emptyTexture.loadFromFile("Sprites/Empty.png");
	appleTexture.loadFromFile("Sprites/Apple.png");
	snakeHeadTexture.loadFromFile("Sprites/SnakeHead.png");
	snakeBodyTexture.loadFromFile("Sprites/SnakeBody.png");

	emptySprite.setTexture(emptyTexture);
	appleSprite.setTexture(appleTexture);
	snakeHeadSprite.setTexture(snakeHeadTexture);
	snakeBodySprite.setTexture(snakeBodyTexture);

	sf::Clock clock;
	float timer = 0, delay = 0.1;

	apple.x = rand() % COLS;
	apple.y = rand() % ROWS;

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();

		if (timer > delay)
		{
			timer = 0;
			Update();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			direction = 'N';
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			direction = 'S';
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			direction = 'W';
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			direction = 'E';

		for (int i = 0; i < COLS; i++)
		{
			for (int j = 0; j < ROWS; j++)
			{
				emptySprite.setPosition(sf::Vector2f(i * SIZE, j * SIZE));
				window.draw(emptySprite);
			}
		}

		snakeHeadSprite.setPosition(snake[0].x * SIZE, snake[0].y * SIZE);
		window.draw(snakeHeadSprite);
		for (int i = 1; i < snakeLength; i++)
		{
			snakeBodySprite.setPosition(sf::Vector2f(snake[i].x * SIZE, snake[i].y * SIZE));
			window.draw(snakeBodySprite);
		}

		appleSprite.setPosition(apple.x * SIZE, apple.y * SIZE);
		window.draw(appleSprite);

		window.display();
	}

	return 0;
}