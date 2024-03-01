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
	hasGameOverTimerStarted = false;
	firstStart = true;
	inMenu = false;

#pragma region THOR
	//THOR
	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));
	m_holder.acquire("mapSprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/Map.png"));
	m_holder.acquire("mainMenuBG", thor::Resources::fromFile<sf::Texture>("resources/sprites/menu.png"));
	m_holder.acquire("UIAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/UI_Atlas.png"));
	m_holder.acquire("obstacleAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/ObstacleAtlas.png"));
	m_holder.acquire("tutorialMenu", thor::Resources::fromFile<sf::Texture>("resources/sprites/InformationScreen.png"));
#pragma endregion

#pragma region SOUND
	//SOUND
	if (!m_pickupSoundBuffer.loadFromFile("resources/sounds/orb_pickup.wav"))
	{
		std::cout << "error loading orb pick up sound";
	}
	m_pickupSound.setBuffer(m_pickupSoundBuffer);
	m_pickupSound.setVolume(15.0f);

	if (!m_menuMusicBuffer.loadFromFile("resources/sounds/menu_music.wav"))
	{
		std::cout << "error loading menu music";
	}
	m_menuMusic.setBuffer(m_menuMusicBuffer);
	m_menuMusic.setLoop(true);
	m_menuMusic.setVolume(1.0f);

	if (!m_gameplayMusicBuffer.loadFromFile("resources/sounds/gameplay_music.wav"))
	{
		std::cout << "error loading gameplay music";
	}
	m_gameplayMusic.setBuffer(m_gameplayMusicBuffer);
	m_gameplayMusic.setLoop(true);
	m_gameplayMusic.setVolume(2.0f);

	m_menuMusic.play();
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

#pragma region OTHERS (Font, Rumble, Background, score)
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

	score = 0;
	smallEK = 0;
	normalEK = 0;
	bigEK = 0;
	bossEK = 0;
	updateCount = 0;
	timeSurvived = -2;
#pragma endregion

#pragma region MENU INITIALISERS
#pragma region main menu
	//MAIN MENU INITIALISER
	Texture& mainMenuBgTexture = m_holder["mainMenuBG"];
	Texture& UITexture = m_holder["UIAtlas"];
	menuBgSprite.setTexture(mainMenuBgTexture);
	menuBgSprite.setTextureRect(IntRect{ 0,0,1600,900 });
	menuBgSprite.setOrigin(1600.0f / 2.0f, 900.0f / 2.0f);
	menuBgSprite.setPosition(m_menuCamera.getCenter());

	m_menuButtons.push_back(new Button(ButtonType::Play, UITexture, m_arialFont, Vector2f(550, 200), Vector2f(1.0f, 1.0f)));
	m_menuButtons.push_back(new Button(ButtonType::Resume, UITexture, m_arialFont, Vector2f(550, 320), Vector2f(1.0f, 1.0f)));
	m_menuButtons.push_back(new Button(ButtonType::Tutorial, UITexture, m_arialFont, Vector2f(550, 440), Vector2f(1.0f, 1.0f)));
	m_menuButtons.push_back(new Button(ButtonType::Credits, UITexture, m_arialFont, Vector2f(550, 560), Vector2f(1.0f, 1.0f)));
	m_menuButtons.push_back(new Button(ButtonType::Exit, UITexture, m_arialFont, Vector2f(550, 680), Vector2f(1.0f, 1.0f)));
#pragma endregion

#pragma region pause menu
	//PAUSE MENU INITIALISER 
	pauseBgSprite.setTexture(UITexture);
	pauseBgSprite.setTextureRect(IntRect{ 0, 1965, 600, 275 });
	pauseBgSprite.setOrigin(300, 275.0F / 2.0F);
	pauseBgSprite.setScale(2.5f, 2.5f);
	pauseBgSprite.setPosition(m_playerCamera.getCenter());

	m_pauseButtons.push_back(new Button(ButtonType::Resume, UITexture, m_arialFont, Vector2f(m_playerCamera.getCenter().x - 250, m_playerCamera.getCenter().y), Vector2f(1.0f, 1.0f)));
	m_pauseButtons.push_back(new Button(ButtonType::ToMenu, UITexture, m_arialFont, Vector2f(m_playerCamera.getCenter().x + 250, m_playerCamera.getCenter().y), Vector2f(1.0f, 1.0f)));

