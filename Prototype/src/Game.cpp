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

#pragma region THOR
	//THOR
	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));
	m_holder.acquire("mapSprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/Map.png"));
	m_holder.acquire("mainMenuBG", thor::Resources::fromFile<sf::Texture>("resources/sprites/menu.png"));
	m_holder.acquire("UIAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/UI_Atlas.png"));
	m_holder.acquire("obstacleAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/ObstacleAtlas.png"));
#pragma endregion

#pragma region SOUND
	//SOUND
	if (!m_pickupSoundBuffer.loadFromFile("resources/sounds/orb_pickup.wav"))
	{
		std::cout << "error loading orb pick up sound";
	}
	m_pickupSound.setBuffer(m_pickupSoundBuffer);
	m_pickupSound.setVolume(15.0f);
#pragma endregion

#pragma region ENEMIES & OBSTACLEES INITIALISER
	//ENEMIES
	for (int i = 0; i < 12; i++)
	{
		m_enemies.push_back(new Enemy(m_holder["starterAtlas"], m_player.getPosition(), EnemyType::Small));
	}
	m_enemies.push_back(new Enemy(m_holder["starterAtlas"], m_player.getPosition(), EnemyType::Boss));

	m_currentLevel = 1;

	//OBSTACLES
	for (int i = 0; i < 3; i++)
	{
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock1));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock2));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock3));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Tree));
	}
#pragma endregion

#pragma region OTHERS (Font, Rumble, Background)
	//FONT
	if (!m_arialFont.loadFromFile("BebasNeue.otf"))
	{
		std::cout << "Error loading font file";
	}

	//RUMBLE INITIALISER
	pIsRumbling = false;
	oIsRumbling = false;

	//BACKGROUND
	sf::Texture& bgTexture = m_holder["mapSprite"];
	bgSprite.setTexture(bgTexture);
	bgSprite.setTextureRect(IntRect{ 0,0,3200,1800 });
	bgSprite.setOrigin(800, 500);
	bgSprite.setPosition(800, 500);
#pragma endregion

#pragma region MENU INITIALISERS
	//MAIN MENU INITIALISER
	Texture& mainMenuBgTexture = m_holder["mainMenuBG"];
	Texture& UITexture = m_holder["UIAtlas"];
	menuBgSprite.setTexture(mainMenuBgTexture);
	menuBgSprite.setTextureRect(IntRect{ 0,0,1600,900 });
	menuBgSprite.setOrigin(1600.0f / 2.0f, 900.0f / 2.0f);
	menuBgSprite.setPosition(m_menuCamera.getCenter());

	m_menuButtons.push_back(new Button(ButtonType::Play, UITexture, m_arialFont, Vector2f(550, 200)));
	m_menuButtons.push_back(new Button(ButtonType::Tutorial, UITexture, m_arialFont, Vector2f(550, 400)));
	m_menuButtons.push_back(new Button(ButtonType::Credits, UITexture, m_arialFont, Vector2f(550, 600)));
	m_menuButtons.push_back(new Button(ButtonType::Exit, UITexture, m_arialFont, Vector2f(550, 800)));


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

#pragma endregion

}

