#include "Game.h"
#include <iostream>

// Our target FPS
static double const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_width, ScreenSize::s_height, 32), "Codename: Reclaim", sf::Style::Default),
	m_playerCamera(sf::FloatRect(0, 0, m_window.getSize().x, m_window.getSize().y)),
	m_menuCamera(FloatRect(0, 0, m_window.getSize().x, m_window.getSize().y))
{
	sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
	m_window.create(desktopMode, "Codename: Reclaim", sf::Style::Fullscreen);

	m_renderTexture.create(ScreenSize::s_width, ScreenSize::s_height);
	m_outlineShader.loadFromFile("shader.vert", "shader.frag");
		
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
	m_bossSpawned = false;
	highScore = 0;
	isFirstPage = true;

#pragma region THOR
	//THOR
	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));
	m_holder.acquire("mapSprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/Map.png"));
	m_holder.acquire("mainMenuBG", thor::Resources::fromFile<sf::Texture>("resources/sprites/menu.png"));
	m_holder.acquire("UIAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/UI_Atlas.png"));
	m_holder.acquire("obstacleAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/ObstacleAtlas.png"));
	m_holder.acquire("tutorialMenu", thor::Resources::fromFile<sf::Texture>("resources/sprites/InformationScreen.png"));
	m_holder.acquire("creditsMenu", thor::Resources::fromFile<sf::Texture>("resources/sprites/credits.png"));
	m_holder.acquire("screenVignette", thor::Resources::fromFile<sf::Texture>("resources/sprites/bigblackbox.png"));
	m_holder.acquire("rankedScreen", thor::Resources::fromFile<sf::Texture>("resources/sprites/Rank.png"));
#pragma endregion

#pragma region SOUND
	//SOUND
	if (!m_orbPickupSoundBuffer.loadFromFile("resources/sounds/orb_pickup.wav"))
	{
		std::cout << "error loading orb pick up sound";
	}
	m_orbPickupSound.setBuffer(m_orbPickupSoundBuffer);
	m_orbPickupSound.setVolume(15.0f);

	if (!m_healthPickupSoundBuffer.loadFromFile("resources/sounds/health_pickup.wav"))
	{
		std::cout << "error loading health pick up sound";
	}
	m_healthPickupSound.setBuffer(m_healthPickupSoundBuffer);
	m_healthPickupSound.setVolume(3.0f);
	
	if (!m_levelupSoundBuffer.loadFromFile("resources/sounds/levelup.wav"))
	{
		std::cout << "error loading level up sound";
	}
	m_levelupSound.setBuffer(m_levelupSoundBuffer);
	m_levelupSound.setVolume(2.0f);

	if (!m_explosionSoundBuffer.loadFromFile("resources/sounds/rpg_explosion.wav"))
	{
		std::cout << "error loading explosion sound";
	}
	m_explosionSound.setBuffer(m_explosionSoundBuffer);
	m_explosionSound.setVolume(4.0f);

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

	if (!m_menuSelectBuffer.loadFromFile("resources/sounds/button_select.wav"))
	{
		std::cout << "error loading select sound";
	}
	m_menuSelectSound.setBuffer(m_menuSelectBuffer);
	m_menuSelectSound.setVolume(3.0f);

	if (!m_menuScrollBuffer.loadFromFile("resources/sounds/button_scroll.wav"))
	{
		std::cout << "error loading scroll sound";
	}
	m_menuScrollSound.setBuffer(m_menuScrollBuffer);
	m_menuScrollSound.setVolume(1.5f);

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
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock1, m_player.getPosition()));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock2, m_player.getPosition()));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock3, m_player.getPosition()));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Tree, m_player.getPosition()));
	}
#pragma endregion

#pragma region OTHERS (Font, Rumble, Background, score)
	//FONT
	if (!m_arialFont.loadFromFile("resources/BebasNeue.otf"))
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

	m_playerRank = 1;
	score = 0;
	smallEK = 0;
	bigEK = 0;
	bossEK = 0;
	updateCount = 0;
	timeSurvived = 0;
	timeSurvivedSeconds = 0;
	timeSurvivedMinutes = 0;
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

	m_vignetteSprite.setTexture(m_holder["screenVignette"]);
	m_vignetteSprite.setTextureRect(IntRect{ 0,0,1600,900 });
	m_vignetteSprite.setOrigin(1600.0f / 2.0f, 900.0f / 2.0f);
	m_vignetteSprite.setPosition(m_playerCamera.getCenter());
	m_hudSprite.setTexture(m_holder["screenVignette"]);
	m_hudSprite.setTextureRect(IntRect{ 0,0,1600,80 });
	m_hudSprite.setOrigin(1600.0f / 2.0f, 900.0f / 2.0f);
	m_hudSprite.setPosition(m_playerCamera.getCenter());

	m_menuButtons.push_back(new Button(ButtonType::Play, UITexture, m_arialFont, Vector2f(550, 200), Vector2f(1.0f, 1.0f)));
	m_menuButtons.push_back(new Button(ButtonType::Resume, UITexture, m_arialFont, Vector2f(550, 320), Vector2f(1.0f, 1.0f)));
	m_menuButtons.push_back(new Button(ButtonType::Tutorial, UITexture, m_arialFont, Vector2f(550, 440), Vector2f(1.0f, 1.0f)));
	m_menuButtons.push_back(new Button(ButtonType::Credits, UITexture, m_arialFont, Vector2f(550, 560), Vector2f(1.0f, 1.0f)));
	m_menuButtons.push_back(new Button(ButtonType::Exit, UITexture, m_arialFont, Vector2f(550, 680), Vector2f(1.0f, 1.0f)));
	m_menuButtons[0]->scaleUp();


	m_highscoreSprite.setTexture(UITexture);
	m_highscoreSprite.setTextureRect(IntRect{ 89, 2367, 627, 155 });
	m_highscoreSprite.setOrigin(627.0f / 2.0f, 118.0f / 2.0f);
	m_highscoreSprite.setPosition(m_menuCamera.getCenter().x + 375, 150);
	m_highscoreSprite.setScale(1.0f, 1.0f);

	m_highscoreText.setFont(m_arialFont);
	m_highscoreText.setStyle(sf::Text::Bold);
	m_highscoreText.setCharacterSize(30U);
	m_highscoreText.setOutlineColor(sf::Color::Black);
	m_highscoreText.setFillColor(sf::Color::White);
	m_highscoreText.setOutlineThickness(2.0f);
	m_highscoreText.setString(to_string(highScore));
	m_highscoreText.setOrigin(m_highscoreText.getGlobalBounds().width / 2.0f, m_highscoreText.getGlobalBounds().height / 2.0f);
	m_highscoreText.setPosition(m_highscoreSprite.getPosition().x + 190, m_highscoreSprite.getPosition().y + 10.0f);


	m_rankBadgeSprite.setTexture(m_holder["starterAtlas"]);
	m_rankBadgeSprite.setTextureRect(IntRect{ (2192 + (136 * (m_currentLevel - 1))), 1472, 136, 33 });
	m_rankBadgeSprite.setOrigin(136.0f / 2.0f, 33.0f / 2.0f); 
	m_rankBadgeSprite.setPosition(m_highscoreSprite.getPosition().x - 41.0f, m_highscoreSprite.getPosition().y + 72.5f);
	m_rankBadgeSprite.setScale(1.0f, 1.0f);
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

	InfoImgBGSprite.setTexture(UITexture);
	InfoImgBGSprite.setTextureRect(IntRect({ 1000, 0, 250, 250 }));
	InfoImgBGSprite.setOrigin(125.0f, 125.0f);
	InfoImgBGSprite.setPosition(m_playerCamera.getCenter());

	InfoTxtBGSprite.setTexture(UITexture);
	InfoTxtBGSprite.setTextureRect(IntRect({ 1250, 0, 250, 250 }));
	InfoTxtBGSprite.setOrigin(125.0f, 125.0f);
	InfoTxtBGSprite.setPosition(m_playerCamera.getCenter());

	m_gunInfoCurrentFrame = 0;
	m_gunInfoTime = seconds(0.1f);