#pragma endregion

#pragma region upgrade menu
	//UPGRADE MENU INITIALISER
	levelUpBGSprite.setTexture(UITexture);
	levelUpBGSprite.setTextureRect(IntRect{ 0, 300, 800, 600 });
	levelUpBGSprite.setOrigin(400.0f, 300.0f);
	levelUpBGSprite.setScale(1.3f, 1.3f);
	levelUpBGSprite.setPosition(m_playerCamera.getCenter());
	//no need to setup buttons in constructor since they are randomised everytime

	dashInfoImgBGSprite.setTexture(UITexture);
	dashInfoImgBGSprite.setTextureRect(IntRect({ 1000, 0, 250, 250 }));
	dashInfoImgBGSprite.setOrigin(125.0f, 125.0f);
	dashInfoImgBGSprite.setPosition(m_playerCamera.getCenter());

	dashInfoTxtBGSprite.setTexture(UITexture);
	dashInfoTxtBGSprite.setTextureRect(IntRect({ 1250, 0, 250, 250 }));
	dashInfoTxtBGSprite.setOrigin(125.0f, 125.0f);
	dashInfoTxtBGSprite.setPosition(m_playerCamera.getCenter());

	dashInfoImgSprite.setTexture(m_holder["starterAtlas"]);
	dashInfoImgSprite.setTextureRect(IntRect({ 0, 1778, 200, 160 }));
	dashInfoImgSprite.setOrigin(100.0f, 80.0f);

	dashInfoTxt.setFont(m_arialFont);
	dashInfoTxt.setStyle(sf::Text::Bold);
	dashInfoTxt.setCharacterSize(40U);
	dashInfoTxt.setOutlineColor(sf::Color::Black);
	dashInfoTxt.setOutlineThickness(3U);
	dashInfoTxt.setFillColor(sf::Color::White);

	dashDescriptionText.setFont(m_arialFont);
	dashDescriptionText.setStyle(sf::Text::Bold);
	dashDescriptionText.setCharacterSize(25U);
	dashDescriptionText.setOutlineColor(sf::Color::Black);
	dashDescriptionText.setOutlineThickness(2.0f);
	dashDescriptionText.setFillColor(sf::Color::White);
#pragma endregion

#pragma region game over menu
	////GAME OVER INITIALISER
	gameOverBGSprite.setTexture(UITexture);
	gameOverBGSprite.setTextureRect(IntRect{ 0, 901, 800, 600 });
	gameOverBGSprite.setOrigin(400.0f, 300.0f);
	gameOverBGSprite.setScale(1.5, 1.5f);
	gameOverBGSprite.setPosition(m_playerCamera.getCenter());

	m_gameoverButtons.push_back(new Button(ButtonType::Play, UITexture, m_arialFont, Vector2f(m_playerCamera.getCenter().x - 275, m_playerCamera.getCenter().y + 365), Vector2f(0.64f, 0.7f)));
	m_gameoverButtons.push_back(new Button(ButtonType::ToMenu, UITexture, m_arialFont, Vector2f(m_playerCamera.getCenter().x + 275, m_playerCamera.getCenter().y + 365), Vector2f(0.64f, 0.7f)));

	m_scoreSprite.setTexture(UITexture);
	m_scoreSprite.setTextureRect(IntRect{ 0, 1503, 550, 150 });
	m_scoreSprite.setOrigin(550.0f / 2.0f, 150.0f / 2);
	m_scoreSprite.setScale(1.0f, 1.0f);
	m_scoreSprite.setPosition(m_playerCamera.getCenter().x, m_playerCamera.getCenter().y + 250); 

	m_scoreVarBGSprite.setTexture(UITexture);
	m_scoreVarBGSprite.setTextureRect(IntRect{ 1250, 0, 250, 250 });
	m_scoreVarBGSprite.setOrigin(125, 125);
	m_scoreVarBGSprite.setScale(2.0f, 1.5f);
	m_scoreVarBGSprite.setPosition(m_playerCamera.getCenter().x, m_playerCamera.getCenter().y + 50);

