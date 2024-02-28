#include "Game.h"
#include <iostream>

// Our target FPS
static double const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_width, ScreenSize::s_height, 32), "Prototype", sf::Style::Default),
	m_playerCamera(sf::FloatRect(0, 0, m_window.getSize().x, m_window.getSize().y)),
	m_menuCamera(FloatRect(0, 0, m_window.getSize().x, m_window.getSize().y))
{
	srand(time(nullptr));
	init();

	switch (m_currentGamemode) {
	case Gamemode::Menu:
		m_window.setView(m_menuCamera);
		break;
	case Gamemode::Gameplay:
		m_window.setView(m_playerCamera);
	}
}

////////////////////////////////////////////////////////////
void Game::init()
{
	// Really only necessary is our target FPS is greater than 60.
	m_window.setVerticalSyncEnabled(true);

	m_currentGamemode = Gamemode::Menu;

	//THOR
	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));
	m_holder.acquire("mapSprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/Map.png"));
	m_holder.acquire("mainMenuBG", thor::Resources::fromFile<sf::Texture>("resources/sprites/menu.png"));
	m_holder.acquire("UIAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/UI_Atlas.png"));
	m_holder.acquire("obstacleAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/ObstacleAtlas.png"));
	
	//SOUND
	if (!m_pickupSoundBuffer.loadFromFile("resources/sounds/orb_pickup.wav"))
	{
		std::cout << "error loading orb pick up sound";
	}
	m_pickupSound.setBuffer(m_pickupSoundBuffer);
	m_pickupSound.setVolume(15.0f);

	//ENEMIES
	for (int i = 0; i < 12; i++)
	{
		m_enemies.push_back(new Enemy(m_holder["starterAtlas"]));
	}
	for (auto enemy : m_enemies)
	{
		enemy->initialisePosition(m_player.getPosition());
	}
	m_currentLevel = 1;

	//OBSTACLES
	for (int i = 0; i < 2; i++)
	{
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock1));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock2));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock3));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Tree));
	}

	//FONT
	if (!m_arialFont.loadFromFile("BebasNeue.otf"))
	{
		std::cout << "Error loading font file";
	}

	//BACKGROUND
	sf::Texture& bgTexture = m_holder["mapSprite"];
	bgSprite.setTexture(bgTexture);
	bgSprite.setTextureRect(IntRect{ 0,0,3200,1800 });
	bgSprite.setOrigin(800, 500);
	bgSprite.setPosition(800, 500);

	//MAIN MENU INITIALISER
	Texture& mainMenuBgTexture = m_holder["mainMenuBG"];
	Texture& UITexture = m_holder["UIAtlas"];
	menuBgSprite.setTexture(mainMenuBgTexture);
	menuBgSprite.setTextureRect(IntRect{ 0,0,1600,900 });
	menuBgSprite.setOrigin(1600.0f / 2.0f, 900.0f / 2.0f);
	menuBgSprite.setPosition(m_menuCamera.getCenter());

	m_menuButtons.push_back(new Button(ButtonType::Play, UITexture, m_arialFont, Vector2f(550, 535)));
	m_menuButtons.push_back(new Button(ButtonType::Tutorial, UITexture, m_arialFont, Vector2f(1050, 535)));
	m_menuButtons.push_back(new Button(ButtonType::Credits, UITexture, m_arialFont, Vector2f(550, 680)));
	m_menuButtons.push_back(new Button(ButtonType::Exit, UITexture, m_arialFont, Vector2f(1050, 680)));


	//PAUSE MENU INITIALISER 
	pauseBgSprite.setTexture(UITexture);
	pauseBgSprite.setTextureRect(IntRect{ 0, 1965, 600, 275 });
	pauseBgSprite.setOrigin(300, 275.0F / 2.0F);
	pauseBgSprite.setScale(2.5f, 2.5f);
	pauseBgSprite.setPosition(m_playerCamera.getCenter());

	m_pauseButtons.push_back(new Button(ButtonType::Resume, UITexture, m_arialFont, Vector2f(m_playerCamera.getCenter().x - 250, m_playerCamera.getCenter().y)));
	m_pauseButtons.push_back(new Button(ButtonType::ToMenu, UITexture, m_arialFont, Vector2f(m_playerCamera.getCenter().x + 250, m_playerCamera.getCenter().y)));

	//UPGRADE MENU INITIALISER
	levelUpBGSprite.setTexture(UITexture);
	levelUpBGSprite.setTextureRect(IntRect{ 0, 300, 800, 600 });
	levelUpBGSprite.setOrigin(400.0f, 300.0f);
	levelUpBGSprite.setScale(1.3f, 1.3f);
	levelUpBGSprite.setPosition(m_playerCamera.getCenter());
	//no need to setup buttons in constructor since they are randomised everytime

	//CURSOR INITIALISER
	m_cursorPos = 0;
	m_cursorSprite.setTexture(UITexture);
	m_cursorSprite.setTextureRect(IntRect{ 0, 1664, 550, 150 });
	m_cursorSprite.setOrigin(275.0f, 75.0f);
	m_cursorSprite.setScale(0.9f, 1.0f);
	m_cursorSprite.setPosition(m_menuButtons[m_cursorPos]->getPositon());
	m_cursorButtonType = m_menuButtons[m_cursorPos]->getType();
}



