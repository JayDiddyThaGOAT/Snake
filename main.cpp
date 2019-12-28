#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <limits>
#include <stack>
#include <iostream>

const int SIZE = 32;
const int COUNT = 12;
const int SCALE = 1;
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

bool displayText;

struct Cell
{
	int x, y;
	char direction = 0;
	int parent = -1;
	bool wall = false;
	sf::Sprite sprite;
	sf::Text indexText, directionText;
};

Cell cells[COUNT * COUNT];

sf::Texture emptyTexture, appleTexture, snakeHeadTexture, snakeBodyTexture;
sf::Font font;

int ManhattanDistance(int a, int b, Cell grid[])
{
	return abs(grid[b].x - grid[a].x) + abs(grid[b].y - grid[a].y);
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

std::unordered_map<char, int> GetNeighbors(int root, Cell grid[])
{
	std::unordered_map<char, int> neighbors;

	if (grid[root].x < COUNT - 1)
	{
		neighbors['E'] = root + 1;
	}

	if (grid[root].x > 0)
	{
		neighbors['W'] = root - 1;
	}

	if (grid[root].y > 0)
	{
		neighbors['N'] = root - COUNT;
	}
	if (grid[root].y < COUNT - 1)
	{
		neighbors['S'] = root + COUNT;
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

	cells[apple].sprite.setTexture(appleTexture);
	eaten = true;
}

void Update()
{
	int north, east, south, west;

	for (int i = snake.size() - 1; i > 0; i--)
	{
		snake[i] = snake[i - 1];
	}

	if (snakeAI)
	{
		snakeDirection = snakeDirections[snake[0]];
	}

	switch (snakeDirection)
	{
	case 'N':
		north = snake[0] - COUNT;

		if (cells[snake[0]].y > 0 && std::find(snake.begin(), snake.end(), north) == snake.end())
		{
			snake[0] = north;
		}
		else
			dead = true;

		break;
	case 'E':
		east = snake[0] + 1;

		if (cells[snake[0]].x < COUNT - 1 && std::find(snake.begin(), snake.end(), east) == snake.end())
		{
			snake[0] = east;
		}
		else
			dead = true;

		break;
	case 'S':
		south = snake[0] + COUNT;

		if (cells[snake[0]].y < COUNT - 1 && std::find(snake.begin(), snake.end(), south) == snake.end())
		{
			snake[0] = south;
		}
		else
			dead = true;

		break;
	case 'W':
		west = snake[0] - 1;

		if (cells[snake[0]].x > 0 && std::find(snake.begin(), snake.end(), west) == snake.end())
		{
			snake[0] = west;
		}
		else
			dead = true;

		break;
	}

	cells[snake[0]].direction = snakeDirection;

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

	for (int i = 0; i < COUNT; i++)
	{
		for (int j = 0; j < COUNT; j++)
		{
			std::vector<int>::iterator iter = std::find(snake.begin(), snake.end(), i * COUNT + j);
			if (iter == snake.end())
			{
				if (i * COUNT + j == apple)
					continue;

				if (cells[i * COUNT + j].sprite.getTexture() != &emptyTexture)
					cells[i * COUNT + j].sprite.setTexture(emptyTexture);
			}
			else
			{
				if (!dead)
				{
					if (!eaten)
					{
						if (iter == snake.begin())
							cells[i * COUNT + j].sprite.setTexture(snakeHeadTexture);
						else
							cells[i * COUNT + j].sprite.setTexture(snakeBodyTexture);
					}
					else
						cells[i * COUNT + j].sprite.setTexture(snakeHeadTexture);
				}
				else
				{
					if (cells[i * COUNT + j].sprite.getTexture() != &snakeBodyTexture)
						cells[i * COUNT + j].sprite.setTexture(snakeBodyTexture);
				}
			}
		}
	}
}


bool WithinGrid(int root, Cell grid[])
{
	return grid[root].x < COUNT - 1 || grid[root].y < COUNT - 1 || grid[root].x > 0 || grid[root].y > 0;
}

std::vector<int> CalculateMinPath(int start, int end, Cell grid[])
{
	bool visited[COUNT * COUNT];
	for (int i = 0; i < COUNT * COUNT; i++)
		visited[i] = false;

	std::vector<int> queue;
	queue.push_back(start);
	visited[start] = true;

	std::vector<int> path;

	while (!queue.empty())
	{
		int current = -1;
		int minDistance = 1000000;
		int minIndex = 0;
		for (int i = 0; i < queue.size(); i++)
		{
			int location = queue[i];
			int distance = ManhattanDistance(location, end, grid);
			if (distance < minDistance)
			{
				current = location;
				minIndex = i;
				minDistance = distance;
			}
		}
		queue.erase(queue.begin() + minIndex);

		if (current == end)
		{
			while (grid[current].parent != -1)
			{
				path.insert(path.begin(), current);
				current = grid[current].parent;
			}

			path.insert(path.begin(), current);

			for (auto& location : path)
			{
				grid[location].parent = -1;
			}

			return path;
		}

		std::unordered_map<char, int> currentNeighbors = GetNeighbors(current, grid);
		for (auto neighbor : currentNeighbors)
		{
			char direction = neighbor.first;
			int location = neighbor.second;

			if (!visited[location] && !grid[location].wall)
			{
				grid[location].parent = current;
				queue.push_back(location);
				visited[location] = true;
			}
		}
	}

	path.clear();
	return path;
}

std::vector<int> CalculateMaxPath(int start, int end, Cell grid[])
{
	std::vector<int> path = CalculateMinPath(start, end, grid);
	if (path.size() == 0)
		return path;

	bool visited[COUNT * COUNT];
	for (int i = 0; i < COUNT * COUNT; i++)
		visited[i] = std::find(path.begin(), path.end(), i) != path.end();

	int i = 0;
	int j = 1;
	while (true)
	{
		int pi = path[i];
		int pj = path[j];

		std::vector<int> extension(2);

		if (abs(pj - pi) == 1)
		{
			bool northInBounds = WithinGrid(pi + COUNT, grid) && WithinGrid(pj + COUNT, grid) && ManhattanDistance(pi, pi + COUNT, grid) == 1 && ManhattanDistance(pj, pj + COUNT, grid) == 1;
			bool northCanExtend = !(visited[pi + COUNT] && visited[pj + COUNT]) && std::find(path.begin(), path.end(), pi + COUNT) == path.end() && std::find(path.begin(), path.end(), pj + COUNT) == path.end();
			bool northEmptySpace = !(grid[pi + COUNT].wall || grid[pj + COUNT].wall);

			bool southInBounds = WithinGrid(pi - COUNT, grid) && WithinGrid(pj - COUNT, grid) && ManhattanDistance(pi, pi - COUNT, grid) == 1 && ManhattanDistance(pj, pj - COUNT, grid) == 1;
			bool southCanExtend = !(visited[pi - COUNT] && visited[pj - COUNT]) && std::find(path.begin(), path.end(), pi - COUNT) == path.end() && std::find(path.begin(), path.end(), pj - COUNT) == path.end();
			bool southEmptySpace = !(grid[pi - COUNT].wall || grid[pj - COUNT].wall);

			//Check if you can extend to the south...
			if (northInBounds && northCanExtend && northEmptySpace)
			{
				extension[0] = pi + COUNT;
				extension[1] = pj + COUNT;
				path.insert(path.begin() + (i + 1), extension.begin(), extension.end());
				visited[pi + COUNT] = true;
				visited[pj + COUNT] = true;
			}
			//Otherwise, check if you can extended to the north
			else if (southInBounds && southCanExtend && southEmptySpace)
			{
				extension[0] = pi - COUNT;
				extension[1] = pj - COUNT;
				path.insert(path.begin() + (i + 1), extension.begin(), extension.end());
				visited[pi - COUNT] = true;
				visited[pj - COUNT] = true;
			}
		}
		else if (abs(pj - pi) == COUNT)
		{
			bool eastInBounds = WithinGrid(pi + 1, grid) && WithinGrid(pj + 1, grid) && ManhattanDistance(pi, pi + 1, grid) == 1 && ManhattanDistance(pj, pj + 1, grid) == 1;
			bool eastCanExtend = !(visited[pi + 1] && visited[pj + 1]) && std::find(path.begin(), path.end(), pi + 1) == path.end() && std::find(path.begin(), path.end(), pj + 1) == path.end();
			bool eastEmptySpace = !(grid[pi + 1].wall || grid[pj + 1].wall);

			bool westInBounds = WithinGrid(pi - 1, grid) && WithinGrid(pj - 1, grid) && ManhattanDistance(pi, pi - 1, grid) == 1 && ManhattanDistance(pj, pj - 1, grid) == 1;
			bool westCanExtend = !(visited[pi - 1] && visited[pj - 1]) && std::find(path.begin(), path.end(), pi - 1) == path.end() && std::find(path.begin(), path.end(), pj - 1) == path.end();
			bool westEmptySpace = !(grid[pi - 1].wall || grid[pj - 1].wall);

			if (eastInBounds && eastCanExtend && eastEmptySpace)
			{
				extension[0] = pi + 1;
				extension[1] = pj + 1;
				path.insert(path.begin() + (i + 1), extension.begin(), extension.end());
				visited[pi + 1] = true;
				visited[pj + 1] = true;
			}
			else if (westInBounds && westCanExtend && westEmptySpace)
			{
				extension[0] = pi - 1;
				extension[1] = pj - 1;
				path.insert(path.begin() + (i + 1), extension.begin(), extension.end());
				visited[pi - 1] = true;
				visited[pj - 1] = true;
			}
		}

		if (extension[0] == 0 && extension[1] == 0)
		{
			i = j;
			j++;

			if (j == path.size())
				break;
		}
		else
		{
			i = 0;
			j = 1;
		}
	}

	return path;
}

std::vector<int> BuildHamiltonianCycle(Cell grid[])
{
	std::vector<int> path;

	int startPoint = snake[0];

	std::unordered_map<char, int> startNeighbors = GetNeighbors(startPoint, grid);
	std::unordered_map<char, int>::iterator iter = startNeighbors.begin();
	std::advance(iter, rand() % startNeighbors.size());
	
	int midPoint = startNeighbors[iter->first];
	int endPoint = GetNeighbors(midPoint, grid)[iter->first];

	grid[midPoint].wall = true;
	path = CalculateMaxPath(startPoint, endPoint, grid);
	grid[midPoint].wall = false;

	path.push_back(midPoint);
	path.push_back(startPoint);

	return path;
}

void MapDirectionsFromPath(char* directions, std::vector<int>& path)
{
	int i = 0;
	int j = 1;

	while (j < path.size())
	{
		int pi = path[i];
		int pj = path[j];

		switch (pj - pi)
		{
		case 1:
			directions[pi] = 'E';
			break;
		case -1:
			directions[pi] = 'W';
			break;
		case COUNT:
			directions[pi] = 'S';
			break;
		case -COUNT:
			directions[pi] = 'N';
			break;
		}

		i = j;
		j++;
	}
}

void StartGame()
{
	srand(time(NULL));

	paused = false;
	gameBegan = false;
	dead = false;
	beatGame = false;

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

	for (int i = 0; i < COUNT; i++)
	{
		for (int j = 0; j < COUNT; j++)
		{
			cells[i * COUNT + j].x = j;
			cells[i * COUNT + j].y = i;
			cells[i * COUNT + j].sprite.setPosition(BORDER + cells[i * COUNT + j].x * SIZE, BORDER + cells[i * COUNT + j].y * SIZE);

			if (i * COUNT + j == snake[0])
				cells[i * COUNT + j].sprite.setTexture(snakeHeadTexture);
			else
			{
				if (i * COUNT + j != apple)
					cells[i * COUNT + j].sprite.setTexture(emptyTexture);
			}
		}
	}

	if (snakeAI)
	{
		std::vector<int> path = BuildHamiltonianCycle(cells);
		MapDirectionsFromPath(snakeDirections, path);
	}

	for (int i = 0; i < COUNT * COUNT; i++)
	{
		sf::Text directionText;
		directionText.setFont(font);
		directionText.setCharacterSize(SIZE / 4);

		sf::Text indexText;
		indexText.setFont(font);
		indexText.setCharacterSize(SIZE / 4);

		directionText.setString(snakeDirections[i]);
		indexText.setString(std::to_string(i));

		directionText.setFillColor(sf::Color::Black);
		indexText.setFillColor(sf::Color::Black);

		directionText.setPosition(cells[i].x * SIZE + directionText.getCharacterSize() * 1.5, cells[i].y * SIZE + directionText.getCharacterSize());

		if (i >= 100)
			indexText.setPosition(cells[i].x * SIZE + indexText.getCharacterSize() - 2.5, cells[i].y * SIZE + indexText.getCharacterSize() * 2.5);
		else if (i >= 10 && i < 100)
			indexText.setPosition(cells[i].x * SIZE + indexText.getCharacterSize(), cells[i].y * SIZE + indexText.getCharacterSize() * 2.5);
		else if (i < 10)
			indexText.setPosition(cells[i].x * SIZE + indexText.getCharacterSize() * 1.5, cells[i].y * SIZE + indexText.getCharacterSize() * 2.5);

		cells[i].indexText = indexText;
		cells[i].directionText = directionText;
	}
}

int main()
{
	auto desktop = sf::VideoMode::getDesktopMode();

	sf::RenderWindow window(sf::VideoMode(width, height), "Snake", sf::Style::Titlebar | sf::Style::Close);
	window.setSize(sf::Vector2u(width * SCALE, height * SCALE));
	window.setPosition(sf::Vector2i(desktop.width / 2 - window.getSize().x / 2, desktop.height / 2 - window.getSize().y / 2));
	window.setKeyRepeatEnabled(false);

	emptyTexture.loadFromFile("Sprites/Empty.png");
	appleTexture.loadFromFile("Sprites/Apple.png");
	snakeHeadTexture.loadFromFile("Sprites/SnakeHead.png");
	snakeBodyTexture.loadFromFile("Sprites/SnakeBody.png");

	font.loadFromFile("Font/ARCADE_I.TTF");

	sf::Clock clock;
	float timer = 0, delay;
	float eatTimer = 0;

	if (snakeAI)
		delay = 0.01f;
	else
		delay = 0.1f;

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
				if (event.key.code == sf::Keyboard::Escape)
					window.close();

				if (event.key.code == sf::Keyboard::T)
					displayText = !displayText;

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

				if (eatTimer > 0.3)
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
				window.draw(cells[i * COUNT + j].sprite);

				if (displayText)
				{
					window.draw(cells[i * COUNT + j].indexText);
					window.draw(cells[i * COUNT + j].directionText);
				}
			}
		}

		window.display();
	}

	return 0;
}