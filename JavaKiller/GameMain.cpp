#include "pch.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "EnemyArena.h"
#include "TextureHolder.h"
#include "bullet.h"
#include "Enemy.h"
#include "Pickup.h"


int main()
{
	TextureHolder holder;

	//States of game
	enum class State
	{
		PAUSED, LEVELING_UP, GAME_OVER, PLAYING
	};

	State state = State::GAME_OVER;
	//Automatic download resolution info
	sf::Vector2f resolution;
	resolution.x = sf::VideoMode::getDesktopMode().width;
	resolution.y = sf::VideoMode::getDesktopMode().height;
	double factor = (1920 / resolution.x + 1080 / resolution.y) / 2;

	sf::RenderWindow window(sf::VideoMode(resolution.x, resolution.y), "JavaKiller", sf::Style::Fullscreen);
	//window.setVerticalSyncEnabled(true);
	sf::View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));
	
	//Some variables - i think there names told us all about it.
	sf::Clock clock;
	sf::Time gameTimeTotal;

	sf::Vector2f mouseWorldPosition; //Position of mouse in game

	sf::Vector2i mouseScreenPosition; //Position of mouse on screen

	Player player;

	sf::IntRect arena;

	sf::VertexArray background;

	sf::Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

	int numEnemies;
	int numEnemiesAlive;
	Enemy* enemies = nullptr;

	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1;
	sf::Time lastPressed;

	window.setMouseCursorVisible(false);// We can disable visibility of cursor and we can see onl
	sf::Sprite spriteCrosshair;
	sf::Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");

	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	Pickup healthPickup(1);
	Pickup ammoPickup(2);

	int score = 0;
	int hiScore = 0;

	sf::Sprite spriteGameOver;
	sf::Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	sf::Vector2u textureSize1(textureGameOver.getSize());
	sf::Vector2f scaleFactorBackground;						//Scaling object to current window resolution
	scaleFactorBackground.x = resolution.x / textureSize1.x;
	scaleFactorBackground.y = resolution.y / textureSize1.y;
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setScale(scaleFactorBackground);
	spriteGameOver.setPosition(0, 0);

	sf::Sprite spriteClose;
	sf::Texture textureClose = TextureHolder::GetTexture("graphics/close.png");
	sf::Vector2u textureSize2(textureClose.getSize());
	sf::Vector2f scaleFactorClose;						//Scaling object to current window resolution
	scaleFactorClose.x = resolution.x / textureSize2.x;
	scaleFactorClose.y = resolution.y / textureSize2.y;
	spriteClose.setTexture(textureClose);
	spriteClose.setScale(scaleFactorClose);
	spriteClose.setPosition(0, 0);

	sf::Vector2f scaleText = scaleFactorClose; //Defining scale factor

	sf::View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	sf::Sprite spriteAmmoIcon;
	sf::Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(20 * scaleText.x, 980 * scaleText.y);

	sf::Sprite spritePegiIcon;
	sf::Texture texturePegiIcon = TextureHolder::GetTexture("graphics/pegiIcon.png");
	sf::Vector2u textureSize3(texturePegiIcon.getSize());
	sf::Vector2f scaleFactorPegi;						//Scaling object to current window resolution
	scaleFactorPegi.x = resolution.x / textureSize3.x;
	scaleFactorPegi.y = resolution.y / textureSize3.y;
	spritePegiIcon.setTexture(texturePegiIcon);
	spritePegiIcon.setScale(scaleFactorPegi);
	spritePegiIcon.setPosition(0, 0);

	sf::Sprite spriteLogo;
	sf::Texture textureLogo = TextureHolder::GetTexture("graphics/LogoMecenas.png");
	sf::Vector2u textureSize4(textureLogo.getSize());
	sf::Vector2f scaleFactorLogo;						//Scaling object to current window resolution
	scaleFactorLogo.x = resolution.x / textureSize4.x;
	scaleFactorLogo.y = resolution.y / textureSize4.y;
	spriteLogo.setTexture(textureLogo);
	spriteLogo.setScale(scaleFactorLogo);
	spriteLogo.setPosition(0, 0);

	sf::Font font;
	font.loadFromFile("fonts/B.ttf");

	sf::Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155 * scaleText.x);
	pausedText.setFillColor(sf::Color::White);