#pragma region Text
	m_statsText.setFont(m_arialFont);
	m_statsText.setStyle(sf::Text::Bold);
	m_statsText.setStyle(sf::Text::Underlined);
	m_statsText.setCharacterSize(40U);
	m_statsText.setOutlineColor(sf::Color::Black);
	m_statsText.setOutlineThickness(3U);
	m_statsText.setFillColor(sf::Color::Red);
	m_statsText.setOutlineThickness(2.0f);
	m_statsText.setString("STATS");
	m_statsText.setOrigin(m_statsText.getGlobalBounds().width / 2.0f, m_statsText.getGlobalBounds().height / 2.0f);
	m_statsText.setPosition(m_scoreVarBGSprite.getPosition().x, m_scoreVarBGSprite.getPosition().y - 105);

	m_smallEKText.setFont(m_arialFont);
	m_smallEKText.setStyle(sf::Text::Bold);
	m_smallEKText.setCharacterSize(25U);
	m_smallEKText.setOutlineColor(sf::Color::Black);
	m_smallEKText.setFillColor(sf::Color::White);
	m_smallEKText.setOutlineThickness(2.0f);
	m_smallEKText.setString("Small enemies killed: " + to_string(smallEK));
	m_smallEKText.setOrigin(0.0f, m_smallEKText.getGlobalBounds().height / 2.0f);
	m_smallEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 70);

	m_normalEKText.setFont(m_arialFont);
	m_normalEKText.setStyle(sf::Text::Bold);
	m_normalEKText.setCharacterSize(25U);
	m_normalEKText.setOutlineColor(sf::Color::Black);
	m_normalEKText.setFillColor(sf::Color::White);
	m_normalEKText.setOutlineThickness(2.0f);
	m_normalEKText.setString("Normal enemies killed: " + to_string(normalEK));
	m_normalEKText.setOrigin(0.0f, m_normalEKText.getGlobalBounds().height / 2.0f);
	m_normalEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 100);

	m_bigEKText.setFont(m_arialFont);
	m_bigEKText.setStyle(sf::Text::Bold);
	m_bigEKText.setCharacterSize(25U);
	m_bigEKText.setOutlineColor(sf::Color::Black);
	m_bigEKText.setFillColor(sf::Color::White);
	m_bigEKText.setOutlineThickness(2.0f);
	m_bigEKText.setString("Big enemies killed: " + to_string(bigEK));
	m_bigEKText.setOrigin(0.0f, m_bigEKText.getGlobalBounds().height / 2.0f);
	m_bigEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 130);

	m_bossEKText.setFont(m_arialFont);
	m_bossEKText.setStyle(sf::Text::Bold);
	m_bossEKText.setCharacterSize(25U);
	m_bossEKText.setOutlineColor(sf::Color::Black);
	m_bossEKText.setFillColor(sf::Color::White);
	m_bossEKText.setOutlineThickness(2.0f);
	m_bossEKText.setString("Boss enemies killed: " + to_string(bossEK));
	m_bossEKText.setOrigin(0.0f, m_bigEKText.getGlobalBounds().height / 2.0f);
	m_bossEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 160);

	m_totalEKText.setFont(m_arialFont);
	m_totalEKText.setStyle(sf::Text::Bold);
	m_totalEKText.setCharacterSize(25U);
	m_totalEKText.setOutlineColor(sf::Color::Black);
	m_totalEKText.setFillColor(sf::Color::White);
	m_totalEKText.setOutlineThickness(2.0f);
	m_totalEKText.setString("TOTAL ENEMIES KILLED: " + to_string(totalEK));
	m_totalEKText.setOrigin(0.0f, m_totalEKText.getGlobalBounds().height / 2.0f);
	m_totalEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 190);

	m_playerLevelText.setFont(m_arialFont);
	m_playerLevelText.setStyle(sf::Text::Bold);
	m_playerLevelText.setCharacterSize(25U);
	m_playerLevelText.setOutlineColor(sf::Color::Black);
	m_playerLevelText.setFillColor(sf::Color::White);
	m_playerLevelText.setOutlineThickness(2.0f);
	m_playerLevelText.setString("Player Level: " + to_string(m_currentLevel));
	m_playerLevelText.setOrigin(0.0f, m_playerLevelText.getGlobalBounds().height / 2.0f);
	m_playerLevelText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 220);

	m_timeSurvivedText.setFont(m_arialFont);
	m_timeSurvivedText.setStyle(sf::Text::Bold);
	m_timeSurvivedText.setCharacterSize(25U);
	m_timeSurvivedText.setOutlineColor(sf::Color::Black);
	m_timeSurvivedText.setFillColor(sf::Color::White);
	m_timeSurvivedText.setOutlineThickness(2.0f);
	m_timeSurvivedText.setString("Time Survived: " + to_string(timeSurvived) + " seconds");
	m_timeSurvivedText.setOrigin(0.0f, m_timeSurvivedText.getGlobalBounds().height / 2.0f);
	m_timeSurvivedText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 250);

	m_scoreText.setFont(m_arialFont);
	m_scoreText.setStyle(sf::Text::Bold);
	m_scoreText.setCharacterSize(30U);
	m_scoreText.setOutlineColor(sf::Color::Black);
	m_scoreText.setFillColor(sf::Color::White);
	m_scoreText.setOutlineThickness(2.0f);
	m_scoreText.setString(to_string(score));
	m_scoreText.setOrigin(m_scoreText.getGlobalBounds().width / 2.0f, m_scoreText.getGlobalBounds().height / 2.0f);
	m_scoreText.setPosition(m_scoreSprite.getPosition().x + 150, m_scoreSprite.getPosition().y);

