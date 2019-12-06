#include <SFML/Graphics.hpp>
#include <set>

const int SIZE = 32;
const int ROWS = 17;
const int COLS = 17;

int width = ROWS * SIZE;
int height = COLS * SIZE;

char snakeDirection;
int snakeLength = 3;
std::vector<int> snake;
bool dead = false;
bool snakeAI = false;

int apple;
int newTail;
bool eaten, playing;

struct Cell
{
	int x, y;
	char direction;
	bool visited;
	bool empty;
};

Cell cells[ROWS * COLS];

sf::Texture emptyTexture, appleTexture, snakeHeadTexture, snakeBodyTexture;
sf::Sprite emptySprite, appleSprite, snakeHeadSprite, snakeBodySprite;
sf::Font font;

void Update()
{
	if (dead)
		return;

	int tail = snake.size() - 1;
	cells[snake[tail]].empty = true;
	for (int i = snake.size() - 1; i > 0; i--)
		snake[i] = snake[i - 1];

	switch (snakeDirection)
	{
	case 'N':
		if (cells[snake[0]].y > 0)
			snake[0] -= ROWS;
		else
			dead = true;
		break;
	case 'S':
		if (cells[snake[0]].y < ROWS - 1)
			snake[0] += ROWS;
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
		if (cells[snake[0]].x < COLS - 1)
			snake[0]++;
		else
			dead = true;
		break;
	}

	if (!cells[snake[0]].empty && snake[0] != apple)
		dead = true;

	cells[snake[0]].visited = true;
	cells[snake[0]].empty = false;
	cells[snake[0]].direction = snakeDirection;

	if (snake[0] == apple)
	{
		eaten = true;

		do
		{
			apple = rand() % (ROWS * COLS);
		} while (!cells[apple].empty);
		cells[apple].empty = false;

		int tail = snake.size() - 1;
		switch (cells[snake[tail]].direction)
		{
		case 'N':
			newTail = snake[tail] + ROWS;
			break;
		case 'S':
			newTail = snake[tail] - ROWS;
			break;
		case 'W':
			newTail = snake[tail] + 1;
			break;
		case 'E':
			newTail = snake[tail] - 1;
			break;
		}

		snake.push_back(newTail);
		cells[newTail].empty = false;
		snakeLength = snake.size();
	}
}

void StartGame()
{
	playing = false;

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			cells[i * COLS + j].x = j;
			cells[i * COLS + j].y = i;
			cells[i * COLS + j].empty = true;
			cells[i * COLS + j].visited = false;
			cells[i + COLS + j].direction = 'X';
		}
	}

	snake.clear();

	int center = (ROWS * COLS) / 2;
	snake.push_back(center);

	snakeLength = 3;
	snakeDirection = 'N';
	for (int i = 1; i <= snakeLength; i++)
	{
		switch (snakeDirection)
		{
		case 'N':
			snake.push_back(center + (i * ROWS));
			cells[center + (i * ROWS)].empty = false;
			cells[center + (i * ROWS)].direction = snakeDirection;
			break;
		case 'S':
			snake.push_back(center - (i * ROWS));
			cells[center - (i * ROWS)].empty = false;
			cells[center - (i * ROWS)].direction = snakeDirection;
			break;
		case 'W':
			snake.push_back(center + i);
			cells[center + i].empty = false;
			cells[center + i].direction = snakeDirection;
			break;
		case 'E':
			snake.push_back(center - i);
			cells[center - i].empty = false;
			cells[center - i].direction = snakeDirection;
			break;
		}
	}

	cells[snake[0]].visited = true;
	cells[snake[0]].empty = false;
	cells[snake[0]].direction = snakeDirection;

	apple = rand() % snake[0];
	cells[apple].empty = false;

	dead = false;
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
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					if (!dead)
						playing = !playing;
				}
			}
		}

		window.clear();

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		if (!snakeAI)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				snakeDirection = 'N';
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				snakeDirection = 'S';
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				snakeDirection = 'W';
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				snakeDirection = 'E';
		}
		else
		{

		}

		if (dead)
		{
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				StartGame();
		}

		if (playing)
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

		for (int i = 0; i < COLS; i++)
		{
			for (int j = 0; j < ROWS; j++)
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

		for (int i = 0; i < ROWS * COLS; i++)
		{
			sf::Text text(std::to_string(i), font);
			text.setCharacterSize(SIZE / 4);

			if (!cells[i].empty)
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