#include <SFML/Graphics.hpp>
#include <set>
#include <iostream>

const int SIZE = 32;
const int COUNT = 4;
const int SCALE = 4;
const int BORDER = 2;

int width = COUNT * SIZE + BORDER;
int height = COUNT * SIZE + BORDER;

char snakeDirection;
int snakeLength;
std::vector<int> snake;

bool snakeAI = true;
char snakeDirections[COUNT * COUNT];

int apple;
int newTail;
bool eaten;

bool gameBegan, paused, dead, beatGame;

struct Cell
{
	int x, y;
	char direction = 0;
};

Cell cells[COUNT * COUNT];

sf::Texture emptyTexture, appleTexture, snakeHeadTexture, snakeBodyTexture;
sf::Sprite emptySprite, appleSprite, snakeHeadSprite, snakeBodySprite;
sf::Font font;

int ManhattanDistance(int a, int b, Cell grid[])
{
	return abs(grid[b].x - grid[a].x) + abs(grid[b].y - grid[a].y);
}

char CalculateDirection(int a, int b, Cell grid[])
{
	int x = grid[b].x - grid[a].x;
	int y = grid[b].y - grid[a].y;

	if (x == 0)
	{
		if (y < 0)
			return 'N';
		else if (y > 0)
			return 'S';
	}
	else if (y == 0)
	{
		if (x < 0)
			return 'W';
		else if (x > 0)
			return 'E';
	}

	return 0;
}

char OpppositeOf(char direction)
{
	char oppositeDirection = 0;

	if (direction == 'N')
		oppositeDirection = 'S';
	else if (direction == 'S')
		oppositeDirection = 'N';
	else if (direction == 'W')
		oppositeDirection = 'E';
	else if (direction == 'E')
		oppositeDirection = 'W';

	return oppositeDirection;
}

std::vector<char> DirectionToNeighbors(int root, Cell grid[])
{
	std::vector<char> neighbors;

	if (grid[root].y > 0)
	{
		neighbors.push_back('N'); //root - COUNT
	}

	if (grid[root].x < COUNT - 1)
	{
		neighbors.push_back('E'); //root + 1
	}

	if (grid[root].y < COUNT - 1)
	{
		neighbors.push_back('S'); //root + COUNT
	}

	if (grid[root].x > 0)
	{
		neighbors.push_back('W'); //root - 1
	}


	return neighbors;
}

void SpawnApple()
{
	if (snake.size() == (COUNT * COUNT) - 1)
	{
		apple = -1;
		beatGame = true;
		return;
	}

	do
	{
		apple = rand() % (COUNT * COUNT);
	} while (std::find(snake.begin(), snake.end(), apple) != snake.end());
}

void SetDirections(char* directions, Cell grid[])
{
	//Hamiltonian Cycle that works only with a grid size of 4 rows and columns of cells
	directions[0] = 'E';
	directions[1] = 'E';
	directions[2] = 'E';
	directions[3] = 'S';
	directions[4] = 'N';
	directions[5] = 'W';
	directions[6] = 'W';
	directions[7] = 'S';
	directions[8] = 'E';
	directions[9] = 'E';
	directions[10] = 'N';
	directions[11] = 'S';
	directions[12] = 'N';
	directions[13] = 'W';
	directions[14] = 'W';
	directions[15] = 'W';
}