#pragma region dash
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

	gunInfoImgSprite.setTexture(m_holder["starterAtlas"]);
	gunInfoImgSprite.setTextureRect({ 0,0,128,32 }); // pistol by default
	gunInfoImgSprite.setOrigin({ 128 / 2.0f,16.0f });

	gunLvlInfoTxt.setFont(m_arialFont);
	gunLvlInfoTxt.setStyle(sf::Text::Bold);
	gunLvlInfoTxt.setCharacterSize(40U);
	gunLvlInfoTxt.setOutlineColor(sf::Color::Black);
	gunLvlInfoTxt.setOutlineThickness(3U);
	gunLvlInfoTxt.setFillColor(sf::Color::White);

	gunDescriptionText.setFont(m_arialFont);
	gunDescriptionText.setStyle(sf::Text::Bold);
	gunDescriptionText.setCharacterSize(25U);
	gunDescriptionText.setOutlineColor(sf::Color::Black);
	gunDescriptionText.setOutlineThickness(2.0f);
	gunDescriptionText.setFillColor(sf::Color::White);

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
	m_timeSurvivedText.setString("Time Survived: " + to_string(timeSurvivedMinutes) + ":" + to_string(timeSurvivedSeconds));
	m_timeSurvivedText.setOrigin(0.0f, m_timeSurvivedText.getGlobalBounds().height / 2.0f);
	m_timeSurvivedText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 250);

	m_inGameTimerText.setFont(m_arialFont);
	m_inGameTimerText.setStyle(sf::Text::Bold);
	m_inGameTimerText.setCharacterSize(36U);
	m_inGameTimerText.setOutlineColor(sf::Color::Black);
	m_inGameTimerText.setFillColor(sf::Color::White);
	m_inGameTimerText.setOutlineThickness(2.0f);
	m_inGameTimerText.setString(to_string(timeSurvivedMinutes) + ":" + to_string(timeSurvivedSeconds));
	m_inGameTimerText.setOrigin(0.0f, m_inGameTimerText.getGlobalBounds().height / 2.0f);
	m_inGameTimerText.setPosition(0.0f,0.0f);

	m_scoreText.setFont(m_arialFont);
	m_scoreText.setStyle(sf::Text::Bold);
	m_scoreText.setCharacterSize(30U);
	m_scoreText.setOutlineColor(sf::Color::Black);
	m_scoreText.setFillColor(sf::Color::White);
	m_scoreText.setOutlineThickness(2.0f);
	m_scoreText.setString(to_string(score));
	m_scoreText.setOrigin(m_scoreText.getGlobalBounds().width / 2.0f, m_scoreText.getGlobalBounds().height / 2.0f);
	m_scoreText.setPosition(m_scoreSprite.getPosition().x + 150, m_scoreSprite.getPosition().y);

	m_currentLevelText.setFont(m_arialFont);
	m_currentLevelText.setCharacterSize(18.0f);
	m_currentLevelText.setOutlineColor(sf::Color::Black);
	m_currentLevelText.setFillColor(sf::Color::White);
	m_currentLevelText.setOutlineThickness(2.0f);
	m_currentLevelText.setString("Level: " + to_string(m_currentLevel));
	m_currentLevelText.setOrigin(m_currentLevelText.getGlobalBounds().width / 2.0f, m_currentLevelText.getGlobalBounds().height / 2.0f);
	m_currentLevelText.setPosition(0.0f, 0.0f);

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

	isFirstPage = true;

	m_player.initialise();
	m_currentLevel = 1;
	hasGameOverTimerStarted = false;

	m_gameplayMusic.play();
	m_menuMusic.stop();

	score = 0;
	smallEK = 0;
	bigEK = 0;
	bossEK = 0;
	updateCount = 0;
	timeSurvived = 0;
	timeSurvivedSeconds = 0;
	timeSurvivedMinutes = 0;

	////VECTOR INTIALISATION
	//ENEMY

	//m_enemies.clear();
	for (auto it = m_enemies.begin(); it != m_enemies.end();)
	{
		if (*it)
		{
			delete* it; // Delete the object
		}
		it = m_enemies.erase(it); // Remove the pointer from the vector
	}

	for (int i = 0; i < 10; i++) {
		m_enemies.push_back(new Enemy(m_holder["starterAtlas"], m_player.getPosition(), EnemyType::Small));
	}

	//m_xpOrbs.clear();
	for (auto it = m_xpOrbs.begin(); it != m_xpOrbs.end();)
	{
		if (*it)
		{
			delete* it; // Delete the object
		}
		it = m_xpOrbs.erase(it); // Remove the pointer from the vector
	}


	//m_pickups.clear();
	for (auto it = m_pickups.begin(); it != m_pickups.end();)
	{
		if (*it)
		{
			delete* it; // Delete the object
		}
		it = m_pickups.erase(it); // Remove the pointer from the vector
	}


	//OBSTACLES
	//m_obstacles.clear();
	for (auto it = m_obstacles.begin(); it != m_obstacles.end();)
	{
		if (*it)
		{
			delete* it; // Delete the object
		}
		it = m_obstacles.erase(it); // Remove the pointer from the vector
	}

	for (int i = 0; i < 3; i++)
	{
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock1, m_player.getPosition()));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock2, m_player.getPosition()));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Rock3, m_player.getPosition()));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Tree, m_player.getPosition()));
		m_obstacles.push_back(new Obstacle(m_holder["obstacleAtlas"], ObstacleType::Building, m_player.getPosition()));
	}

	m_gunInfoCurrentFrame = 0;
	m_gunInfoTime = seconds(0.1f);
	m_gunInfoFrames.clear();

	orbRumbleTimer.restart();
	pickupRumbleTimer.restart();
	enemyHitRumbleTimer.restart();
	levelUpRumbleTimer.restart();

	oIsRumbling = false;
	pIsRumbling = false;
	eIsRumbling = false;
	luIsRumbling = false;

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
#pragma region Controller input
			//D-Pad pressed
			if (Event::JoystickMoved == event.type) 
			{
				if (event.joystickMove.axis == sf::Joystick::PovY) // D-pad up/down
				{
					if (!inMenu)
					{
						if (event.joystickMove.position == -100) // Down
						{ 
							m_menuButtons[m_cursorPos]->scaleDown();
							m_cursorPos++;
							m_menuScrollSound.stop();
							m_menuScrollSound.play();
						}
						else if (event.joystickMove.position == 100) // Up
						{
							m_menuButtons[m_cursorPos]->scaleDown();
							m_cursorPos--;
							m_menuScrollSound.stop();
							m_menuScrollSound.play();
						}
					}
				}
				if (event.joystickMove.axis == sf::Joystick::PovX) // D-pad left/right
				{
					if (m_cursorButtonType == ButtonType::Tutorial && inMenu)
					{
						if (event.joystickMove.position == -100 && !isFirstPage)// left
						{
							menuBgSprite.setTexture(m_holder["tutorialMenu"]);
							isFirstPage = true;
							m_menuScrollSound.stop();
							m_menuScrollSound.play();
						}
						if (event.joystickMove.position == 100 && isFirstPage) // right
						{
							menuBgSprite.setTexture(m_holder["rankedScreen"]);
							isFirstPage = false;
							m_menuScrollSound.stop();
							m_menuScrollSound.play();
						}
					}
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
				m_menuButtons[m_cursorPos]->scaleUp();
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
								m_menuButtons[m_cursorPos]->scaleDown();
							}
							break;
						case ButtonType::Tutorial:
							inMenu = true;
							isFirstPage = true;
							menuBgSprite.setTexture(m_holder["tutorialMenu"]);
							break;
						case ButtonType::Credits:
							inMenu = true;
							menuBgSprite.setTexture(m_holder["creditsMenu"]); // PLEASE REMEBER TO CHANGE THE HOLDER TO CRREDITS WHEN BG IS READY
							break;
						case ButtonType::Exit:
							m_player.rumbleStop();
							m_window.close();
							break;
						}
						m_menuSelectSound.stop();
						m_menuSelectSound.play();
					}
				}
				else {
					if (event.joystickButton.button == 1) {
						inMenu = false;
						menuBgSprite.setTexture(m_holder["mainMenuBG"]);
						m_menuSelectSound.stop();
						m_menuSelectSound.play();
					}
				}
			}