#pragma endregion

#pragma endregion

#pragma region cursor
	//CURSOR INITIALISER
	m_cursorPos = 0;
	m_cursorSprite.setTexture(UITexture);
	m_cursorSprite.setTextureRect(IntRect{ 0, 1664, 550, 150 });
	m_cursorSprite.setOrigin(275.0f, 75.0f);
	m_cursorSprite.setScale(0.9f, 1.0f);
	m_cursorSprite.setPosition(m_menuButtons[m_cursorPos]->getPositon());
	m_cursorButtonType = m_menuButtons[m_cursorPos]->getType();

#pragma endregion

#pragma endregion

}

void Game::startGame()
{
	m_currentGamemode = Gamemode::Gameplay;
	m_bossTimer.restart();
	m_bossSpawned = false;

	m_player.initialise();
	m_currentLevel = 1;
	hasGameOverTimerStarted = false;

	m_gameplayMusic.play();
	m_menuMusic.stop();

	score = 0;
	smallEK = 0;
	normalEK = 0;
	bigEK = 0;
	bossEK = 0;
	updateCount = 0;
	timeSurvived = -2;


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
					m_menuMusic.stop();
					m_gameplayMusic.play();

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
				if (!inMenu) {
					if (event.joystickButton.button == 0) { //0=A 7=Start
						switch (m_cursorButtonType)
						{
						case ButtonType::Play:
							m_currentGamemode = Gamemode::Gameplay;
							firstStart = false;
							m_player.rumbleStop();
							startGame();
							break;
						case ButtonType::Resume:
							if (!firstStart) {
								m_currentGamemode = Gamemode::Gameplay;
								m_menuMusic.stop();
								m_gameplayMusic.play();
							}
							break;
						case ButtonType::Tutorial:
							inMenu = true;
							menuBgSprite.setTexture(m_holder["tutorialMenu"]);
							break;
							//CASE FOR TUTORIAL AND CASE FOR CREDITS
						case ButtonType::Exit:
							m_player.rumbleStop();
							m_window.close();
							break;
						}
					}
				}	
				else {
					if (event.joystickButton.button == 1) {
						inMenu = false;
						menuBgSprite.setTexture(m_holder["mainMenuBG"]);
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
					m_menuMusic.play();
					m_gameplayMusic.stop();

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
						m_menuMusic.play();
						m_gameplayMusic.stop();

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

#pragma region Game over input handling
		case Gamemode::GameOver:
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

				if (m_cursorPos > (static_cast<int>(m_gameoverButtons.size()) - 1)) {
					m_cursorPos = 0;
					m_cursorSprite.setPosition(m_gameoverButtons[m_cursorPos]->getPositon());
					m_cursorButtonType = m_gameoverButtons[m_cursorPos]->getType();
				}
				if (m_cursorPos < 0) {
					m_cursorPos = static_cast<int>(m_gameoverButtons.size()) - 1;
					m_cursorSprite.setPosition(m_gameoverButtons[m_cursorPos]->getPositon());
					m_cursorButtonType = m_gameoverButtons[m_cursorPos]->getType();
				}

				m_cursorSprite.setPosition(m_gameoverButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_gameoverButtons[m_cursorPos]->getType();
			}

			//Button pressed
			if (Event::JoystickButtonPressed == event.type) {
				if (event.joystickButton.button == 0) { //0=A 7=Start
					switch (m_cursorButtonType)
					{
					case ButtonType::Play:
						m_currentGamemode = Gamemode::Gameplay;
						startGame();
						break;
					case ButtonType::ToMenu:
						m_currentGamemode = Gamemode::Menu;

						m_menuMusic.play();
						m_gameplayMusic.stop();

						m_cursorPos = 0;
						m_cursorSprite.setPosition(m_menuButtons[m_cursorPos]->getPositon());
						m_cursorButtonType = m_menuButtons[m_cursorPos]->getType();
						m_player.rumbleStop();
						break;
					}
				}
				break;
			}

#pragma endregion
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
		if (!m_player.getAliveState() && !hasGameOverTimerStarted){
			m_gameOverTimer.restart();
			hasGameOverTimerStarted = true;
		}
		if (m_gameOverTimer.getElapsedTime().asSeconds() > 1.5f && !m_player.getAliveState()) {
			isGameOver = false;
			//switch to game over game mode here
			m_currentGamemode = Gamemode::GameOver;
			m_cursorSprite.setPosition(m_gameoverButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_gameoverButtons[m_cursorPos]->getType();
			score = (((1 * smallEK) + (1.5 * normalEK) + (5 * bigEK) + (200 * bossEK)) / 100) * timeSurvived;

#pragma region Menu UI setup
#pragma region Sprites
			m_scoreSprite.setPosition(m_playerCamera.getCenter().x, m_playerCamera.getCenter().y + 250);
			m_scoreVarBGSprite.setPosition(m_playerCamera.getCenter().x, m_playerCamera.getCenter().y + 50);
			gameOverBGSprite.setPosition(m_playerCamera.getCenter());
			m_gameoverButtons[0]->setPosition(Vector2f(m_playerCamera.getCenter().x - 275, m_playerCamera.getCenter().y + 365));
			m_gameoverButtons[1]->setPosition(Vector2f(m_playerCamera.getCenter().x + 275, m_playerCamera.getCenter().y + 365));
#pragma endregion

#pragma region Text
			m_statsText.setPosition(m_scoreVarBGSprite.getPosition().x, m_scoreVarBGSprite.getPosition().y - 105);

			m_smallEKText.setString("Small enemies killed: " + to_string(smallEK));
			m_smallEKText.setOrigin(0.0f, m_smallEKText.getGlobalBounds().height / 2.0f);
			m_smallEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 70);

			m_normalEKText.setString("Normal enemies killed: " + to_string(normalEK));
			m_normalEKText.setOrigin(0.0f, m_normalEKText.getGlobalBounds().height / 2.0f);
			m_normalEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 100);

			m_bigEKText.setString("Big enemies killed: " + to_string(bigEK));
			m_bigEKText.setOrigin(0.0f, m_bigEKText.getGlobalBounds().height / 2.0f);
			m_bigEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 130);

			m_bossEKText.setString("Boss enemies killed: " + to_string(bossEK));
			m_bossEKText.setOrigin(0.0f, m_bigEKText.getGlobalBounds().height / 2.0f);
			m_bossEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 160);

			m_totalEKText.setString("TOTAL ENEMIES KILLED: " + to_string(totalEK));
			m_totalEKText.setOrigin(0.0f, m_totalEKText.getGlobalBounds().height / 2.0f);
			m_totalEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 190);

			m_playerLevelText.setString("Player Level: " + to_string(m_currentLevel));
			m_playerLevelText.setOrigin(0.0f, m_playerLevelText.getGlobalBounds().height / 2.0f);
			m_playerLevelText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 220);

			m_timeSurvivedText.setString("Time Survived: " + to_string(timeSurvived) + " seconds");
			m_timeSurvivedText.setOrigin(0.0f, m_timeSurvivedText.getGlobalBounds().height / 2.0f);
			m_timeSurvivedText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 250);

			m_scoreText.setString(to_string(score));
			m_scoreText.setOrigin(m_scoreText.getGlobalBounds().width / 2.0f, m_scoreText.getGlobalBounds().height / 2.0f);
			m_scoreText.setPosition(m_scoreSprite.getPosition().x + 150, m_scoreSprite.getPosition().y);