void Update()
{
	int head = snake[0];
	int tail = snake[snake.size() - 1];
	int north, east, south, west;

	for (int i = snake.size() - 1; i > 0; i--)
		snake[i] = snake[i - 1];

	if (snakeAI)
	{
		snakeDirection = snakeDirections[head];
	}

	switch (snakeDirection)
	{
	case 'N':
		north = head - COUNT;

		if (cells[head].y > 0 && std::find(snake.begin(), snake.end(), north) == snake.end())
		{
			snake[0] = north;
		}
		else
			dead = true;

		break;
	case 'E':
		east = head + 1;

		if (cells[head].x < COUNT - 1 && std::find(snake.begin(), snake.end(), east) == snake.end())
		{
			snake[0] = east;
		}
		else
			dead = true;

		break;
	case 'S':
		south = head + COUNT;

		if (cells[head].y < COUNT - 1 && std::find(snake.begin(), snake.end(), south) == snake.end())
		{
			snake[0] = south;
		}
		else
			dead = true;

		break;
	case 'W':
		west = head - 1;

		if (cells[head].x > 0 && std::find(snake.begin(), snake.end(), west) == snake.end())
		{
			snake[0] = west;
		}
		else
			dead = true;

		break;
	}

	head = snake[0];
	cells[head].direction = snakeDirection;

	if (snake[0] == apple)
	{
		SpawnApple();

		int tail = snake[snake.size() - 1];
		switch (cells[tail].direction)
		{
		case 'N':
			newTail = tail + COUNT;
			break;
		case 'S':
			newTail = tail - COUNT;
			break;
		case 'W':
			newTail = tail + 1;
			break;
		case 'E':
			newTail = tail - 1;
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
	beatGame = false;

	for (int i = 0; i < COUNT; i++)
	{
		for (int j = 0; j < COUNT; j++)
		{
			cells[i * COUNT + j].x = j;
			cells[i * COUNT + j].y = i;
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

	SpawnApple();

	if (snakeAI)
	{
		Cell temp[COUNT * COUNT];
		std::copy(std::begin(cells), std::end(cells), std::begin(temp));

		SetDirections(snakeDirections, temp);
	}
}

int main()
{
	srand(time(NULL));

	auto desktop = sf::VideoMode::getDesktopMode();

	sf::RenderWindow window(sf::VideoMode(width, height), "Snake", sf::Style::Titlebar | sf::Style::Close);
	window.setSize(sf::Vector2u(width * SCALE, height * SCALE));
	window.setPosition(sf::Vector2i(desktop.width / 2 - window.getSize().x / 2, desktop.height / 2 - window.getSize().y / 2));
	window.setKeyRepeatEnabled(false);

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
			else if (event.type == sf::Event::KeyPressed)
			{
				if (!snakeAI)
				{
					if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up)
					{
						snakeDirection = 'N';

						if (!gameBegan)
							gameBegan = true;
					}
					else if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right)
					{
						snakeDirection = 'E';

						if (!gameBegan)
							gameBegan = true;
					}
					else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
					{
						snakeDirection = 'S';

						if (!gameBegan)
							gameBegan = true;
					}
					else if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left)
					{
						snakeDirection = 'W';

						if (!gameBegan)
							gameBegan = true;
					}
					else if (event.key.code == sf::Keyboard::Space)
					{
						if (gameBegan)
						{
							if (dead)
								StartGame();
							else
								paused = !paused;
						}
					}
				}
				else
				{
					if (event.key.code == sf::Keyboard::Space)
					{
						if (gameBegan)
						{
							if (dead || beatGame)
								StartGame();
							else
								paused = !paused;
						}
						else
							gameBegan = true;
					}
				}
			}
		}

		window.clear();

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		if (gameBegan && !paused && !dead && !beatGame)
		{
			if (timer > delay)
			{
				timer = 0;
				Update();
			}

			if (eaten)
			{
				eatTimer += time;

				if (eatTimer > delay * 3)
				{
					eatTimer = 0;
					eaten = false;
				}
			}
		}

		if (dead)
		{
			if (eaten)
			{
				eatTimer = 0;
				eaten = false;
			}
		}

		for (int i = 0; i < COUNT; i++)
		{
			for (int j = 0; j < COUNT; j++)
			{
				emptySprite.setPosition(sf::Vector2f(BORDER + i * SIZE, BORDER + j * SIZE));
				window.draw(emptySprite);
			}
		}

		int snakeHead = snake[0];
		snakeHeadSprite.setPosition(BORDER + cells[snakeHead].x * SIZE, BORDER + cells[snakeHead].y * SIZE);
		window.draw(snakeHeadSprite);

		for (int i = 1; i < snake.size(); i++)
		{
			int snakePart = snake[i];

			if (!eaten)
			{
				snakeBodySprite.setPosition(BORDER + cells[snakePart].x * SIZE, BORDER + cells[snakePart].y * SIZE);
				window.draw(snakeBodySprite);
			}
			else
			{
				snakeHeadSprite.setPosition(BORDER + cells[snakePart].x * SIZE, BORDER + cells[snakePart].y * SIZE);
				window.draw(snakeHeadSprite);
			}
		}

		appleSprite.setPosition(BORDER + cells[apple].x * SIZE, BORDER + cells[apple].y * SIZE);
		window.draw(appleSprite);

		for (int i = 0; i < COUNT * COUNT; i++)
		{
			sf::Text text;
			text.setFont(font);
			text.setCharacterSize(SIZE / 4);

			if (snakeAI)
				text.setString(snakeDirections[i]);

			text.setFillColor(sf::Color::Black);
			text.setPosition(cells[i].x * SIZE + text.getCharacterSize() / 2, cells[i].y * SIZE + text.getCharacterSize() / 2);

			window.draw(text);
		}

		window.display();
	}

	return 0;
}