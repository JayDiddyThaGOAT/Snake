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

bool snakeAI = true;
std::vector<int> snakePath;
std::string snakePathDirections;
int snakePathIndex;

int apple;
int newTail;
bool eaten;

bool gameBegan, paused, dead;

struct Cell
{
	int x, y;
	char direction;
	bool solid;
};

Cell cells[COUNT * COUNT];

sf::Texture emptyTexture, appleTexture, snakeHeadTexture, snakeBodyTexture;
sf::Sprite emptySprite, appleSprite, snakeHeadSprite, snakeBodySprite;
sf::Font font;

int ManhattanDistance(int a, int b, Cell grid[])
{
	return abs(grid[b].x - grid[a].x) + abs(grid[b].y - grid[a].y);
}

std::vector<int> NeighborsOf(int root, Cell grid[])
{
	std::vector<int> neighbors;

	if (grid[root - COUNT].y >= 0)
		neighbors.push_back(root - COUNT);

	if (grid[root + 1].x < COUNT - 1)
		neighbors.push_back(root + 1);

	if (grid[root + COUNT].y < COUNT - 1)
		neighbors.push_back(root + COUNT);

	if (grid[root - 1].x >= 0)
		neighbors.push_back(root - 1);


	return neighbors;
}

std::vector<int> ShortestPathBetween(int start, int end, Cell grid[])
{
	std::vector<int> path;

	std::stack<int> open;
	open.push(start);

	while (!open.empty())
	{
		int current = open.top();
		open.pop();

		path.push_back(current);

		if (current == end)
			return path;

		std::vector<int> currentNeighbors = NeighborsOf(current, grid);
		int minNeighbor = -1;
		int minDistance = INT_MAX;
		for (auto& neighbor : currentNeighbors)
		{
			int distance = ManhattanDistance(neighbor, end, grid);
			if (distance < minDistance)
			{
				minNeighbor = neighbor;
				minDistance = distance;
			}
		}

		if (minNeighbor < 0)
			break;

		open.push(minNeighbor);
	}

	path.clear();
	return path;
}

std::string GetPathDirections(std::vector<int> path, Cell grid[])
{
	std::string directions;

	int i = 0;
	int j = 1;

	while (j < path.size())
	{
		int k = path[i];
		int l = path[j];

		int x = grid[l].x - grid[k].x;
		int y = grid[l].y - grid[k].y;

		if (x == 0)
		{
			if (y == -1)
				directions.append("N");
			else if (y == 1)
				directions.append("S");
		}
		else if (y == 0)
		{
			if (x == -1)
				directions.append("W");
			else if (x == 1)
				directions.append("E");
		}

		i++;
		j++;
	}

	return directions;
}

void SpawnApple()
{
	do
	{
		apple = rand() % (COUNT * COUNT);
	} while (cells[apple].solid);
	cells[apple].solid = false;
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
		if (snakePathIndex < snakePathDirections.length())
		{
			snakeDirection = snakePathDirections.at(snakePathIndex);
			snakePathIndex++;
		}
	}

	switch (snakeDirection)
	{
	case 'N':
		north = head - COUNT;

		if (cells[head].y > 0 && !cells[north].solid)
		{
			cells[tail].solid = false;
			cells[head].solid = false;
			snake[0] = north;
		}
		else
			dead = true;

		break;
	case 'E':
		east = head + 1;

		if (cells[head].x < COUNT - 1 && !cells[east].solid)
		{
			cells[tail].solid = false;
			cells[head].solid = false;
			snake[0] = east;
		}
		else
			dead = true;

		break;
	case 'S':
		south = head + COUNT;

		if (cells[head].y < COUNT - 1 && !cells[south].solid)
		{
			cells[tail].solid = false;
			cells[head].solid = false;
			snake[0] = south;
		}
		else
			dead = true;

		break;
	case 'W':
		west = head - 1;

		if (cells[head].x > 0 && !cells[west].solid)
		{
			cells[tail].solid = false;
			cells[head].solid = false;
			snake[0] = west;
		}
		else
			dead = true;

		break;
	}

	head = snake[0];
	cells[head].direction = snakeDirection;
	for (auto& snakePart : snake)
		cells[snakePart].solid = true;

	if (head == apple)
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

	for (int i = 0; i < COUNT; i++)
	{
		for (int j = 0; j < COUNT; j++)
		{
			cells[i * COUNT + j].x = j;
			cells[i * COUNT + j].y = i;
			cells[i * COUNT + j].solid = false;
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
	cells[snake[0]].solid = true;

	SpawnApple();

	snakePathIndex = 0;
	if (snakeAI)
	{
		Cell temp[COUNT * COUNT];
		std::copy(std::begin(cells), std::end(cells), std::begin(temp));
		snakePath = ShortestPathBetween(snake[0], apple, temp);
		snakePathDirections = GetPathDirections(snakePath, temp);
	}
}

int main()
{
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(width, height), "Snake", sf::Style::Titlebar | sf::Style::Close);
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
							if (dead)
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

		if (gameBegan && !paused && !dead)
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
			for (int i = 0; i < COUNT * COUNT; i++)
				cells[i].solid = false;

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