#pragma region USELESS FUNCTIONS (LIKE RUN AND PROCESS EVENTS)
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
		}
		render(); // as many as possible
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

#pragma endregion


void Game::processGameEvents(sf::Event& event)
{
	// check if the event is a a mouse button release
	if (sf::Event::KeyPressed == event.type)
	{
		switch (m_currentGamemode)
		{
#pragma region Menu input handling
		case Gamemode::Menu:
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				m_window.close();

			case sf::Keyboard::Up:
				m_cursorPos--;
				break;
			case sf::Keyboard::Down:
				m_cursorPos++;
				break;
			case sf::Keyboard::Left:
				m_cursorPos--;
				break;
			case sf::Keyboard::Right:
				m_cursorPos++;
				break;

			case sf::Keyboard::Enter:
				switch (m_cursorButtonType)
				{
				case ButtonType::Play:
					m_currentGamemode = Gamemode::Gameplay;
					break;
					//CASE FOR TUTORIAL AND CASE FOR CREDITS
				case ButtonType::Exit:
					m_window.close();
					break;
				}
			}

			if (m_cursorPos > (static_cast<int>(m_menuButtons.size()) - 1)) { m_cursorPos = 0; }
			if (m_cursorPos < 0) { m_cursorPos = static_cast<int>(m_menuButtons.size()) - 1; }

			m_cursorSprite.setPosition(m_menuButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_menuButtons[m_cursorPos]->getType();
			break;
#pragma endregion

#pragma region Pause input handling
		case Gamemode::Pause:
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				m_currentGamemode = Gamemode::Gameplay;
				break;

			case sf::Keyboard::Left:
				m_cursorPos--;
				break;
			case sf::Keyboard::Right:
				m_cursorPos++;
				break;

			case Keyboard::Enter:
				switch (m_cursorButtonType)
				{
				case ButtonType::Resume:
					m_currentGamemode = Gamemode::Gameplay;
					break;
				case ButtonType::ToMenu:
					m_currentGamemode = Gamemode::Menu;
					m_cursorSprite.setPosition(m_menuButtons[m_cursorPos]->getPositon());
					m_cursorButtonType = m_menuButtons[m_cursorPos]->getType();
					break;
				}
				break;
			default:
				break;
			}

			if (m_cursorPos > (static_cast<int>(m_pauseButtons.size()) - 1)) { m_cursorPos = 0; }
			if (m_cursorPos < 0) { m_cursorPos = static_cast<int>(m_pauseButtons.size()) - 1; }

			m_cursorSprite.setPosition(m_pauseButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_pauseButtons[m_cursorPos]->getType();
			break;

#pragma endregion

#pragma region Upgrade input handling
		case Gamemode::Upgrade:
			switch (event.key.code)
			{
			case sf::Keyboard::Up:
				m_cursorPos--;
				break;
			case sf::Keyboard::Down:
				m_cursorPos++;
				break;

			case Keyboard::Enter:
				switch (m_cursorButtonType)
				{
				case ButtonType::UpgradeHealth:
					m_player.upgradePlayer(PlayerUpgrade::Health);
					break;
				case ButtonType::UpgradeSpeed:
					m_player.upgradePlayer(PlayerUpgrade::Speed);
					break;
				case ButtonType::UpgradeXP:
					m_player.upgradePlayer(PlayerUpgrade::XP);
					break;
				case ButtonType::UpgradeArmor:
					m_player.upgradePlayer(PlayerUpgrade::Armor);
					break;
				case ButtonType::UpgradePistol:
					m_player.upgradeGun(WeaponType::Pistol); // go back to player and refreactor to allow for pistol and ar
					break;
				case ButtonType::UpgradeAR:
					m_player.upgradeGun(WeaponType::AssaultRifle);
					break;
				}
				createRandomWeapons();
				m_currentGamemode = Gamemode::CarePackage;
				break;
			default:
				break;
			}

			if (m_cursorPos > (static_cast<int>(m_upgradeButtons.size()) - 1)) { m_cursorPos = 0; }
			if (m_cursorPos < 0) { m_cursorPos = static_cast<int>(m_upgradeButtons.size()) - 1; }

			m_cursorSprite.setPosition(m_upgradeButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_upgradeButtons[m_cursorPos]->getType();
			break;

#pragma endregion

#pragma region Care Package input handling
		case Gamemode::CarePackage:
			switch (event.key.code)
			{
			case sf::Keyboard::Up:
				m_cursorPos--;
				break;
			case sf::Keyboard::Down:
				m_cursorPos++;
				break;

			case Keyboard::Enter:
				switch (m_cursorButtonType)
				{
				case ButtonType::GetPistol:
					m_player.giveWeapon(WeaponType::Pistol);
					break;
				case ButtonType::GetRifle:
					m_player.giveWeapon(WeaponType::AssaultRifle);
					break;
				}
				m_currentGamemode = Gamemode::Gameplay;
				break;
			default:
				break;
			}

			if (m_cursorPos > (static_cast<int>(m_weaponButtons.size()) - 1)) { m_cursorPos = 0; }
			if (m_cursorPos < 0) { m_cursorPos = static_cast<int>(m_weaponButtons.size()) - 1; }

			m_cursorSprite.setPosition(m_weaponButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_weaponButtons[m_cursorPos]->getType();
			break;

#pragma endregion

#pragma region Gameplay input handling
		case Gamemode::Gameplay:
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				m_currentGamemode = Gamemode::Pause;
				pauseBgSprite.setPosition(m_playerCamera.getCenter());
				m_pauseButtons[0]->setPosition(Vector2f(m_playerCamera.getCenter().x - 250, m_playerCamera.getCenter().y));
				m_pauseButtons[1]->setPosition(Vector2f(m_playerCamera.getCenter().x + 250, m_playerCamera.getCenter().y));
				m_cursorSprite.setPosition(m_pauseButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_pauseButtons[m_cursorPos]->getType();
				break;

			default:
				break;
			}
			break;
#pragma endregion

		}
	}
}


////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	//switching camera modes
	switch (m_currentGamemode) {
	case Gamemode::Menu:
		m_window.setView(m_menuCamera);
		break;
	case Gamemode::Gameplay:
		m_window.setView(m_playerCamera);
	}

#pragma region Gameplay Gamemode logic
	if (m_currentGamemode == Gamemode::Gameplay) //switching between screens
	{
		sf::Vector2f targetPosition = m_player.getPosition();
		sf::Vector2f interpolatedPosition = m_playerCamera.getCenter();

		float speed = dt / 1000 * 10.0f;

		interpolatedPosition.x = interpolatedPosition.x + (targetPosition.x - interpolatedPosition.x) * speed;
		interpolatedPosition.y = interpolatedPosition.y + (targetPosition.y - interpolatedPosition.y) * speed;

		if (interpolatedPosition.x < ScreenSize::s_width / 2.0f)
		{
			interpolatedPosition.x = ScreenSize::s_width / 2.0f;
		}
		else if (interpolatedPosition.x > ScreenSize::s_mapWidth - ScreenSize::s_width / 2.0f)
		{
			interpolatedPosition.x = ScreenSize::s_mapWidth - ScreenSize::s_width / 2.0f;
		}

		if (interpolatedPosition.y < ScreenSize::s_height / 2.0f)
		{
			interpolatedPosition.y = ScreenSize::s_height / 2.0f;
		}
		else if (interpolatedPosition.y > ScreenSize::s_mapHeight - ScreenSize::s_height / 2.0f)
		{
			interpolatedPosition.y = ScreenSize::s_mapHeight - ScreenSize::s_height / 2.0f;
		}

		m_playerCamera.setCenter(interpolatedPosition);
		m_window.setView(m_playerCamera);

		//cout << m_view.getCenter().x << "	" << m_view.getCenter().y << "\n";

		m_player.update(dt, m_playerCamera, m_enemies);

		for (auto enemy : m_enemies)
		{
			enemy->update(dt, m_player);
		}

		for (auto orb : m_xpOrbs)
		{
			orb->update(dt, m_player);
		}

		for (auto pickup : m_pickups)
		{
			pickup->update(dt);
		}

		//std::cout << m_enemies.size() << std::endl;

		levelUpSpawner();

		checkCollisions();
	}

#pragma endregion

	if (m_currentGamemode == Gamemode::Upgrade) //setup upgrade screen here
	{
		//m_player.levelUp(m_currentGamemode);
	}

	if (m_currentGamemode == Gamemode::Menu)
	{
		//Do menu based stuff
	}

	if (m_currentGamemode == Gamemode::Pause)
	{
		//Do pause based stuff
	}
}



////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
	
#pragma region GAMEPLAY
	if (m_currentGamemode == Gamemode::Gameplay || m_currentGamemode == Gamemode::Pause || m_currentGamemode == Gamemode::Upgrade || m_currentGamemode == Gamemode::CarePackage)
	{
		m_window.draw(bgSprite);

		for (auto obstacle : m_obstacles)
		{
			obstacle->renderBottom(m_window);
		}

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

		m_player.renderPlayer(m_window);
		m_player.renderHUD(m_window);

		for (auto obstacle : m_obstacles)
		{
			obstacle->renderTop(m_window);
		}

		//Render pause menu
		if (m_currentGamemode == Gamemode::Pause)
		{
			m_window.draw(pauseBgSprite);
			for (auto buttons : m_pauseButtons)
			{
				buttons->render(m_window);
			}
			m_window.draw(m_cursorSprite);
		}

		//Render level up menu
		if (m_currentGamemode == Gamemode::Upgrade)
		{
			m_window.draw(levelUpBGSprite);
			for (auto buttons : m_upgradeButtons)
			{
				buttons->render(m_window);
			}
			m_window.draw(m_cursorSprite);
		}

		if (m_currentGamemode == Gamemode::CarePackage)
		{
			m_window.draw(levelUpBGSprite);
			for (auto buttons : m_weaponButtons)
			{
				buttons->render(m_window);
			}
			m_window.draw(m_cursorSprite);
		}
	}
#pragma endregion

	if (m_currentGamemode == Gamemode::Menu)
	{
		m_window.draw(menuBgSprite);
		for (auto buttons : m_menuButtons) {
			buttons->render(m_window);
		}
		m_window.draw(m_cursorSprite);
	}


	m_window.display();
}