void Game::startGame()
{
	m_currentGamemode = Gamemode::Gameplay;
	m_bossTimer.restart();
	m_bossSpawned = false;

	m_player.initialise();
	m_currentLevel = 1;

	////VECTOR INTIALISATION
	//ENEMY
	m_enemies.clear();
	for (int i = 0; i < 10; i++) {
		m_enemies.push_back(new Enemy(m_holder["starterAtlas"], m_player.getPosition(), EnemyType::Small));
	}

	m_xpOrbs.clear();
	m_pickups.clear();

	//OBSTACLES
	m_obstacles.clear();
	for (int i = 0; i < 3; i++)
	{
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock1));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock2));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock3));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Tree));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Building));
	}

	orbRumbleTimer.restart();
	pickupRumbleTimer.restart();
	enemyHitRumbleTimer.restart();

	oIsRumbling = false;
	pIsRumbling = false; 
	eIsRumbling = false;

	m_cursorPos = 0;
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
	if (sf::Event::KeyPressed == event.type || Event::JoystickButtonPressed == event.type || Event::JoystickMoved == event.type)
	{
		switch (m_currentGamemode)
		{
#pragma region Menu input handling
		case Gamemode::Menu:
#pragma region Keyboard Input
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

#pragma endregion

#pragma region Controller input
			//D-Pad pressed
			if (Event::JoystickMoved == event.type) {
				if (event.joystickMove.axis == sf::Joystick::PovY) // D-pad up/down
				{
					if (event.joystickMove.position == -100) // Up
						m_cursorPos++;
					else if (event.joystickMove.position == 100) // Down
						m_cursorPos--;
				}

			if (m_cursorPos > (static_cast<int>(m_menuButtons.size()) - 1)) { 
				m_cursorPos = 0; 
				m_cursorSprite.setPosition(m_menuButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_menuButtons[m_cursorPos]->getType();
			}
			if (m_cursorPos < 0) { 
				m_cursorPos = static_cast<int>(m_menuButtons.size()) - 1; 
				m_cursorSprite.setPosition(m_menuButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_menuButtons[m_cursorPos]->getType();
			}			
			
			m_cursorSprite.setPosition(m_menuButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_menuButtons[m_cursorPos]->getType();
			}

			//Button pressed
			if (Event::JoystickButtonPressed == event.type) {
				if (event.joystickButton.button == 0) { //0=A 7=Start
					switch (m_cursorButtonType)
					{
					case ButtonType::Play:
						m_currentGamemode = Gamemode::Gameplay;
						m_player.rumbleStop();
						startGame();
						break;
						//CASE FOR TUTORIAL AND CASE FOR CREDITS
					case ButtonType::Exit:
						m_player.rumbleStop();
						m_window.close();
						break;
					}
				}
			}
#pragma endregion

			break;
#pragma endregion

#pragma region Pause input handling
		case Gamemode::Pause:
#pragma region Keyboard Input

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
#pragma endregion

#pragma region Controller input
			//D-Pad pressed
			if (Event::JoystickMoved == event.type) {
				if (event.joystickMove.axis == sf::Joystick::PovX) // D-pad left/right
				{
					if (event.joystickMove.position == -100) // Left
						m_cursorPos--;
					else if (event.joystickMove.position == 100) // Right
						m_cursorPos++;
				}

			if (m_cursorPos > (static_cast<int>(m_pauseButtons.size()) - 1)) {
				m_cursorPos = 0;
				m_cursorSprite.setPosition(m_pauseButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_pauseButtons[m_cursorPos]->getType();
			}
			if (m_cursorPos < 0) {
				m_cursorPos = static_cast<int>(m_pauseButtons.size()) - 1;
				m_cursorSprite.setPosition(m_pauseButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_pauseButtons[m_cursorPos]->getType();
			}

			m_cursorSprite.setPosition(m_pauseButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_pauseButtons[m_cursorPos]->getType();
			}

			//Button pressed
			if (Event::JoystickButtonPressed == event.type) {
				if (event.joystickButton.button == 0) { //0=A 7=Start
					switch (m_cursorButtonType)
					{
					case ButtonType::Resume:
						m_currentGamemode = Gamemode::Gameplay;
						m_player.rumbleStop();
						break;
					case ButtonType::ToMenu:
						m_currentGamemode = Gamemode::Menu;
						m_cursorPos = 0;
						m_cursorSprite.setPosition(m_menuButtons[m_cursorPos]->getPositon());
						m_cursorButtonType = m_menuButtons[m_cursorPos]->getType();
						m_player.rumbleStop();
						break;
					}
				}
				if (event.joystickButton.button == 7) {
					m_currentGamemode = Gamemode::Gameplay;
				}
			}
#pragma endregion

			break;

#pragma endregion

#pragma region Upgrade input handling
		case Gamemode::Upgrade:
#pragma region Keyboard Input
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
				m_cursorPos = 0;
				m_cursorSprite.setPosition(m_weaponButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_weaponButtons[m_cursorPos]->getType();
				m_player.rumbleStop();
				break;
			default:
				break;
			}
#pragma endregion

#pragma region Controller input
			//D-Pad pressed
			if (Event::JoystickMoved == event.type) {
				if (event.joystickMove.axis == sf::Joystick::PovY) // D-pad up/down
				{
					if (event.joystickMove.position == -100) // Up
						m_cursorPos++;
					else if (event.joystickMove.position == 100) // Down
						m_cursorPos--;
				}

			if (m_cursorPos > (static_cast<int>(m_upgradeButtons.size()) - 1)) {
				m_cursorPos = 0;
				m_cursorSprite.setPosition(m_upgradeButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_upgradeButtons[m_cursorPos]->getType();
			}
			if (m_cursorPos < 0) {
				m_cursorPos = static_cast<int>(m_upgradeButtons.size()) - 1;
				m_cursorSprite.setPosition(m_upgradeButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_upgradeButtons[m_cursorPos]->getType();
			}

			m_cursorSprite.setPosition(m_upgradeButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_upgradeButtons[m_cursorPos]->getType();		
			}

			//Button pressed
			if (Event::JoystickButtonPressed == event.type) {
				if (event.joystickButton.button == 0) { //0=A 7=Start
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
					m_cursorPos = 0;
					m_cursorSprite.setPosition(m_weaponButtons[m_cursorPos]->getPositon());
					m_cursorButtonType = m_weaponButtons[m_cursorPos]->getType();
					m_player.rumbleStop();
				}
			}
#pragma endregion

			break;

#pragma endregion

#pragma region Care Package input handling
		case Gamemode::CarePackage:
#pragma region Keyboard Input
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
#pragma endregion

#pragma region Controller input
			//D-Pad pressed
			if (Event::JoystickMoved == event.type) {
				if (event.joystickMove.axis == sf::Joystick::PovY) // D-pad up/down
				{
					if (event.joystickMove.position == -100) // Up
						m_cursorPos--;
					else if (event.joystickMove.position == 100) // Down
						m_cursorPos++;
				}

			if (m_cursorPos > (static_cast<int>(m_weaponButtons.size()) - 1)) {
				m_cursorPos = 0;
				m_cursorSprite.setPosition(m_weaponButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_weaponButtons[m_cursorPos]->getType();
			}
			if (m_cursorPos < 0) {
				m_cursorPos = static_cast<int>(m_weaponButtons.size()) - 1;
				m_cursorSprite.setPosition(m_weaponButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_weaponButtons[m_cursorPos]->getType();
			}

			m_cursorSprite.setPosition(m_weaponButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_weaponButtons[m_cursorPos]->getType();
			}

			//Button pressed
			if (Event::JoystickButtonPressed == event.type) {
				if (event.joystickButton.button == 0) { //0=A 7=Start
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
				}
			}
#pragma endregion

			break;

#pragma endregion

#pragma region Gameplay input handling
		case Gamemode::Gameplay:
#pragma region Keyboard Input
			switch (event.key.code)
			{
			case sf::Keyboard::Escape:
				m_currentGamemode = Gamemode::Pause;
				pauseBgSprite.setPosition(m_playerCamera.getCenter());
				m_pauseButtons[0]->setPosition(Vector2f(m_playerCamera.getCenter().x - 250, m_playerCamera.getCenter().y));
				m_pauseButtons[1]->setPosition(Vector2f(m_playerCamera.getCenter().x + 250, m_playerCamera.getCenter().y));
				m_cursorPos = 0;
				m_cursorSprite.setPosition(m_pauseButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_pauseButtons[m_cursorPos]->getType();
				break;

			default:
				break;
			}
#pragma endregion

#pragma region Controller Input
			if (event.joystickButton.button == 7) {
				m_currentGamemode = Gamemode::Pause;
				pauseBgSprite.setPosition(m_playerCamera.getCenter());
				m_pauseButtons[0]->setPosition(Vector2f(m_playerCamera.getCenter().x - 250, m_playerCamera.getCenter().y));
				m_pauseButtons[1]->setPosition(Vector2f(m_playerCamera.getCenter().x + 250, m_playerCamera.getCenter().y));
				m_cursorPos = 0;
				m_cursorSprite.setPosition(m_pauseButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_pauseButtons[m_cursorPos]->getType();
				m_player.rumbleStop();
			}
#pragma endregion

			break;
#pragma endregion

		}
	}
}

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

			if (enemy->getDeathFrameCounter() > 10)
			{
				dropLoot(enemy);
				enemy->initialisePosition(m_player.getPosition());
			}
		}

		for (auto obstacle : m_obstacles)
		{
			obstacle->update(dt);
		}

		for (auto orb : m_xpOrbs)
		{
			orb->update(dt, m_player);
		}

		for (auto pickup : m_pickups)
		{
			pickup->update(dt);
		}

		if (!m_bossSpawned && m_bossTimer.getElapsedTime().asSeconds() > 120.0f)
		{
			m_enemies.push_back(new Enemy(m_holder["starterAtlas"], m_player.getPosition(), EnemyType::Boss));
			m_bossSpawned = true;
		}

		//std::cout << m_enemies.size() << std::endl;

		levelUpSpawner();

		checkCollisions();

		//Rumble timer logic
		if (orbRumbleTimer.getElapsedTime().asSeconds() > 0.1f && oIsRumbling) {
			oIsRumbling = false;
		}
		if (pickupRumbleTimer.getElapsedTime().asSeconds() > 0.01f && pIsRumbling) {
			pIsRumbling = false;
		}	
		if (enemyHitRumbleTimer.getElapsedTime().asSeconds() > 0.05f && eIsRumbling) {
			eIsRumbling = false;
		}
		if (!oIsRumbling && !pIsRumbling && !eIsRumbling && !m_player.getRumbleState()) {
			m_player.rumbleStop();
		}
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

void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
	
#pragma region GAMEPLAY
	if (m_currentGamemode == Gamemode::Gameplay || m_currentGamemode == Gamemode::Pause || m_currentGamemode == Gamemode::Upgrade || m_currentGamemode == Gamemode::CarePackage)
	{
		m_window.draw(bgSprite);
#pragma region in game
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

		for (auto obstacle : m_obstacles)
		{
			obstacle->renderTop(m_window);
		}

		m_player.renderHUD(m_window);

#pragma endregion

#pragma region pause menu
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
#pragma endregion

#pragma region Level up and acquisition menu
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
#pragma endregion

	}
#pragma endregion

#pragma region MENUS
	if (m_currentGamemode == Gamemode::Menu)
	{
		m_window.draw(menuBgSprite);
		for (auto buttons : m_menuButtons) {
			buttons->render(m_window);
		}
		m_window.draw(m_cursorSprite);
	}
#pragma endregion

	m_window.display();
}

#pragma region COLLISION HANDLER
void Game::checkCollisions()
{
#pragma region Enemies
	for (auto enemy : m_enemies)
	{
		if (enemy->getState() != CharacterState::DeadState)
		{
#pragma region Dash -> Enemy
			//Dash to Enemy
			if (CollisionDetection::playerDashEnemyCollision(m_player, enemy))
			{
				switch (enemy->getType())
				{
				case EnemyType::Small:
					enemy->setState(CharacterState::DeadState);
					break;
				case EnemyType::Big:
					enemy->setState(CharacterState::DeadState);
					break;
				case EnemyType::Boss:
					enemy->decreaseHealth(5.0f);

					if (enemy->getHealth() < 0)
					{
						enemy->setState(CharacterState::DeadState);
					}
					break;
				default:
					break;
				}
			}
#pragma endregion

#pragma region Slash -> Enemy
			//Slash to Enemy
			if (CollisionDetection::playerSlashEnemyCollision(m_player, enemy))
			{
				switch (enemy->getType())
				{
				case EnemyType::Small:
					enemy->setState(CharacterState::DeadState);
					break;
				case EnemyType::Big:
					enemy->setState(CharacterState::DeadState);
					break;
				case EnemyType::Boss:
					enemy->decreaseHealth(10.0f);

					if (enemy->getHealth() < 0)
					{
						enemy->setState(CharacterState::DeadState);
					}
					break;
				default:
					break;
				}
			}
#pragma endregion

#pragma region Bullet -> Enemy
			//Bullet to Enemy
			for (auto weapon : m_player.getWeapon())
			{
				if (weapon->getType() == WeaponType::Pistol || weapon->getType() == WeaponType::AssaultRifle) // If bullet is meant to delete when collided with enemy
				{
					for (auto it = weapon->getBullets().begin(); it != weapon->getBullets().end();) // this line is the issue. it needs to be permanent. it gets destroyed so no work
					{
						if (CollisionDetection::bulletEnemyCollision((*it), enemy))
						{
							switch (enemy->getType())
							{
							case EnemyType::Small:
								enemy->playHitSound();
								enemy->setState(CharacterState::DeadState);
								break;
							case EnemyType::Big:
								enemy->playHitSound();
								enemy->setState(CharacterState::DeadState);
								break;
							case EnemyType::Boss:
								enemy->playHitSound();
								enemy->decreaseHealth((*it)->getDamage());

								enemy->setColour(sf::Color::Red);

								if (enemy->getHealth() < 0.0f)
								{
									enemy->setState(CharacterState::DeadState);
								}
								break;
							default:
								break;
							}

							m_player.weakRumbleStart();
							enemyHitRumbleTimer.restart();
							eIsRumbling = true;

							delete* it; // Delete the bullet object
							it = weapon->getBullets().erase(it); // Remove the bullet pointer from the vector
						}
						else
						{
							++it;
						}
					}
				}
				else
				{
					for (auto bullet : weapon->getBullets())
					{
						if (CollisionDetection::bulletEnemyCollision(bullet, enemy))
						{
							switch (enemy->getType())
							{
							case EnemyType::Small:
								enemy->playHitSound();
								enemy->setState(CharacterState::DeadState);
								break;
							case EnemyType::Big:
								enemy->playHitSound();
								enemy->setState(CharacterState::DeadState);
								break;
							case EnemyType::Boss:
								enemy->playHitSound();
								enemy->decreaseHealth(bullet->getDamage() / 100.0f);

								if (enemy->getHealth() < 0)
								{
									enemy->setState(CharacterState::DeadState);
								}
							default:
								break;
							}
						}
					}
				}
			}

#pragma endregion

#pragma region Player -> Enemy
			//Player to Enemy
			if (CollisionDetection::playerEnemyCollision(m_player, enemy))
			{
				switch (enemy->getType())
				{
				case EnemyType::Small:
					m_player.decreaseHealth(1.0f);
					break;
				case EnemyType::Big:
					m_player.decreaseHealth(2.0f);
					break;
				case EnemyType::Boss:
					m_player.decreaseHealth(5.0f);
					break;
				default:
					break;
				}

			}
#pragma endregion
		}
	}

#pragma endregion

#pragma region Orbs
	for (auto it = m_xpOrbs.begin(); it != m_xpOrbs.end();)
	{
		if (CollisionDetection::playerOrbCollision(m_player, *it))
		{
			m_player.rumbleStart();
			orbRumbleTimer.restart();
			oIsRumbling = true; //Orb has rumbled = ohr

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

#pragma endregion

#pragma region Pickups
	for (auto it = m_pickups.begin(); it != m_pickups.end();)
	{
		if (CollisionDetection::playerPickupCollision(m_player, *it))
		{
			m_player.increaseHealth();

			m_player.rumbleStart();
			pickupRumbleTimer.restart();
			pIsRumbling = true; // pickup is rumbling

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

#pragma region Button Functions
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

#pragma endregion

void Game::levelUpSpawner()
{
	if (m_player.getLevel() > m_currentLevel)
	{
		for (int i = 0; i < 10; i++)
		{
			m_enemies.push_back(new Enemy(m_holder["starterAtlas"], m_player.getPosition(), EnemyType::Small));
		}
		for (int i = 0; i < 3; i++)
		{
			m_enemies.push_back(new Enemy(m_holder["starterAtlas"], m_player.getPosition(), EnemyType::Big));
		}
		
		m_currentLevel++;
		m_currentGamemode = Gamemode::Upgrade;
		levelUpBGSprite.setPosition(m_playerCamera.getCenter());

		createRandomUpgrades();
		m_cursorPos = 0;
		m_cursorSprite.setPosition(m_upgradeButtons[m_cursorPos]->getPositon());
		m_cursorButtonType = m_upgradeButtons[m_cursorPos]->getType();
	}
}

void Game::dropLoot(Enemy* t_enemy)
{
	if (rand() % 4 != 0)	// 75% chance enemy spawns an XP Orb on death
	{
		m_xpOrbs.push_back(new XPOrb(m_holder["starterAtlas"], t_enemy->getPosition()));
	}

	if (rand() % 10 == 0)	// 10% chance enemy spawns Health Pickup on death
	{
		m_pickups.push_back(new Pickup(m_holder["starterAtlas"], t_enemy->getPosition(), PickupType::Health));
	}
}