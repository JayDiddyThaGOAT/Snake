#include <SFML/Graphics.hpp>
#include <stack>
#include <iostream>

const int SIZE = 32;
const int COUNT = 16;

int width = COUNT * SIZE;
int height = COUNT * SIZE;

char snakeDirection;
int snakeLength;
std::vector<int> snake;
bool dead = false;

bool snakeAI = false;

int apple;
int newTail;
bool eaten;

bool gameBegan, paused;

struct Cell
{
	int x, y;
	char direction;
	bool visited;
	bool solid;
};

Cell cells[COUNT * COUNT];

sf::Texture emptyTexture, appleTexture, snakeHeadTexture, snakeBodyTexture;
sf::Sprite emptySprite, appleSprite, snakeHeadSprite, snakeBodySprite;
sf::Font font;

int ManhattanDistance(int a, int b)
{
	return abs(cells[b].x - cells[a].x) + abs(cells[b].y - cells[a].y);
}

std::vector<int> NeighborsOf(int root)
{
	std::vector<int> neighbors;

	if (cells[root - COUNT].y >= 0)
		neighbors.push_back(root - COUNT);

	if (cells[root + 1].x < COUNT - 1)
		neighbors.push_back(root + 1);

	if (cells[root + COUNT].y < COUNT - 1)
		neighbors.push_back(root + COUNT);

	if (cells[root - 1].x >= 0)
		neighbors.push_back(root - 1);


	return neighbors;
}

void Update()
{
	if (dead)
		return;

	int tail = snake.size() - 1;
	cells[snake[tail]].solid = false;
	for (int i = tail; i > 0; i--)
	{
		snake[i] = snake[i - 1];
		cells[snake[i]].solid = true;
	}

	switch (snakeDirection)
	{
	case 'N':
		if (cells[snake[0]].y > 0)
			snake[0] -= COUNT;
		else
			dead = true;
		break;
	case 'S':
		if (cells[snake[0]].y < COUNT - 1)
			snake[0] += COUNT;
		else
			dead = true;
		break;
	case 'W':
		if (cells[snake[0]].x > 0)
			snake[0]--;
		else
			dead = true;
		break;
	case 'E':
		if (cells[snake[0]].x < COUNT - 1)
			snake[0]++;
		else
			dead = true;
		break;
	}

	cells[snake[0]].visited = true;
	cells[snake[0]].solid = true;
	cells[snake[0]].direction = snakeDirection;

	if (snake.size() == 2)
	{
		int head = snake[0];
		int tail = snake[1];

		if (cells[head].direction == 'N' && cells[tail].direction == 'S' || cells[head].direction == 'S' && cells[tail].direction == 'N')
		{
			snake[0] = snake[1];
			dead = true;
		}

		if (cells[head].direction == 'W' && cells[tail].direction == 'E' || cells[head].direction == 'E' && cells[tail].direction == 'W')
		{
			snake[0] = snake[1];
			dead = true;
		}
	}
	else if (snake.size() > 2)
	{
		for (int i = 1; i < snake.size(); i++)
		{
			if (snake[0] == snake[i])
				dead = true;
		}
	}

	if (snake[0] == apple)
	{
		eaten = true;

		do
		{
			apple = rand() % (COUNT * COUNT);
		} while (cells[apple].solid);
		cells[apple].solid = false;

		int tail = snake.size() - 1;
		switch (cells[snake[tail]].direction)
		{
		case 'N':
			newTail = snake[tail] + COUNT;
			break;
		case 'S':
			newTail = snake[tail] - COUNT;
			break;
		case 'W':
			newTail = snake[tail] + 1;
			break;
		case 'E':
			newTail = snake[tail] - 1;
			break;
		}

		snake.push_back(newTail);
		snakeLength = snake.size();
	}
}

void StartGame()
{
	paused = false;
	gameBegan = false;
	dead = false;

	for (int i = 0; i < COUNT; i++)
	{
		for (int j = 0; j < COUNT; j++)
		{
			cells[i * COUNT + j].x = j;
			cells[i * COUNT + j].y = i;
			cells[i * COUNT + j].solid = false;
			cells[i * COUNT + j].visited = false;
			cells[i + COUNT + j].direction = 'X';
		}
	}

	snake.clear();
	snakeLength = 0;

	int center = (COUNT * COUNT) / 2;
	if (COUNT % 2 == 0)
	{
		int offsets[4] = { (COUNT / 2) - 1,  COUNT / 2, COUNT + (COUNT / 2) - 1, COUNT + (COUNT / 2) };
		center += offsets[rand() % 4];
	}


	snake.push_back(center);
	cells[snake[0]].visited = true;
	cells[snake[0]].solid = true;

	do
	{
		apple = rand() % (COUNT * COUNT);
	} while (cells[apple].solid);
	cells[apple].solid = false;
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

	font.loadFromFile("Font/ARCADE_I.TTF");

	sf::Clock clock;
	float timer = 0, delay = 0.1;
	float eatTimer = 0;

	StartGame();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (!dead)
		{
			if (!snakeAI)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				{
					snakeDirection = 'N';

					if (!gameBegan)
						gameBegan = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				{
					snakeDirection = 'E';

					if (!gameBegan)
						gameBegan = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				{
					snakeDirection = 'S';

					if (!gameBegan)
						gameBegan = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				{
					snakeDirection = 'W';

					if (!gameBegan)
						gameBegan = true;
				}
			}
		}
		else
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				StartGame();
		}

		window.clear();

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		if (gameBegan && !paused)
		{
			if (timer > delay)
			{
				timer = 0;
				Update();
			}

			if (eaten)
			{
				eatTimer += time;

				if (eatTimer > delay * 3 || dead)
				{
					eatTimer = 0;
					eaten = false;
				}
			}
		}

		for (int i = 0; i < COUNT; i++)
		{
			for (int j = 0; j < COUNT; j++)
			{
				emptySprite.setPosition(sf::Vector2f(i * SIZE, j * SIZE));
				window.draw(emptySprite);
			}
		}

		int snakeHead = snake[0];
		snakeHeadSprite.setPosition(cells[snakeHead].x * SIZE, cells[snakeHead].y * SIZE);
		window.draw(snakeHeadSprite);

		for (int i = 1; i < snake.size(); i++)
		{
			int snakePart = snake[i];

			if (!eaten)
			{
				snakeBodySprite.setPosition(cells[snakePart].x * SIZE, cells[snakePart].y * SIZE);
				window.draw(snakeBodySprite);
			}
			else
			{
				snakeHeadSprite.setPosition(cells[snakePart].x * SIZE, cells[snakePart].y * SIZE);
				window.draw(snakeHeadSprite);
			}
		}

		appleSprite.setPosition(cells[apple].x * SIZE, cells[apple].y * SIZE);
		window.draw(appleSprite);

		for (int i = 0; i < COUNT * COUNT; i++)
		{
			sf::Text text(std::to_string(i), font);
			text.setCharacterSize(SIZE / 4);

			if (cells[i].solid)
				text.setFillColor(sf::Color::Blue);
			else
				text.setFillColor(sf::Color::Black);

			text.setPosition(cells[i].x * SIZE + text.getCharacterSize() / 2, cells[i].y * SIZE + text.getCharacterSize() / 2);

			window.draw(text);
		}

		window.display();
	}

	return 0;
}