#pragma endregion

#pragma endregion


		}
		

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

		//time in game
		if (updateCount > 60) {
			timeSurvived++;
			updateCount = 0;
		}
		updateCount++;
	}

#pragma endregion

	if (m_currentGamemode == Gamemode::Upgrade) //setup upgrade screen here
	{
		//m_player.levelUp(m_currentGamemode);
	}
}

void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
	
#pragma region GAMEPLAY
	if (m_currentGamemode == Gamemode::Gameplay || m_currentGamemode == Gamemode::Pause ||
		m_currentGamemode == Gamemode::Upgrade || m_currentGamemode == Gamemode::CarePackage ||
		m_currentGamemode == Gamemode::GameOver)
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

			m_window.draw(dashInfoImgBGSprite);
			m_window.draw(dashInfoTxtBGSprite);
			m_window.draw(dashInfoImgSprite);
			m_window.draw(dashInfoTxt);
			m_window.draw(dashDescriptionText);

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

#pragma region Game over menu
		if (m_currentGamemode == Gamemode::GameOver)
		{
			m_window.draw(gameOverBGSprite);
			for (auto buttons : m_gameoverButtons)
			{
				buttons->render(m_window);
			}

			m_window.draw(m_scoreVarBGSprite);
			m_window.draw(m_scoreSprite);

			m_window.draw(m_statsText);
			m_window.draw(m_smallEKText);
			m_window.draw(m_normalEKText);
			m_window.draw(m_bigEKText);
			m_window.draw(m_bossEKText);
			m_window.draw(m_totalEKText);
			m_window.draw(m_playerLevelText);
			m_window.draw(m_timeSurvivedText);
			m_window.draw(m_scoreText);
			//RectangleShape rect;
			//rect.setSize(m_smallEKText.getGlobalBounds().getSize());
			//rect.setOutlineColor(Color::Red);
			//rect.setOutlineThickness(2.0f);
			//rect.setOrigin(m_smallEKText.getOrigin());
			//rect.setPosition(m_smallEKText.getPosition());
			//m_window.draw(rect);
			m_window.draw(m_cursorSprite);
		}