#pragma endregion

			break;
#pragma endregion

#pragma region Pause input handling
		case Gamemode::Pause:
#pragma region Controller input
			//D-Pad pressed
			if (Event::JoystickMoved == event.type) {
				if (event.joystickMove.axis == sf::Joystick::PovX) // D-pad left/right
				{
					if (event.joystickMove.position == -100) // Left
					{
						m_pauseButtons[m_cursorPos]->scaleDown();
						m_cursorPos--;
						m_menuScrollSound.stop();
						m_menuScrollSound.play();
					}
					else if (event.joystickMove.position == 100) // Right
					{
						m_pauseButtons[m_cursorPos]->scaleDown();
						m_cursorPos++;
						m_menuScrollSound.stop();
						m_menuScrollSound.play();
					}	
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
				m_pauseButtons[m_cursorPos]->scaleUp();
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
					m_menuSelectSound.stop();
					m_menuSelectSound.play();
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
#pragma region Controller input
			//D-Pad pressed
			if (Event::JoystickMoved == event.type) {
				if (event.joystickMove.axis == sf::Joystick::PovY) // D-pad up/down
				{
					if (event.joystickMove.position == -100) // Down
					{
						m_upgradeButtons[m_cursorPos]->scaleDown();
						m_cursorPos++;
						m_menuScrollSound.stop();
						m_menuScrollSound.play();
					}	
					else if (event.joystickMove.position == 100) // Up
					{
						m_upgradeButtons[m_cursorPos]->scaleDown();
						m_cursorPos--;
						m_menuScrollSound.stop();
						m_menuScrollSound.play();
					}
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
				m_upgradeButtons[m_cursorPos]->scaleUp();
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
					case ButtonType::UpgradeMagnet:
						m_player.upgradePlayer(PlayerUpgrade::Magnet);
						break;
					}
					m_menuSelectSound.stop();
					m_menuSelectSound.play();

					m_currentGamemode = Gamemode::CarePackage;

					gunInfoImgSprite.setPosition(InfoImgBGSprite.getPosition());
					gunLvlInfoTxt.setPosition(InfoTxtBGSprite.getPosition().x, InfoTxtBGSprite.getPosition().y - 50);
					gunDescriptionText.setPosition(gunLvlInfoTxt.getPosition().x, gunLvlInfoTxt.getPosition().y + 60);

					m_cursorPos = 0;
					createRandomWeapons();
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
#pragma region Controller input
			//D-Pad pressed
			if (Event::JoystickMoved == event.type) {
				if (event.joystickMove.axis == sf::Joystick::PovY) // D-pad up/down
				{
					if (event.joystickMove.position == -100) // Down
					{
						m_weaponButtons[m_cursorPos]->scaleDown();
						m_cursorPos++;
						m_menuScrollSound.stop();
						m_menuScrollSound.play();

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
						m_weaponButtons[m_cursorPos]->scaleUp();

						setGunInfo();
					}
					else if (event.joystickMove.position == 100) // Up
					{
						m_weaponButtons[m_cursorPos]->scaleDown();
						m_cursorPos--;
						m_menuScrollSound.stop();
						m_menuScrollSound.play();

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
						m_weaponButtons[m_cursorPos]->scaleUp();

						setGunInfo();
					}
				}
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
					case ButtonType::GetSniper:
						m_player.giveWeapon(WeaponType::Sniper);
						break;
					case ButtonType::GetRPG:
						m_player.giveWeapon(WeaponType::RPG);
						break;
					case ButtonType::UpgradePistol:
						m_player.upgradeGun(WeaponType::Pistol); // go back to player and refreactor to allow for pistol and ar
						break;
					case ButtonType::UpgradeAR:
						m_player.upgradeGun(WeaponType::AssaultRifle);
						break;
					case ButtonType::UpgradeSniper:
						m_player.upgradeGun(WeaponType::Sniper); // go back to player and refreactor to allow for pistol and ar
						break;
					case ButtonType::UpgradeRPG:
						m_player.upgradeGun(WeaponType::RPG);
						break;
					}
					m_currentGamemode = Gamemode::Gameplay;
					m_menuSelectSound.stop();
					m_menuSelectSound.play();
				}
			}
#pragma endregion

			break;

#pragma endregion

#pragma region Gameplay input handling
		case Gamemode::Gameplay:
#pragma region Controller Input
			if (event.joystickButton.button == 7) {
				m_currentGamemode = Gamemode::Pause;
				pauseBgSprite.setPosition(m_playerCamera.getCenter());
				m_vignetteSprite.setPosition(m_playerCamera.getCenter());
				m_pauseButtons[0]->setPosition(Vector2f(m_playerCamera.getCenter().x - 250, m_playerCamera.getCenter().y));
				m_pauseButtons[1]->setPosition(Vector2f(m_playerCamera.getCenter().x + 250, m_playerCamera.getCenter().y));
				m_cursorPos = 0;
				m_cursorSprite.setPosition(m_pauseButtons[m_cursorPos]->getPositon());
				m_cursorButtonType = m_pauseButtons[m_cursorPos]->getType();
				m_pauseButtons[m_cursorPos]->scaleUp();
				m_pauseButtons[m_cursorPos + 1]->scaleDown();
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
					{
						m_gameoverButtons[m_cursorPos]->scaleDown();
						m_cursorPos--;
						m_menuScrollSound.stop();
						m_menuScrollSound.play();
					}
					else if (event.joystickMove.position == 100) // Right
					{
						m_gameoverButtons[m_cursorPos]->scaleDown();
						m_cursorPos++;
						m_menuScrollSound.stop();
						m_menuScrollSound.play();
					}
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
				m_gameoverButtons[m_cursorPos]->scaleUp();
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
					m_menuSelectSound.stop();
					m_menuSelectSound.play();
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
#pragma region Gameplay Gamemode logic
	if (m_currentGamemode == Gamemode::Gameplay) //switching between screens
	{

#pragma region camera movement
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

#pragma endregion

		//cout << m_view.getCenter().x << "	" << m_view.getCenter().y << "\n";

		levelUpSpawner();
		checkCollisions();

		m_player.update(dt, m_playerCamera, m_enemies);

#pragma region iterators
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

#pragma endregion


		if (!m_bossSpawned && timeSurvived > 120)
		{
			m_enemies.push_back(new Enemy(m_holder["starterAtlas"], m_player.getPosition(), EnemyType::Boss));
			m_bossSpawned = true;
		}

		//std::cout << m_enemies.size() << std::endl;

		//levelUpSpawner();
		//checkCollisions();



		//time in game
		if (updateCount > 60) {
			timeSurvived++;
			timeSurvivedSeconds++;
			updateCount = 0;
		}
		updateCount++;

		if (timeSurvivedSeconds > 59)
		{
			timeSurvivedMinutes++;
			timeSurvivedSeconds = 0;

			if (m_bossSpawned && timeSurvived > 300 && timeSurvivedMinutes % 2 == 0)
			{
				m_enemies.push_back(new Enemy(m_holder["starterAtlas"], m_player.getPosition(), EnemyType::Boss));
			}
		}

		if (timeSurvivedSeconds < 10)
		{
			m_inGameTimerText.setString(to_string(timeSurvivedMinutes) + ":0" + to_string(timeSurvivedSeconds));
		}
		else
		{
			m_inGameTimerText.setString(to_string(timeSurvivedMinutes) + ":" + to_string(timeSurvivedSeconds));
		}
		
		m_inGameTimerText.setOrigin(0.0f, m_inGameTimerText.getGlobalBounds().height / 2.0f);
		m_inGameTimerText.setPosition(m_playerCamera.getCenter().x + 625.0f, m_playerCamera.getCenter().y - 415.0f);

		m_currentLevelText.setString("Level: " + to_string(m_currentLevel));
		m_currentLevelText.setOrigin(0.0f, m_currentLevelText.getGlobalBounds().height / 2.0f);
		m_currentLevelText.setPosition(m_playerCamera.getCenter().x - 450.0f, m_playerCamera.getCenter().y - 412.0f);

#pragma region Gameover logic
		if (!m_player.getAliveState() && !hasGameOverTimerStarted) {
			m_gameOverTimer.restart();
			hasGameOverTimerStarted = true;
		}
		if (m_gameOverTimer.getElapsedTime().asSeconds() > 1.5f && !m_player.getAliveState()) {
			isGameOver = false;
			//switch to game over game mode here
			m_currentGamemode = Gamemode::GameOver;

			m_cursorPos = 0;
			m_cursorSprite.setPosition(m_gameoverButtons[m_cursorPos]->getPositon());
			m_cursorButtonType = m_gameoverButtons[m_cursorPos]->getType();
			float multiplier = 1.0f + (0.2f * m_currentLevel);
			totalEK = smallEK + bigEK + bossEK;
			score = ((smallEK * 1.0f) + (bigEK * 2.0f) + (bossEK * 100.0f)) * multiplier;

			if (score > highScore) {
				highScore = score;
				m_highscoreText.setString(to_string(highScore));
				m_highscoreText.setOrigin(m_highscoreText.getGlobalBounds().width / 2.0f, m_highscoreText.getGlobalBounds().height / 2.0f);
				m_highscoreText.setPosition(m_highscoreSprite.getPosition().x + 140, m_highscoreSprite.getPosition().y + 7.5f);
			}

			if (highScore < 500) { m_playerRank = 0; }
			if (highScore >= 1000) { m_playerRank = 1; }
			if (highScore >= 2500) { m_playerRank = 2; }
			if (highScore >= 5000) { m_playerRank = 3; }
			if (highScore >= 7500) { m_playerRank = 4; }
			if (highScore >= 10000) { m_playerRank = 5; }
			m_rankBadgeSprite.setTextureRect(IntRect{ (2192 + (136 * (m_playerRank))), 1472, 136, 33 });

			m_player.rumbleStop();
#pragma region Menu UI setup
#pragma region Sprites
			m_scoreSprite.setPosition(m_playerCamera.getCenter().x, m_playerCamera.getCenter().y + 250);
			m_scoreVarBGSprite.setPosition(m_playerCamera.getCenter().x, m_playerCamera.getCenter().y + 50);
			gameOverBGSprite.setPosition(m_playerCamera.getCenter());
			m_vignetteSprite.setPosition(m_playerCamera.getCenter());
			m_gameoverButtons[0]->setPosition(Vector2f(m_playerCamera.getCenter().x - 275, m_playerCamera.getCenter().y + 365));
			m_gameoverButtons[1]->setPosition(Vector2f(m_playerCamera.getCenter().x + 275, m_playerCamera.getCenter().y + 365));
#pragma endregion

#pragma region Text
			m_statsText.setPosition(m_scoreVarBGSprite.getPosition().x, m_scoreVarBGSprite.getPosition().y - 105);

			m_smallEKText.setString("Normal enemies killed: " + to_string(smallEK));
			m_smallEKText.setOrigin(0.0f, m_smallEKText.getGlobalBounds().height / 2.0f);
			m_smallEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 70);

			m_bigEKText.setString("Big enemies killed: " + to_string(bigEK));
			m_bigEKText.setOrigin(0.0f, m_bigEKText.getGlobalBounds().height / 2.0f);
			m_bigEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 100);

			m_bossEKText.setString("Boss enemies killed: " + to_string(bossEK));
			m_bossEKText.setOrigin(0.0f, m_bigEKText.getGlobalBounds().height / 2.0f);
			m_bossEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 130);

			m_totalEKText.setString("TOTAL ENEMIES KILLED: " + to_string(totalEK));
			m_totalEKText.setOrigin(0.0f, m_totalEKText.getGlobalBounds().height / 2.0f);
			m_totalEKText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 160);

			m_playerLevelText.setString("Player Level: " + to_string(m_currentLevel));
			m_playerLevelText.setOrigin(0.0f, m_playerLevelText.getGlobalBounds().height / 2.0f);
			m_playerLevelText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 190);

			if (timeSurvivedSeconds < 10)
			{
				m_timeSurvivedText.setString("Time Survived: " + to_string(timeSurvivedMinutes) + ":0" + to_string(timeSurvivedSeconds));
			}
			else
			{
				m_timeSurvivedText.setString("Time Survived: " + to_string(timeSurvivedMinutes) + ":" + to_string(timeSurvivedSeconds));
			}
			m_timeSurvivedText.setOrigin(0.0f, m_timeSurvivedText.getGlobalBounds().height / 2.0f);
			m_timeSurvivedText.setPosition(m_scoreVarBGSprite.getPosition().x - 205, (m_scoreVarBGSprite.getPosition().y - 125) + 220);

			m_scoreText.setString(to_string(score));
			m_scoreText.setOrigin(m_scoreText.getGlobalBounds().width / 2.0f, m_scoreText.getGlobalBounds().height / 2.0f);
			m_scoreText.setPosition(m_scoreSprite.getPosition().x + 150, m_scoreSprite.getPosition().y);

