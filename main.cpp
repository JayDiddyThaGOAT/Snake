#include <SFML/Graphics.hpp>

const int SIZE = 32;
const int ROWS = 17;
const int COLS = 17;

int width = ROWS * SIZE;
int height = COLS * SIZE;

char snakeDirection = 'S';
int snakeLength = 4;
bool dead = false;

bool snakeAI = true;

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

bool IsInSnake(sf::Vector2i& cell)
{
	bool snakeNeighbor = false;
	for (int i = 1; i < snakeLength; i++)
	{
		if (cell.x == snake[i].x && cell.y == snake[i].y)
		{
			snakeNeighbor = true;
			break;
		}
	}

	return snakeNeighbor;
}

void Update()
{
	if (dead)
		return;

	for (int i = snakeLength; i > 0; --i)
	{
		snake[i].x = snake[i - 1].x;
		snake[i].y = snake[i - 1].y;
	}

	switch (snakeDirection)
	{
	case 'N':
		if (snake[0].y == 0)
			snake[0].y = ROWS - 1;
		else
			snake[0].y -= 1;
		break;
	case 'S':
		snake[0].y = (snake[0].y + 1) % ROWS;
		break;
	case 'W':
		if (snake[0].x == 0)
			snake[0].x = COLS - 1;
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

	snake[0].x = (COLS - 1) / 2;
	snake[0].y = (ROWS - 1) / 2;
	for (int i = 1; i < snakeLength; i++)
	{
		snake[i].x = snake[0].x;
		snake[i].y = snake[0].y;
	}

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

		if (!snakeAI)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				snakeDirection = 'N';
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				snakeDirection = 'S';
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				snakeDirection = 'W';
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				snakeDirection = 'E';
		}
		else
		{
			sf::Vector2i north, south, west, east;
			if (snake[0].y == 0)
				north = sf::Vector2i(snake[0].x, ROWS - 1);
			else
				north = sf::Vector2i(snake[0].x, snake[0].y - 1);

			if (snake[0].y == ROWS - 1)
				south = sf::Vector2i(snake[0].x, 0);
			else
				south = sf::Vector2i(snake[0].x, snake[0].y + 1);

			if (snake[0].x == 0)
				west = sf::Vector2i(COLS - 1, snake[0].y);
			else
				west = sf::Vector2i(snake[0].x - 1, snake[0].y);

			if (snake[0].x == COLS - 1)
				east = sf::Vector2i(0, snake[0].y);
			else
				east = sf::Vector2i(snake[0].x + 1, snake[0].y);

			std::map<char, sf::Vector2i> neighbors;

			if (!IsInSnake(north))
				neighbors['N'] = north;

			if (!IsInSnake(south))
				neighbors['S'] = south;

			if (!IsInSnake(west))
				neighbors['W'] = west;

			if (!IsInSnake(east))
				neighbors['E'] = east;

			int minDistance = 100000000;

			for (auto& neighbor : neighbors)
			{
				int distanceFromNeighborToApple = abs(neighbor.second.x - apple.x) + abs(neighbor.second.y - apple.y);

				if (distanceFromNeighborToApple < minDistance)
				{
					snakeDirection = neighbor.first;
					minDistance = distanceFromNeighborToApple;
				}
			}
		}

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