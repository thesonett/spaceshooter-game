// Created on 04-02-2021 by Joy Saha
// @Author: TheSonett


#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <vector>

using namespace sf;
using namespace std;

// Structures of Space, Bullet, Player & Enemies

class Space
{
public:
	Sprite shape;

	Space(Texture* texture)
	{
		this->shape.setTexture(*texture);
		this->shape.setPosition(0.0f, 100.0f);
	}

	~Space() {}
};

class Bullet
{
public:
	Sprite shape;

	Bullet(Texture* texture, Vector2f pos)
	{
		this->shape.setTexture(*texture);
		this->shape.setScale(0.05f, 0.05f);
		this->shape.setPosition(pos);
	}

	~Bullet() {}
};

class Player
{
public:
	Sprite shape;
	Texture* texture;
	int HP;
	int HPMax;

	vector<Bullet> bullets;

	Player(Texture* texture)
	{
		this->HPMax = 10;
		this->HP = this->HPMax;
		this->texture = texture;
		this->shape.setTexture(*texture);
		this->shape.setScale(0.1f, 0.1f);
	}

	~Player() {}
};

class Enemy
{
public:
	Sprite shape;

	int HP;
	int HPMax;

	Enemy(Texture* texture, Vector2u windowSize)
	{
		this->HPMax = rand() % 3 + 1;
		this->HP = this->HPMax;
		this->shape.setTexture(*texture);
		this->shape.setScale(0.07f, 0.07f);
		this->shape.setRotation(270);
		this->shape.setPosition(windowSize.x - this->shape.getGlobalBounds().width, rand() % (int)(windowSize.y - this->shape.getGlobalBounds().height));
	}

	~Enemy() {}
};


int main()
{
	// Random number generator
	srand(time_t(NULL));

	// Window settings
	RenderWindow window(VideoMode(1260, 512), "Enemies Shooter", Style::Default);
	window.setFramerateLimit(60);

	//Init fonts
	Font font;
	font.loadFromFile("Fonts/Roboto-Regular.ttf");

	// Init Textures
	Texture spaceTex;
	spaceTex.loadFromFile("Textures/space.png");

	Texture playerTex;
	playerTex.loadFromFile("Textures/spaceship.png");

	Texture enemyTex;
	enemyTex.loadFromFile("Textures/enemy.png");

	Texture bulletTex;
	bulletTex.loadFromFile("Textures/missile.png");

	//Uer Interface init
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(30);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(10.f, 10.f);

	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(80);
	gameOverText.setFillColor(Color::Red);
	gameOverText.setPosition(400.f, window.getSize().y / 3.0f);
	gameOverText.setString("GAME OVER!");

	// Space init
	Space space(&spaceTex);

	// Player init
	int score = 0;
	Player player(&playerTex);
	int shootTimer = 15;

	Text hptext;
	hptext.setFont(font);
	hptext.setCharacterSize(12);
	hptext.setFillColor(Color::White);

	// Enemy initF
	int enemySpawnTimer = 0;
	vector<Enemy> enemies;

	Text eHptext;
	eHptext.setFont(font);
	eHptext.setCharacterSize(12);
	eHptext.setFillColor(Color::White);

	// Sound Effect
	SoundBuffer buffer;
	if (!buffer.loadFromFile("Sounds/sound2.wav"))
		return -1;

	Sound sound;
	sound.setBuffer(buffer);

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
				window.close();
		}
		if (player.HP > 0)
		{
			// Player
			if (Keyboard::isKeyPressed(Keyboard::W))
				player.shape.move(0.f, -10.f);
			if (Keyboard::isKeyPressed(Keyboard::A))
				player.shape.move(-10.f, 0.f);
			if (Keyboard::isKeyPressed(Keyboard::S))
				player.shape.move(0.f, 10.f);
			if (Keyboard::isKeyPressed(Keyboard::D))
				player.shape.move(10.f, 0.f);

			hptext.setPosition(player.shape.getPosition().x, player.shape.getPosition().y - hptext.getGlobalBounds().height);
			hptext.setString(to_string(player.HP) + "/" + to_string(player.HPMax));

			// Collision with Window
			if (player.shape.getPosition().x <= 0) // Left
				player.shape.setPosition(0.f, player.shape.getPosition().y);

			if (player.shape.getPosition().x >= window.getSize().x - player.shape.getGlobalBounds().width) // Right
				player.shape.setPosition(window.getSize().x - player.shape.getGlobalBounds().width, player.shape.getPosition().y);

			if (player.shape.getPosition().y <= 0) // Top
				player.shape.setPosition(player.shape.getPosition().x, 0.f);

			if (player.shape.getPosition().y >= window.getSize().y - player.shape.getGlobalBounds().height) // bottom
				player.shape.setPosition(player.shape.getPosition().x, window.getSize().y - player.shape.getGlobalBounds().height);

			// Update Controls
			if (shootTimer < 15)
			{
				shootTimer++;
			}

			if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= 15)
			{ // Shooting
				player.bullets.push_back(Bullet(&bulletTex, player.shape.getPosition()));
				sound.play();
				shootTimer = 0; // Reset Shoot Timer
			}

			// Bullets
			for (size_t i = 0; i < player.bullets.size(); i++)
			{
				// Move
				player.bullets[i].shape.move(20.f, 0.f);

				// Out of window bounds
				if (player.bullets[i].shape.getPosition().x > window.getSize().x)
				{
					player.bullets.erase(player.bullets.begin() + i);
					break;
				}

				// Enemy Collision
				for (size_t k = 0; k < enemies.size(); k++)
				{
					if (player.bullets[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds()))
					{
						if (enemies[k].HP <= 1)
						{
							score += enemies[k].HPMax;
							enemies.erase(enemies.begin() + k);
						}
						else
							enemies[k].HP--;

						player.bullets.erase(player.bullets.begin());
						break;
					}
				}
			}

			// Enemy
			if (enemySpawnTimer < 25)
			{
				enemySpawnTimer++;
			}

			if (enemySpawnTimer >= 25)
			{
				enemies.push_back(Enemy(&enemyTex, window.getSize()));
				enemySpawnTimer = 0;
			}

			for (size_t i = 0; i < enemies.size(); i++)
			{
				enemies[i].shape.move(-6.f, 0.f);

				if (enemies[i].shape.getPosition().x <= 0 - enemies[i].shape.getGlobalBounds().width)
				{
					enemies.erase(enemies.begin() + i);
					break;
				}

				if (enemies[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
				{
					enemies.erase(enemies.begin() + i);
					player.HP--;
					break;
				}
			}

			// UI Update
			scoreText.setString("Score : " + to_string(score));
		}
		window.clear();

		window.draw(space.shape);

		// Player
		window.draw(player.shape);

		// Bullets
		for (size_t i = 0; i < player.bullets.size(); i++)
		{
			window.draw(player.bullets[i].shape);
		}

		// Enemy
		for (size_t i = 0; i < enemies.size(); i++)
		{
			eHptext.setString(to_string(enemies[i].HP) + "/" + to_string(enemies[i].HPMax));
			eHptext.setPosition(enemies[i].shape.getPosition().x, enemies[i].shape.getPosition().y - eHptext.getGlobalBounds().height);
			window.draw(eHptext);
			window.draw(enemies[i].shape);
		}

		// UserInterface
		window.draw(scoreText);
		window.draw(hptext);

		if (player.HP <= 0)
			window.draw(gameOverText);

		window.display();
	}
	return 0;
}