#pragma endregion

	}
#pragma endregion

#pragma region MENUS
	if (m_currentGamemode == Gamemode::Menu)
	{
		m_window.draw(menuBgSprite);
		if (!inMenu) {
			for (auto buttons : m_menuButtons) {
				buttons->render(m_window);
			}
			m_window.draw(m_cursorSprite);
		}

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
			for (auto obstacle : m_obstacles)
			{
				if (CollisionDetection::enemyObstacleCollision(enemy, obstacle))
				{
					enemy->inverseMovement();
				}
			}
			
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
						switch (enemy->getType()) {
						case EnemyType::Small:
							smallEK++;
							break;
						case EnemyType::Big:
							bigEK++;
							break;
						case EnemyType::Boss:
							bossEK++;
							break;
						}
						enemy->setState(CharacterState::DeadState);
						totalEK++;
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
						switch (enemy->getType()) {
						case EnemyType::Small:
							smallEK++;
							break;
						case EnemyType::Big:
							bigEK++;
							break;
						case EnemyType::Boss:
							bossEK++;
						enemy->setState(CharacterState::DeadState);
							break;
						}
						totalEK++;
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
								switch (enemy->getType()) {
								case EnemyType::Small:
									smallEK++;
									break;
								case EnemyType::Big:
									bigEK++;
									break;
								case EnemyType::Boss:
									bossEK++;
									break;
								}
								totalEK++;
								break;
							case EnemyType::Big:
								enemy->playHitSound();
								enemy->setState(CharacterState::DeadState);
								switch (enemy->getType()) {
								case EnemyType::Small:
									smallEK++;
									break;
								case EnemyType::Big:
									bigEK++;
									break;
								case EnemyType::Boss:
									bossEK++;
									break;
								}
								totalEK++;
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

	m_upgradeButtons.push_back(new Button(randomUpgradeButton1, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y - 180), Vector2f(0.75f, 0.75f)));
	m_upgradeButtons.push_back(new Button(randomUpgradeButton2, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y), Vector2f(0.75f, 0.75f)));
	m_upgradeButtons.push_back(new Button(randomUpgradeButton3, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y + 180), Vector2f(0.75f, 0.75f)));

	setDashInfo();
}