#pragma region COLLISION HANDLER
void Game::checkCollisions()
{
#pragma region Enemies
	for (auto enemy : m_enemies)
	{
		//Player to Enemy
		if (CollisionDetection::playerEnemyCollision(m_player, enemy))
		{
			m_player.decreaseHealth();
		}

		//Enemy to Dash
		if (CollisionDetection::playerDashEnemyCollision(m_player, enemy))
		{
			enemy->decreaseHealth(100.0f);

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

				enemy->initialisePosition(m_player.getPosition());
			}
		}

		//Bullet to Enemy
		for (auto weapon : m_player.getWeapon())
		{
			if (weapon->getType() == WeaponType::Pistol || weapon->getType() == WeaponType::AssaultRifle) // If bullet is meant to delete when collided with enemy
			{
				for (auto it = weapon->getBullets().begin(); it != weapon->getBullets().end();) // this line is the issue. it needs to be permanent. it gets destroyed so no work
				{
					if (CollisionDetection::bulletEnemyCollision((*it), enemy))
					{
						enemy->decreaseHealth((*it)->getDamage());

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

							enemy->playHitSound();

							enemy->initialisePosition(m_player.getPosition());

							delete* it; // Delete the bullet object
							it = weapon->getBullets().erase(it); // Remove the bullet pointer from the vector
						}
					}
					else
					{
						++it;
					}
				}
			}
			else
			{
				for (auto bullet  : weapon->getBullets())
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

							enemy->playHitSound();

							enemy->initialisePosition(m_player.getPosition());

						}
					}
				}
			}
		}
	}