pausedText.setPosition(300*scaleText.x, 400*scaleText.y);
	pausedText.setString(" Press Enter \n to continue\nor ESC to Exit");
	

	sf::Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(100 * scaleText.x);
	gameOverText.setFillColor(sf::Color::White);
	gameOverText.setPosition(400*scaleText.x, 850*scaleText.y);
	gameOverText.setString("Press Enter to play");

	sf::Text titleText;
	//sf::FloatRect titleTextRect = titleText.getLocalBounds();
	titleText.setFont(font);
	titleText.setCharacterSize(140 * scaleText.x);
	titleText.setFillColor(sf::Color::White);
	titleText.setPosition(30*scaleText.x, 105*scaleText.y);
	//titleText.setOrigin(titleTextRect.left + titleTextRect.width / 2.0f, titleTextRect.top + titleTextRect.height / 2.0f);
	//titleText.setPosition(sf::Vector2f(resolution.x / 2.0f, 100));
	titleText.setString("#include \"JAVA KILLER\"");

	sf::Text infoText;
	infoText.setFont(font);
	infoText.setCharacterSize(120 * scaleText.x);
	infoText.setFillColor(sf::Color::White);
	infoText.setPosition(30*scaleText.x, 105*scaleText.y);
	infoText.setString("#include \"select_upgrade\"");

	sf::Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(30 * scaleText.x);
	levelUpText.setFillColor(sf::Color::White);
	levelUpText.setPosition(50*scaleText.x, 850*scaleText.y);
	std::stringstream levelUpStream;
	levelUpStream <<
		"1 - Increased rate of fire									   4 - Increased run speed" <<
		"\n2 - Increased clip size (next reload)					        5 - More and better health pickups	" <<
		"\n3 - Increased max health								         6 - More and better ammo pickups";

	levelUpText.setString(levelUpStream.str());

	sf::Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(55 * scaleText.x);
	ammoText.setFillColor(sf::Color::White);
	ammoText.setPosition(200*scaleText.x, 980*scaleText.y);

	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(55 * scaleText.x);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(20*scaleText.x, 0);

	std::ifstream inputFile("C:/Doukmenty/JavaKiller/Saves/scores.txt");
	if (inputFile.is_open())
	{
		inputFile >> hiScore;
		inputFile.close();
	}

	sf::Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(55 * scaleText.x);
	hiScoreText.setFillColor(sf::Color::White);
	hiScoreText.setPosition(1400*scaleText.x, 0);
	std::stringstream s;
	s << "Hi Score: " << hiScore;
	hiScoreText.setString(s.str());

	sf::Text enemiesRemainingText;
	enemiesRemainingText.setFont(font);
	enemiesRemainingText.setCharacterSize(55 * scaleText.x);
	enemiesRemainingText.setFillColor(sf::Color::White);
	enemiesRemainingText.setPosition(1500 * scaleText.x, 980 * scaleText.y);
	enemiesRemainingText.setString("Enemies: 100");

	int level = 0;
	sf::Text levelNumberText;
	levelNumberText.setFont(font);
	levelNumberText.setCharacterSize(55 * scaleText.x);
	levelNumberText.setFillColor(sf::Color::White);
	levelNumberText.setPosition(760 * scaleText.x, 0);
	//levelNumberText.setPosition(1250, 980);
	levelNumberText.setString("Level: 0");

	sf::RectangleShape healthBar;
	healthBar.setFillColor(sf::Color::Red);
	healthBar.setPosition(450 * scaleText.x, 980 * scaleText.x);

	int framesSinceLastHUDUpdate = 0;
	int fpsMeasurementFrameInterval = 1000;

	sf::SoundBuffer hitBuffer;
	hitBuffer.loadFromFile("sound/hit.wav");
	sf::Sound hit;
	hit.setBuffer(hitBuffer);

	sf::SoundBuffer splatBuffer;
	splatBuffer.loadFromFile("sound/splat.wav");
	sf::Sound splat;
	splat.setBuffer(splatBuffer);

	sf::SoundBuffer shootBuffer;
	shootBuffer.loadFromFile("sound/shoot.wav");
	sf::Sound shoot;
	shoot.setBuffer(shootBuffer);

	sf::SoundBuffer reloadBuffer;
	reloadBuffer.loadFromFile("sound/reload.wav");
	sf::Sound reload;
	reload.setBuffer(reloadBuffer);

	sf::SoundBuffer reloadFailedBuffer;
	reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
	sf::Sound reloadFailed;
	reloadFailed.setBuffer(reloadFailedBuffer);

	sf::SoundBuffer powerupBuffer;
	powerupBuffer.loadFromFile("sound/powerup.wav");
	sf::Sound powerup;
	powerup.setBuffer(powerupBuffer);

	sf::SoundBuffer pickupBuffer;
	pickupBuffer.loadFromFile("sound/pickup.wav");
	sf::Sound pickup;
	pickup.setBuffer(pickupBuffer);

	sf::SoundBuffer menuBuffer;
	menuBuffer.loadFromFile("sound/menu.wav");
	sf::Sound menu;
	menu.setBuffer(menuBuffer);

	sf::SoundBuffer exitBuffer;
	exitBuffer.loadFromFile("sound/exit.wav");
	sf::Sound exit;
	exit.setBuffer(exitBuffer);

	sf::SoundBuffer failBuffer;
	failBuffer.loadFromFile("sound/fail.wav");
	sf::Sound fail;
	fail.setBuffer(failBuffer);

	sf::SoundBuffer pegiBuffer;
	pegiBuffer.loadFromFile("sound/pegiSound.wav");
	sf::Sound pegi;
	pegi.setBuffer(pegiBuffer);

	sf::SoundBuffer introBuffer;
	introBuffer.loadFromFile("sound/intro.wav");
	sf::Sound intro;
	intro.setBuffer(introBuffer);
	
	pegi.play();
	window.draw(spritePegiIcon);
	window.display();
	sleep(sf::seconds(3));
	intro.play();
	window.draw(spriteLogo);
	window.display();
	sleep(sf::seconds(3));

	menu.play();
	while (window.isOpen()) //Main game loop
	{
		
		sf::Event event;

		while (window.pollEvent(event)) //Support pool events
		{

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Return &&
					state == State::PLAYING)
				{
					state = State::PAUSED;
				}
				else if (event.key.code == sf::Keyboard::Return &&
					state == State::PAUSED)
				{
					state = State::PLAYING;
					clock.restart();
				}
				else if (event.key.code == sf::Keyboard::Return &&
					state == State::GAME_OVER)
				{
					state = State::LEVELING_UP;
					level = 0;
					score = 0;

					currentBullet = 0;
					bulletsSpare = 24;
					bulletsInClip = 6;
					clipSize = 6;
					fireRate = 1;

					player.resetPlayerStats();
				}
				if (state == State::PLAYING)
				{
					if (event.key.code == sf::Keyboard::R)
					{
						if (bulletsSpare >= clipSize)
						{
							bulletsInClip = clipSize;
							bulletsSpare -= clipSize;
							reload.play();
						}
						else if (bulletsSpare > 0)
						{
							bulletsInClip = bulletsSpare;
							bulletsSpare = 0;
							reload.play();
						}
						else
						{
							reloadFailed.play();
						}
					}
				}
			}

		}


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			window.draw(spriteClose);
			window.display();
			fail.play();
			sleep(sf::seconds(1));
			exit.play();
			sleep(sf::seconds(2));
			window.close();
		}

		if (state == State::PLAYING)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				player.moveUp();
			}
			else
			{
				player.stopUp();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				player.moveDown();
			}
			else
			{
				player.stopDown();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				player.moveLeft();
			}
			else
			{
				player.stopLeft();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				player.moveRight();
			}
			else
			{
				player.stopRight();
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0)
				{
					bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);

					currentBullet++;
					if (currentBullet > 99)
					{
						currentBullet = 0;
					}

					lastPressed = gameTimeTotal;

					shoot.play();

					bulletsInClip--;
				}
			}
		}

		if (state == State::LEVELING_UP) //Support of upgrading items
		{
			if (event.key.code == sf::Keyboard::Num1)
			{
				fireRate++;
				state = State::PLAYING;
			}
			if (event.key.code == sf::Keyboard::Num2)
			{
				clipSize += clipSize;
				state = State::PLAYING;
			}
			if (event.key.code == sf::Keyboard::Num3)
			{
				player.upgradeHealth();
				state = State::PLAYING;
			}
			if (event.key.code == sf::Keyboard::Num4)
			{
				player.upgradeSpeed();
				state = State::PLAYING;
			}
			if (event.key.code == sf::Keyboard::Num5)
			{
				healthPickup.upgrade();
				state = State::PLAYING;
			}
			if (event.key.code == sf::Keyboard::Num6)
			{
				ammoPickup.upgrade();
				state = State::PLAYING;
			}

			if (state == State::PLAYING) //Preparing arena for the play
			{
				level++;
				arena.width = 500 * level;
				arena.height = 500 * level;
				arena.left = 0;
				arena.top = 0;

				int tileSize = createBackground(background, arena);

				//int tileSize = 50;

				player.spawn(arena, resolution, tileSize);

				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);

				numEnemies = 5 * level;

				delete[] enemies;
				enemies = createHorde(numEnemies, arena);
				numEnemiesAlive = numEnemies;

				powerup.play();

				clock.restart();
			}

		}

		if (state == State::PLAYING)
		{
			sf::Time dt = clock.restart();

			gameTimeTotal += dt;

			float dtAsSeconds = dt.asSeconds();

			mouseScreenPosition = sf::Mouse::getPosition();

			mouseWorldPosition = window.mapPixelToCoords(sf::Mouse::getPosition(), mainView);

			spriteCrosshair.setPosition(mouseWorldPosition);

			player.update(dtAsSeconds, sf::Mouse::getPosition());

			sf::Vector2f playerPosition(player.getCenter());

			mainView.setCenter(player.getCenter());

			for (int i = 0; i < numEnemies; i++)
			{
				if (enemies[i].isAlive())
				{
					enemies[i].update(dt.asSeconds(), playerPosition);
				}
			}

			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					bullets[i].update(dtAsSeconds);
				}
			}

			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);

			for (int i = 0; i < 100; i++) //Checkout collision bullets with enemies
			{
				for (int j = 0; j < numEnemies; j++)
				{
					if (bullets[i].isInFlight() && enemies[j].isAlive())
					{
						if (bullets[i].getPosition().intersects(enemies[j].getPosition()))
						{
							bullets[i].stop();

							if (enemies[j].hit())
							{
								score += 10;
								if (score >= hiScore)
								{
									hiScore = score;
								}
								numEnemiesAlive--;

								if (numEnemiesAlive == 0)
								{
									state = State::LEVELING_UP;
								}
							}
							splat.play();
						}
					}
				}
			}

			for (int i = 0; i < numEnemies; i++) //Checkout collision player with enemies
			{
				if (player.getPosition().intersects(enemies[i].getPosition()) && enemies[i].isAlive())
				{
					if (player.hit(gameTimeTotal))
					{
						hit.play();
					}
					if (player.getHealth() <= 0)
					{
						state = State::GAME_OVER;

						std::ofstream outputFile("C:/Doukmenty/JavaKiller/Saves/scores.txt");
						outputFile << hiScore;
						outputFile.close();
					}
				}
			}
			//Checkout intersects with bonuses
			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned())
			{
				player.increaseHealthLevel(healthPickup.gotIt());
				pickup.play();
			}

			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned())
			{
				bulletsSpare += ammoPickup.gotIt();
				pickup.play();
			}

			healthBar.setSize(sf::Vector2f(player.getHealth() * 3, 70));
			framesSinceLastHUDUpdate++;

			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval) //Showing and updating hud info
			{
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssLevel;
				std::stringstream ssEnemiesAlive;

				ssAmmo << bulletsInClip << "/" << bulletsSpare;
				ammoText.setString(ssAmmo.str());

				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				ssHiScore << "Hi Score: " << hiScore;
				hiScoreText.setString(ssHiScore.str());

				ssLevel << "Level: " << level;
				levelNumberText.setString(ssLevel.str());

				ssEnemiesAlive << "Enemies: " << numEnemiesAlive;
				enemiesRemainingText.setString(ssEnemiesAlive.str());

				framesSinceLastHUDUpdate = 0;
			}
		}
	
		if (state == State::PLAYING) //Drawing all elements of the game
		{
			window.clear();

			window.setView(mainView);

			window.draw(background, &textureBackground);

			for (int i = 0; i < numEnemies; i++)
			{
				window.draw(enemies[i].getSprite());
			}

			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					window.draw(bullets[i].getShape());
				}
			}

			window.draw(player.getSprite());

			if (ammoPickup.isSpawned())
			{
				window.draw(ammoPickup.getSprite());
			}

			if (healthPickup.isSpawned())
			{
				window.draw(healthPickup.getSprite());
			}

			window.draw(spriteCrosshair);

			window.setView(hudView);

			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(levelNumberText);
			window.draw(enemiesRemainingText);
		}

		if (state == State::LEVELING_UP)
		{
			window.draw(spriteGameOver);
			window.draw(levelUpText);
			window.draw(infoText);
		}
		if (state == State::PAUSED)
		{
			window.draw(pausedText);
		}
		if (state == State::GAME_OVER)
		{
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(titleText);
			window.draw(scoreText);
			window.draw(hiScoreText);

		}
		window.display();
	}

	delete[] enemies;

	return 0;
}