#pragma endregion

#pragma endregion

		}
#pragma endregion		

	}

#pragma endregion

	//switching camera modes
	switch (m_currentGamemode) {
	case Gamemode::Menu:
		m_window.setView(m_menuCamera);
		break;
	case Gamemode::Gameplay:
		m_window.setView(m_playerCamera);
		m_hudSprite.setPosition(m_playerCamera.getCenter());
	}

	if (m_currentGamemode == Gamemode::CarePackage) //setup upgrade screen here
	{
		if (m_gunInfoClock.getElapsedTime() > m_gunInfoTime)
		{
			if (m_gunInfoCurrentFrame + 1 < m_gunInfoFrames.size())
			{
				m_gunInfoCurrentFrame++;
			}
			else
			{
				m_gunInfoCurrentFrame = 0;
			}
			m_gunInfoClock.restart();

			if (m_gunInfoFrames.size() == 11)
			{
				gunInfoImgSprite.setTextureRect(m_gunInfoFrames[m_gunInfoCurrentFrame]);
			}
		}
	}

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
	if (levelUpRumbleTimer.getElapsedTime().asSeconds() > 0.65f && luIsRumbling) {
		luIsRumbling = false;
	}
	if (!oIsRumbling && !pIsRumbling && !eIsRumbling && !luIsRumbling && !m_player.getRumbleState()) {
		m_player.rumbleStop();
	}
}