#pragma endregion

#pragma region Orbs
	for (auto it = m_xpOrbs.begin(); it != m_xpOrbs.end();)
	{
		if (CollisionDetection::playerOrbCollision(m_player, *it))
		{
			m_player.rumbleStart();
			rumbleTimer.restart();

			m_player.playSound(m_pickupSound);
			m_player.increaseXP();

			delete* it; // Delete the orb object
			it = m_xpOrbs.erase(it); // Remove the orb pointer from the vector
		}
		else
		{
			++it;
		}
	}
	//cout << rumbleTimer.getElapsedTime().asSeconds() << "\n";

	if (rumbleTimer.getElapsedTime().asSeconds() > 0.1f) {
		m_player.rumbleStop();
	}

#pragma endregion

#pragma region Pickups
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

#pragma endregion

	for (auto obstacle : m_obstacles)
	{
		if (CollisionDetection::playerObstacleCollision(m_player, obstacle))
		{
			m_player.pushBack();
		}
	}

}
#pragma endregion

void Game::levelUpSpawner()
{
	if (m_player.getLevel() > m_currentLevel)
	{
		for (int i = 0; i < 6; i++)
		{
			m_enemies.push_back(new Enemy(m_holder["starterAtlas"]));
		}
		m_currentLevel++;
		m_currentGamemode = Gamemode::Upgrade;
		levelUpBGSprite.setPosition(m_playerCamera.getCenter());
		createRandomUpgrades();
	}
}