void Game::setDashInfo()
{
	String description;
	switch (m_player.getDashLevel())
	{
	case 1:
		description = "dash unlocked";
		break;
	case 2:
		description = "cooldown reduced";
		break;
	case 3:
		description = "2nd dash charge";
		break;
	case 4:
		description = "slash after dash";
		break;
	case 5:
		description = "distance increased\n3rd dash charge";
		break;
	case 6:
		description = "cooldown reduced";
		break;
	case 7:
		description = "4th dash charge";
		break;
	default:
		description = "FULLY UPGRADED";
		break;
	}
	dashInfoTxt.setString("DASH LEVEL " + to_string(m_player.getDashLevel()));//switch case in function to determine modifier text
	dashInfoTxt.setOrigin(dashInfoTxt.getGlobalBounds().width / 2.0f, dashInfoTxt.getGlobalBounds().height / 2.0f);
	dashDescriptionText.setString(description);
	dashDescriptionText.setOrigin(dashDescriptionText.getGlobalBounds().width / 2.0f, dashDescriptionText.getGlobalBounds().height / 2.0f);
	dashDescriptionText.setPosition(dashInfoTxt.getPosition().x, dashInfoTxt.getPosition().y + 60);
}

void Game::createRandomWeapons()
{
	m_weaponButtons.clear();

	ButtonType randomUpgradeButton1 = static_cast<ButtonType>((rand() % 2) + static_cast<int>(ButtonType::GetPistol));
	ButtonType randomUpgradeButton2 = static_cast<ButtonType>((rand() % 2) + static_cast<int>(ButtonType::GetPistol));
	while (randomUpgradeButton1 == randomUpgradeButton2) { randomUpgradeButton2 = static_cast<ButtonType>((rand() % 2) + static_cast<int>(ButtonType::GetPistol)); }

	m_weaponButtons.push_back(new Button(randomUpgradeButton1, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y - 150), Vector2f(0.75f, 0.75f)));
	m_weaponButtons.push_back(new Button(randomUpgradeButton2, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y + 150), Vector2f(0.75f, 0.75f)));
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

		dashInfoImgBGSprite.setPosition(levelUpBGSprite.getPosition().x + 200, levelUpBGSprite.getPosition().y - 100);
		dashInfoTxtBGSprite.setPosition(dashInfoImgBGSprite.getPosition().x, (dashInfoImgBGSprite.getPosition().y + 250) + 10);
		dashInfoImgSprite.setPosition(dashInfoImgBGSprite.getPosition());
		dashInfoTxt.setPosition(dashInfoTxtBGSprite.getPosition().x, dashInfoTxtBGSprite.getPosition().y - 50);

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