void Game::render()
{

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

		m_player.renderPlayer(m_window, m_outlineShader);

		for (auto obstacle : m_obstacles)
		{
			obstacle->renderTop(m_window);
		}

		if (m_currentGamemode == Gamemode::Gameplay) {
			m_window.draw(m_hudSprite);
			m_window.draw(m_inGameTimerText);
		}

		m_player.renderHUD(m_window);
		m_window.draw(m_currentLevelText);

#pragma endregion

		if (m_currentGamemode == Gamemode::Pause ||
			m_currentGamemode == Gamemode::Upgrade || m_currentGamemode == Gamemode::CarePackage ||
			m_currentGamemode == Gamemode::GameOver) {
		m_window.draw(m_vignetteSprite);
		}


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
			m_window.draw(InfoImgBGSprite);
			m_window.draw(InfoTxtBGSprite);
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
			m_window.draw(InfoImgBGSprite);
			m_window.draw(InfoTxtBGSprite);
			m_window.draw(gunInfoImgSprite);
			m_window.draw(gunLvlInfoTxt);
			m_window.draw(gunDescriptionText);

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
			m_window.draw(m_bigEKText);
			m_window.draw(m_bossEKText);
			m_window.draw(m_totalEKText);
			m_window.draw(m_playerLevelText);
			m_window.draw(m_timeSurvivedText);
			m_window.draw(m_scoreText);
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
			m_window.draw(m_highscoreSprite);
			m_window.draw(m_highscoreText);
			if (!firstStart) { m_window.draw(m_rankBadgeSprite); } 
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
					enemy->adjustMovement(obstacle);
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
					smallEK++;
					break;
				case EnemyType::Big:
					enemy->setState(CharacterState::DeadState);
					bigEK++;
					break;
				case EnemyType::Boss:
					enemy->decreaseHealth(5.0f);

					enemy->setColour(sf::Color::Red);

					if (enemy->getHealth() < 0)
					{
						bossEK++;
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
					smallEK++;
					break;
				case EnemyType::Big:
					enemy->setState(CharacterState::DeadState);
					bigEK++;
					break;
				case EnemyType::Boss:
					enemy->decreaseHealth(10.0f);

					enemy->setColour(sf::Color::Red);

					if (enemy->getHealth() < 0)
					{
						bossEK++;
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
			for (auto weapon : m_player.getWeapons())
			{
				if (weapon->getType() == WeaponType::Pistol || weapon->getType() == WeaponType::AssaultRifle || weapon->getType() == WeaponType::RPG) // If bullet is meant to delete when collided with enemy
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
								smallEK++;
								break;
							case EnemyType::Big:
								enemy->playHitSound();
								enemy->setState(CharacterState::DeadState);
								bigEK++;
								break;
							case EnemyType::Boss:
								enemy->playHitSound();
								enemy->decreaseHealth((*it)->getDamage());

								enemy->setColour(sf::Color::Red);

								if (enemy->getHealth() < 0.0f)
								{
									enemy->setState(CharacterState::DeadState);
									bossEK++;
								}
								break;
							default:
								break;
							}

							if (weapon->getType() == WeaponType::RPG)
							{
								m_explosionSound.play();
								weapon->getExplosion().currentFrame = 0;
								weapon->getExplosion().animationOver = false;

								weapon->getExplosion().position = (*it)->getPosition();
								weapon->getExplosion().radius.setPosition(weapon->getExplosion().position);
								weapon->getExplosion().sprite.setPosition(weapon->getExplosion().position.x, weapon->getExplosion().position.y - 40.0f);
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
								smallEK++;
								enemy->playHitSound();
								enemy->setState(CharacterState::DeadState);
								break;
							case EnemyType::Big:
								bigEK++;
								enemy->playHitSound();
								enemy->setState(CharacterState::DeadState);
								break;
							case EnemyType::Boss:
								enemy->playHitSound();
								enemy->decreaseHealth(bullet->getDamage() / 100.0f);
								
								enemy->setColour(sf::Color::Red);

								if (enemy->getHealth() < 0)
								{
									enemy->setState(CharacterState::DeadState);
									bossEK++;
								}
							default:
								break;
							}
						}
					}
				}
				
				if (weapon->getType() == WeaponType::RPG)
				{
					if (CollisionDetection::enemyExplosionCollision(enemy, weapon->getExplosion()))
					{
						switch (enemy->getType())
						{
						case EnemyType::Small:
							smallEK++;
							enemy->setState(CharacterState::DeadState);
							break;
						case EnemyType::Big:
							bigEK++;
							enemy->setState(CharacterState::DeadState);
							break;
						case EnemyType::Boss:
							enemy->decreaseHealth(20.0f);

							enemy->setColour(sf::Color::Red);

							if (enemy->getHealth() < 0)
							{
								enemy->setState(CharacterState::DeadState);
								bossEK++;
							}
						default:
							break;
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

				m_player.setDamageIndicator(sf::Color::Red);
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

			m_player.playSound(m_orbPickupSound);
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
			m_player.playSound(m_healthPickupSound);
			m_player.increaseHealth();
			m_player.setHealIndicator(sf::Color::Green);

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
	//m_upgradeButtons.clear();
	for (auto it = m_upgradeButtons.begin(); it != m_upgradeButtons.end();)
	{
		if (*it)
		{
			delete* it; // Delete the object
		}
		it = m_upgradeButtons.erase(it); // Remove the pointer from the vector
	}

	ButtonType randomUpgradeButton1 = static_cast<ButtonType>((rand() % 5) + static_cast<int>(ButtonType::UpgradeHealth));
	ButtonType randomUpgradeButton2 = static_cast<ButtonType>((rand() % 5) + static_cast<int>(ButtonType::UpgradeHealth));
	while (randomUpgradeButton1 == randomUpgradeButton2) { randomUpgradeButton2 = static_cast<ButtonType>((rand() % 5) + static_cast<int>(ButtonType::UpgradeHealth)); }
	ButtonType randomUpgradeButton3 = static_cast<ButtonType>((rand() % 5) + static_cast<int>(ButtonType::UpgradeHealth));
	while (randomUpgradeButton3 == randomUpgradeButton1 || randomUpgradeButton3 == randomUpgradeButton2)
	{
		randomUpgradeButton3 = static_cast<ButtonType>((rand() % 5) + static_cast<int>(ButtonType::UpgradeHealth));
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
	//m_weaponButtons.clear();
	for (auto it = m_weaponButtons.begin(); it != m_weaponButtons.end();)
	{
		if (*it)
		{
			delete* it; // Delete the object
		}
		it = m_weaponButtons.erase(it); // Remove the pointer from the vector
	}

	ButtonType randomUpgradeButton1 = static_cast<ButtonType>((rand() % 4) + static_cast<int>(ButtonType::GetPistol));
	ButtonType randomUpgradeButton2 = static_cast<ButtonType>((rand() % 4) + static_cast<int>(ButtonType::GetPistol));
	ButtonType randomUpgradeButton3 = static_cast<ButtonType>((rand() % 4) + static_cast<int>(ButtonType::GetPistol));

	while (randomUpgradeButton1 == randomUpgradeButton2) { randomUpgradeButton2 = static_cast<ButtonType>((rand() % 4) + static_cast<int>(ButtonType::GetPistol)); }
	while (randomUpgradeButton3 == randomUpgradeButton1 || randomUpgradeButton3 == randomUpgradeButton2)
	{
		randomUpgradeButton3 = static_cast<ButtonType>((rand() % 4) + static_cast<int>(ButtonType::GetPistol));
	}


	bool isEquipped1 = false;
	bool isEquipped2 = false;
	bool isEquipped3 = false;
	WeaponType btnWpnTyp1 = WeaponType::Count;
	WeaponType btnWpnTyp2 = WeaponType::Count;
	WeaponType btnWpnTyp3 = WeaponType::Count;
	switch (randomUpgradeButton1) {
	case (0 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp1 = WeaponType::Pistol;
		break;
	case (1 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp1 = WeaponType::AssaultRifle;
		break;
	case (2 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp1 = WeaponType::Sniper;
		break;
	case (3 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp1 = WeaponType::RPG;
		break;
	}
	switch (randomUpgradeButton2) {
	case (0 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp2 = WeaponType::Pistol;
		break;
	case (1 + static_cast<int>(ButtonType::GetPistol)): //remember that this calculation is to get the position of get pistol and get ar bt types
		btnWpnTyp2 = WeaponType::AssaultRifle;
		break;
	case (2 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp2 = WeaponType::Sniper;
		break;
	case (3 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp2 = WeaponType::RPG;
		break;
	}
	switch (randomUpgradeButton3) {
	case (0 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp3 = WeaponType::Pistol;
		break;
	case (1 + static_cast<int>(ButtonType::GetPistol)): //remember that this calculation is to get the position of get pistol and get ar bt types
		btnWpnTyp3 = WeaponType::AssaultRifle;
		break;
	case (2 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp3 = WeaponType::Sniper;
		break;
	case (3 + static_cast<int>(ButtonType::GetPistol)):
		btnWpnTyp3 = WeaponType::RPG;
		break;
	}
	for (auto weapon : m_player.getWeapons()) {
		if (btnWpnTyp1 == weapon->getType()) {
			isEquipped1 = true;
		}
		if (btnWpnTyp2 == weapon->getType()) {
			isEquipped2 = true;
		}
		if (btnWpnTyp3 == weapon->getType()) {
			isEquipped3 = true;
		}
	}
	if (isEquipped1) {
		if (btnWpnTyp1 == WeaponType::Pistol) {
			randomUpgradeButton1 = ButtonType::UpgradePistol;
		}
		if (btnWpnTyp1 == WeaponType::AssaultRifle) {
			randomUpgradeButton1 = ButtonType::UpgradeAR;
		}
		if (btnWpnTyp1 == WeaponType::Sniper) {
			randomUpgradeButton1 = ButtonType::UpgradeSniper;
		}
		if (btnWpnTyp1 == WeaponType::RPG) {
			randomUpgradeButton1 = ButtonType::UpgradeRPG;
		}
	}
	if (isEquipped2) {
		if (btnWpnTyp2 == WeaponType::Pistol) {
			randomUpgradeButton2 = ButtonType::UpgradePistol;
		}
		if (btnWpnTyp2 == WeaponType::AssaultRifle) {
			randomUpgradeButton2 = ButtonType::UpgradeAR;
		}
		if (btnWpnTyp2 == WeaponType::Sniper) {
			randomUpgradeButton2 = ButtonType::UpgradeSniper;
		}
		if (btnWpnTyp2 == WeaponType::RPG) {
			randomUpgradeButton2 = ButtonType::UpgradeRPG;
		}
	}
	if (isEquipped3) {
		if (btnWpnTyp3 == WeaponType::Pistol) {
			randomUpgradeButton3 = ButtonType::UpgradePistol;
		}
		if (btnWpnTyp3 == WeaponType::AssaultRifle) {
			randomUpgradeButton3 = ButtonType::UpgradeAR;
		}
		if (btnWpnTyp3 == WeaponType::Sniper) {
			randomUpgradeButton3 = ButtonType::UpgradeSniper;
		}
		if (btnWpnTyp3 == WeaponType::RPG) {
			randomUpgradeButton3 = ButtonType::UpgradeRPG;
		}
	}

	m_weaponButtons.push_back(new Button(randomUpgradeButton1, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y - 180), Vector2f(0.75f, 0.75f)));
	m_weaponButtons.push_back(new Button(randomUpgradeButton2, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y), Vector2f(0.75f, 0.75f)));
	m_weaponButtons.push_back(new Button(randomUpgradeButton3, m_holder["UIAtlas"], m_arialFont, Vector2f(m_playerCamera.getCenter().x - 175, m_playerCamera.getCenter().y + 180), Vector2f(0.75f, 0.75f)));


	setGunInfo();
}

void Game::setGunInfo()
{
	m_cursorSprite.setPosition(m_weaponButtons[m_cursorPos]->getPositon());
	m_cursorButtonType = m_weaponButtons[m_cursorPos]->getType();

	string heading;
	String description;
	description = "Testing, Testing, 1, 2\nnew line for look";
	int gunLevel = 1;
	WeaponType tempWpnType;


	switch (m_cursorButtonType) {
	case ButtonType::UpgradePistol:
		tempWpnType = WeaponType::Pistol;
		for (auto gun : m_player.getWeapons())
		{
			if (gun->getType() == tempWpnType) {
				gunLevel = gun->getWeaponLevel();
			}
		}
		break;
	case ButtonType::UpgradeAR:
		tempWpnType = WeaponType::AssaultRifle;
		for (auto gun : m_player.getWeapons())
		{
			if (gun->getType() == tempWpnType) {
				gunLevel = gun->getWeaponLevel();
			}
		}
		break;
	case ButtonType::UpgradeSniper:
		tempWpnType = WeaponType::Sniper;
		for (auto gun : m_player.getWeapons())
		{
			if (gun->getType() == tempWpnType) {
				gunLevel = gun->getWeaponLevel();
			}
		}
		break;
	case ButtonType::UpgradeRPG:
		tempWpnType = WeaponType::RPG;
		for (auto gun : m_player.getWeapons())
		{
			if (gun->getType() == tempWpnType) {
				gunLevel = gun->getWeaponLevel();
			}
		}
		break;
	}


	switch (m_cursorButtonType) 
	{
	case ButtonType::GetPistol:
		heading = "Get Pistol";
		description = "Fires one bullet at\nclosest enemy";
		gunInfoImgSprite.setTextureRect({ 0,0,128,32 });
		gunInfoImgSprite.setScale(1.8f, 1.8f);

		m_gunInfoCurrentFrame = 0;
		m_gunInfoFrames.clear();
		for (int i = 0; i < 11; i++)
		{
			m_gunInfoFrames.push_back(IntRect{ 128 * i,0,128,32 });
		}
		m_gunInfoTime = seconds(0.1f);
		break;
	case ButtonType::GetRifle:
		heading = "Get AR";
		description = "Fires burst in the\nfaced direction";
		gunInfoImgSprite.setTextureRect({ 0,32,128,32 });
		gunInfoImgSprite.setScale(1.8f, 1.8f);

		m_gunInfoCurrentFrame = 0;
		m_gunInfoFrames.clear();
		for (int i = 0; i < 11; i++)
		{
			m_gunInfoFrames.push_back(IntRect{ 128 * i,32,128,32 });
		}
		m_gunInfoTime = seconds(0.075f);
		break;
	case ButtonType::GetSniper:
		heading = "Get Sniper";
		description = "Fires one bullet at\nfurthest enemy";
		gunInfoImgSprite.setTextureRect({ 1584, 784, 256, 64 });
		gunInfoImgSprite.setScale(0.8f, 0.8f);

		m_gunInfoCurrentFrame = 0;
		m_gunInfoFrames.clear();
		for (int i = 0; i < 11; i++)
		{
			m_gunInfoFrames.push_back(IntRect{ 1584 + 256 * i,784, 256, 64 });
		}
		m_gunInfoTime = seconds(0.1f);
		break;
	case ButtonType::GetRPG:
		heading = "Get RPG";
		description = "Fires a rocket in\na random direction";
		gunInfoImgSprite.setTextureRect({ 1648, 994, 256, 64 });
		gunInfoImgSprite.setScale(0.8f, 0.8f);

		m_gunInfoCurrentFrame = 0;
		m_gunInfoFrames.clear();
		for (int i = 0; i < 11; i++)
		{
			m_gunInfoFrames.push_back(IntRect{ 1648 + 256 * i,994, 256, 64 });
		}
		m_gunInfoTime = seconds(0.08f);
		break;

	case ButtonType::UpgradePistol:
		heading = "PISTOL LVL " + to_string(gunLevel + 1);
		gunInfoImgSprite.setTextureRect({ 0,0,128,32 });
		gunInfoImgSprite.setScale(1.8f, 1.8f);

		m_gunInfoCurrentFrame = 0;
		m_gunInfoFrames.clear();
		for (int i = 0; i < 11; i++)
		{
			m_gunInfoFrames.push_back(IntRect{ 128 * i,0,128,32 });
		}
		m_gunInfoTime = seconds(0.1f);
		switch (gunLevel) {
		case 1:
			description = "Increase Fire Rate";
			break;
		case 2:
			description = "Increase Fire Rate";
			break;
		case 3:
			description = "Increase Fire Rate";
			break;
		case 4:
			description = "Increase Fire Rate";
			break;
		default:
			description = "GUN FULLY UPGRADED";
			break;
		}
		break;
	case ButtonType::UpgradeAR:
		heading = "RIFLE LVL " + to_string(gunLevel + 1);
		gunInfoImgSprite.setTextureRect({ 0,32,128,32 });
		gunInfoImgSprite.setScale(1.8f, 1.8f);

		m_gunInfoCurrentFrame = 0;
		m_gunInfoFrames.clear();
		for (int i = 0; i < 11; i++)
		{
			m_gunInfoFrames.push_back(IntRect{ 128 * i,32,128,32 });
		}
		m_gunInfoTime = seconds(0.075f);
		switch (gunLevel) {
		case 1:
			description = "Increase mag size +1";
			break;
		case 2:
			description = "Increase Fire Rate";
			break;
		case 3:
			description = "Increase mag size +1";
			break;
		case 4:
			description = "Increase Fire Rate\nIncrease mag size +3";
			break;
		default:
			description = "GUN FULLY UPGRADED";
			break;
		}
		break;
	case ButtonType::UpgradeSniper:
		heading = "SNIPER LVL " + to_string(gunLevel + 1);
		gunInfoImgSprite.setTextureRect({ 1584, 784, 256, 64 });
		gunInfoImgSprite.setScale(0.8f, 0.8f);

		m_gunInfoCurrentFrame = 0;
		m_gunInfoFrames.clear();
		for (int i = 0; i < 11; i++)
		{
			m_gunInfoFrames.push_back(IntRect{ 1584 + 256 * i,784, 256, 64 });
		}
		m_gunInfoTime = seconds(0.1f);
		switch (gunLevel) {
		case 1:
			description = "Increase Fire Rate";
			break;
		case 2:
			description = "Increase Fire Rate";
			break;
		case 3:
			description = "Increase Fire Rate";
			break;
		case 4:
			description = "Increase Fire Rate";
			break;
		default:
			description = "GUN FULLY UPGRADED";
			break;
		}
		break;
	case ButtonType::UpgradeRPG:
		heading = "RPG LVL " + to_string(gunLevel + 1);
		gunInfoImgSprite.setTextureRect({ 1648, 994, 256, 64 });
		gunInfoImgSprite.setScale(0.8f, 0.8f);

		m_gunInfoCurrentFrame = 0;
		m_gunInfoFrames.clear();
		for (int i = 0; i < 11; i++)
		{
			m_gunInfoFrames.push_back(IntRect{ 1648 + 256 * i,994, 256, 64 });
		}
		m_gunInfoTime = seconds(0.08f);
		switch (gunLevel) {
		case 1:
			description = "Increase Fire Rate";
			break;
		case 2:
			description = "Increase Power";
			break;
		case 3:
			description = "Increase Fire Rate\nIncrease Power";
			break;
		case 4:
			description = "Increase Power\nIncrease Fire Rate";
			break;
		default:
			description = "GUN FULLY UPGRADED";
			break;
		}
		break;
	}
	gunInfoImgSprite.setOrigin(gunInfoImgSprite.getTextureRect().width / 2.0f, gunInfoImgSprite.getTextureRect().height / 2.0f);

	gunLvlInfoTxt.setString(heading);//has to be weaopn level which should be gotten from for loop above
	gunLvlInfoTxt.setOrigin(gunLvlInfoTxt.getGlobalBounds().width / 2.0f, gunLvlInfoTxt.getGlobalBounds().height / 2.0f);
	gunDescriptionText.setString(description);
	gunDescriptionText.setOrigin(gunDescriptionText.getGlobalBounds().width / 2.0f, gunDescriptionText.getGlobalBounds().height / 2.0f);
	gunDescriptionText.setPosition(gunLvlInfoTxt.getPosition().x, gunLvlInfoTxt.getPosition().y + 60);

}

#pragma endregion

void Game::levelUpSpawner()
{
	if (m_player.getLevel() > m_currentLevel)
	{
		m_levelupSound.play();

		m_player.strongRumbleStart();
		levelUpRumbleTimer.restart();
		luIsRumbling = true; //level up has rumbled = luhr

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

		InfoImgBGSprite.setPosition(levelUpBGSprite.getPosition().x + 200, levelUpBGSprite.getPosition().y - 100);
		InfoTxtBGSprite.setPosition(InfoImgBGSprite.getPosition().x, (InfoImgBGSprite.getPosition().y + 250) + 10);
		dashInfoImgSprite.setPosition(InfoImgBGSprite.getPosition());
		dashInfoTxt.setPosition(InfoTxtBGSprite.getPosition().x, InfoTxtBGSprite.getPosition().y - 50);

		createRandomUpgrades();
		m_cursorPos = 0;
		m_cursorSprite.setPosition(m_upgradeButtons[m_cursorPos]->getPositon());
		m_cursorButtonType = m_upgradeButtons[m_cursorPos]->getType();

		m_vignetteSprite.setPosition(m_playerCamera.getCenter());
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