void Game::createRandomUpgrades()
{
	m_upgradeButtons.clear();

	ButtonType randomUpgradeButton1 = static_cast<ButtonType>((rand() % 6) + static_cast<int>(ButtonType::UpgradeHealth));
	ButtonType randomUpgradeButton2 = static_cast<ButtonType>((rand() % 6) + static_cast<int>(ButtonType::UpgradeHealth));
	while (randomUpgradeButton1 == randomUpgradeButton2) { randomUpgradeButton2 = static_cast<ButtonType>((rand() % 6) + static_cast<int>(ButtonType::UpgradeHealth)); }
	ButtonType randomUpgradeButton3 = static_cast<ButtonType>((rand() % 6) + static_cast<int>(ButtonType::UpgradeHealth));
	while (randomUpgradeButton3 == randomUpgradeButton1 || randomUpgradeButton3 == randomUpgradeButton2)
	{
		randomUpgradeButton3 = static_cast<ButtonType>((rand() % 6) + static_cast<int>(ButtonType::UpgradeHealth));
	}

	m_upgradeButtons.push_back(new Button(randomUpgradeButton1, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y - 180)));
	m_upgradeButtons.push_back(new Button(randomUpgradeButton2, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y)));
	m_upgradeButtons.push_back(new Button(randomUpgradeButton3, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y + 180)));
}

void Game::createRandomWeapons()
{
	m_weaponButtons.clear();

	ButtonType randomUpgradeButton1 = static_cast<ButtonType>((rand() % 2) + static_cast<int>(ButtonType::GetPistol));
	ButtonType randomUpgradeButton2 = static_cast<ButtonType>((rand() % 2) + static_cast<int>(ButtonType::GetPistol));
	while (randomUpgradeButton1 == randomUpgradeButton2) { randomUpgradeButton2 = static_cast<ButtonType>((rand() % 2) + static_cast<int>(ButtonType::GetPistol)); }

	m_weaponButtons.push_back(new Button(randomUpgradeButton1, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y - 150)));
	m_weaponButtons.push_back(new Button(randomUpgradeButton2, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y + 150)));
}