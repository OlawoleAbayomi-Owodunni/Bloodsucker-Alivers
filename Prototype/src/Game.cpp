#include "Game.h"
#include <iostream>

// Our target FPS
static double const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_width, ScreenSize::s_height, 32), "Prototype", sf::Style::Default)
{
	srand(time(nullptr));
	init();
}

////////////////////////////////////////////////////////////
void Game::init()
{
	// Really only necessary is our target FPS is greater than 60.
	m_window.setVerticalSyncEnabled(true);

	m_holder.acquire("playerSprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/plyrPh.png"));
	m_holder.acquire("enemySprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/enmyPh.png"));
	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));

	for (int i = 0; i < 6; i++)
	{
		m_enemies.push_back(new Enemy(m_holder["enemySprite"]));
	}

	for (auto enemy : m_enemies)
	{
		enemy->initialisePosition();
	}

	if (!m_arialFont.loadFromFile("BebasNeue.otf"))
	{
		std::cout << "Error loading font file";
	}

	m_currentLevel = 1;

	sf::Texture& bgTexture = m_holder["starterAtlas"];

	bgSprite.setTexture(bgTexture);
	bgSprite.setTextureRect(IntRect{ 0,192,1600,900 });
	bgSprite.setOrigin(800, 500);
	bgSprite.setPosition(800, 500);

#ifdef TEST_FPS
	x_updateFPS.setFont(m_arialFont);
	x_updateFPS.setPosition(20, 300);
	x_updateFPS.setCharacterSize(24);
	x_updateFPS.setFillColor(sf::Color::White);
	x_drawFPS.setFont(m_arialFont);
	x_drawFPS.setPosition(20, 350);
	x_drawFPS.setCharacterSize(24);
	x_drawFPS.setFillColor(sf::Color::White);
#endif
}

////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	
	sf::Time timePerFrame = sf::seconds(1.0f / FPS); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame.asMilliseconds()); //60 fps
#ifdef TEST_FPS
			x_secondTime += timePerFrame;
			x_updateFrameCount++;
			if (x_secondTime.asSeconds() > 1)
			{
				std::string updatesPS = "UPS " + std::to_string(x_updateFrameCount - 1);
				x_updateFPS.setString(updatesPS);
				std::string drawsPS = "DPS " + std::to_string(x_drawFrameCount);
				x_drawFPS.setString(drawsPS);
				x_updateFrameCount = 0;
				x_drawFrameCount = 0;
				x_secondTime = sf::Time::Zero;
			}
#endif
		}
		render(); // as many as possible
#ifdef TEST_FPS
		x_drawFrameCount++;
#endif
	}
}

////////////////////////////////////////////////////////////
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
		processGameEvents(event);
	}
}

////////////////////////////////////////////////////////////
void Game::processGameEvents(sf::Event& event)
{
	// check if the event is a a mouse button release
	if (sf::Event::KeyPressed == event.type)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			m_window.close();
			break;
		default:
			break;
		}
	}
}

void Game::checkCollisions()
{

	for (auto enemy : m_enemies)
	{
		if (CollisionDetection::playerEnemyCollision(m_player, enemy))
		{
			m_player.decreaseHealth();
		}

		for (auto weapon : m_player.getWeapon())
		{
			for (auto bullet : weapon->getBullet())
			{
				if (CollisionDetection::bulletEnemyCollision(bullet, enemy))
				{
					enemy->decreaseHealth(bullet->getDamage());

					if (enemy->getHealth() < 0)
					{
						if (rand() % 4 != 0)	// 75% chance enemy spawns an XP Orb on death
						{
							m_xpOrbs.push_back(new XPOrb(m_holder["starterAtlas"], enemy->getPosition()));
						}

						if (rand() % 10 == 0)	// 10% chance enemy spawns Health Pickup on death
						{
							m_pickups.push_back(new Pickup(enemy->getPosition(), PickupType::Health));
						}

						enemy->initialisePosition();
					}
				}
			}
		}
	}

	for (auto it = m_xpOrbs.begin(); it != m_xpOrbs.end();)
	{
		if (CollisionDetection::playerOrbCollision(m_player, *it))
		{
			m_player.increaseXP();

			delete* it; // Delete the orb object
			it = m_xpOrbs.erase(it); // Remove the orb pointer from the vector
		}
		else 
		{
			++it;
		}
	}

	for (auto it = m_pickups.begin(); it != m_pickups.end();)
	{
		if (CollisionDetection::playerPickupCollision(m_player, *it))
		{
			m_player.increaseHealth();

			delete* it; // Delete the pickup object
			it = m_pickups.erase(it); // Remove the pickup pointer from the vector
		}
		else
		{
			++it;
		}
	}
}

void Game::addEnemies()
{
	if (m_player.getLevel() > m_currentLevel)
	{
		for (int i = 0; i < 3; i++)
		{
			m_enemies.push_back(new Enemy(m_holder["enemySprite"]));
		}
		m_currentLevel++;
	}
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	m_player.update(dt, m_enemies);

	for (auto enemy : m_enemies)
	{
		enemy->update(dt, m_player);
	}

	std::cout << m_enemies.size() << std::endl;

	addEnemies();

	checkCollisions();
}

////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
	m_window.draw(bgSprite);

	for (auto pickup : m_pickups)
	{
		pickup->render(m_window);
	}

	for (auto orb : m_xpOrbs)
	{
		orb->render(m_window);
	}

	for (auto enemy : m_enemies)
	{
		enemy->render(m_window);
	}
	
	m_player.render(m_window);

#ifdef TEST_FPS
	m_window.draw(x_updateFPS);
	m_window.draw(x_drawFPS);
#endif
	m_window.display();
}