#include "Game.h"
#include <iostream>

// Our target FPS
static double const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_width, ScreenSize::s_height, 32), "Prototype", sf::Style::Default), 
	m_view(sf::FloatRect(0, 0, m_window.getSize().x, m_window.getSize().y))
{
	srand(time(nullptr));

	m_window.setView(m_view);

	init();
}

////////////////////////////////////////////////////////////
void Game::init()
{
	// Really only necessary is our target FPS is greater than 60.
	m_window.setVerticalSyncEnabled(true);

	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));
	m_holder.acquire("mapSprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/Map.png"));

	for (int i = 0; i < 10; i++)
	{
		m_enemies.push_back(new Enemy(m_holder["starterAtlas"]));
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

	sf::Texture& bgTexture = m_holder["mapSprite"];
	bgSprite.setTexture(bgTexture);
	bgSprite.setTextureRect(IntRect{ 0,0,3200,1800 });
	bgSprite.setOrigin(800, 500);
	bgSprite.setPosition(800, 500);

	m_currentGamemode = Gamemode::Gameplay;

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
			if (m_currentGamemode == Gamemode::Gameplay)
			{
				m_currentGamemode = Gamemode::Pause;
			}
			else
			{
				m_window.close();
			}
			break;
		case sf::Keyboard::Enter:
			if (m_currentGamemode == Gamemode::Menu || m_currentGamemode == Gamemode::Pause)
			{
				m_currentGamemode = Gamemode::Gameplay;
			}
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
							m_pickups.push_back(new Pickup(m_holder["starterAtlas"], enemy->getPosition(), PickupType::Health));
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
			m_enemies.push_back(new Enemy(m_holder["starterAtlas"]));
			m_currentGamemode = Gamemode::Upgrade;
		}
		m_currentLevel++;
	}
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	if (m_currentGamemode == Gamemode::Gameplay)
	{
		m_player.update(dt, m_enemies);

		sf::Vector2f targetPosition = m_player.getPosition();
		sf::Vector2f interpolatedPosition = m_view.getCenter();

		float speed = dt/1000 * 10.0f;

		interpolatedPosition.x = interpolatedPosition.x + (targetPosition.x - interpolatedPosition.x) * speed;
		interpolatedPosition.y = interpolatedPosition.y + (targetPosition.y - interpolatedPosition.y) * speed;

		m_view.setCenter(interpolatedPosition);
		m_window.setView(m_view);

		//cout << m_view.getCenter().x << "	" << m_view.getCenter().y << "\n";


		for (auto enemy : m_enemies)
		{
			enemy->update(dt, m_player);
		}
		
		for (auto orb : m_xpOrbs)
		{
			orb->update(dt);
		}

		for (auto pickup : m_pickups)
		{
			pickup->update(dt);
		}

		//std::cout << m_enemies.size() << std::endl;

		addEnemies();

		checkCollisions();
	}
	
	if (m_currentGamemode == Gamemode::Upgrade)
	{
		m_player.levelUp(m_currentGamemode);
	}
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