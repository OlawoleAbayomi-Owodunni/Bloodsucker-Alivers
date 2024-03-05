#pragma once

// If VS Debug build is enabled, then any block of code enclosed within 
//  the preprocessor directive #ifdef TEST_FPS is compiled and executed.
#ifdef _DEBUG
#define TEST_FPS
#endif // _DEBUG

#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include <string>
#include "ScreenSize.h"
#include "Player.h"
#include "Enemy.h"
#include "XPOrb.h"
#include "Pickup.h"
#include "Obstacle.h"
#include "CollisionDetection.h"

#include "Button.h"

/// <summary>
/// @brief Main class for the SFML Playground project.
/// 
/// This will be a single class framework for learning about SFML. 
/// Example usage:
///		Game game;
///		game.run();
/// </summary>

enum class Gamemode
{
	Menu,
	Gameplay,
	Pause,
	Upgrade,
	CarePackage,
	GameOver
};

class Game
{
public:
	Game();

	void run();

protected:
	void init();

	void update(double dt);

	void render();

	void processEvents();

	void processGameEvents(sf::Event&);

	void checkCollisions();

	void levelUpSpawner();
	void createRandomUpgrades();
	void setDashInfo();
	void createRandomWeapons();
	void setGunInfo();

	void dropLoot(Enemy* t_enemy);

	void playSound();

	void startGame();

	// Font used for all text
	sf::Font m_arialFont;
	// main window
	sf::RenderWindow m_window;
	sf::View m_playerCamera;
	View m_menuCamera;

	Gamemode m_currentGamemode;

	Player m_player;
	std::vector<Enemy*> m_enemies;
	std::vector<XPOrb*> m_xpOrbs;
	std::vector<Pickup*> m_pickups;
	std::vector<Obstacle*> m_obstacles;

	sf::SoundBuffer m_orbPickupSoundBuffer;
	sf::Sound m_orbPickupSound;

	sf::SoundBuffer m_healthPickupSoundBuffer;
	sf::Sound m_healthPickupSound;

	sf::SoundBuffer m_levelupSoundBuffer;
	sf::Sound m_levelupSound;

	sf::SoundBuffer m_explosionSoundBuffer;
	sf::Sound m_explosionSound;

	sf::SoundBuffer m_menuMusicBuffer;
	sf::Sound m_menuMusic;

	sf::SoundBuffer m_gameplayMusicBuffer;
	sf::Sound m_gameplayMusic;

	int m_currentLevel;
	Clock m_bossTimer;
	bool m_bossSpawned;

	thor::ResourceHolder<sf::Texture, std::string> m_holder;

	Sprite bgSprite;

	Clock orbRumbleTimer;
	Clock pickupRumbleTimer;
	Clock enemyHitRumbleTimer;
	Clock levelUpRumbleTimer;

	bool oIsRumbling, pIsRumbling, eIsRumbling, luIsRumbling;

#ifdef TEST_FPS
	sf::Text x_updateFPS;					// text used to display updates per second.
	sf::Text x_drawFPS;						// text used to display draw calls per second.
	sf::Time x_secondTime{ sf::Time::Zero };	// counter used to establish when a second has passed.
	int x_updateFrameCount{ 0 };			// updates per second counter.
	int x_drawFrameCount{ 0 };				// draws per second counter.
#endif // TEST_FPS

	//MENU VARIABLES
	Sprite menuBgSprite;
	vector<Button*> m_menuButtons;

	Sprite m_vignetteSprite;
	Sprite m_hudSprite; 

	Sprite pauseBgSprite;
	vector<Button*> m_pauseButtons;

	Sprite levelUpBGSprite;
	vector<Button*> m_upgradeButtons;
	vector<Button*> m_weaponButtons;

	Sprite InfoImgBGSprite;
	Sprite InfoTxtBGSprite;
	Sprite dashInfoImgSprite;
	Text dashInfoTxt;
	Text dashDescriptionText;
	vector<IntRect> dashInfoFrames;

	vector<sf::IntRect> m_gunInfoFrames;
	int m_gunInfoCurrentFrame;
	Time m_gunInfoTime;
	Clock m_gunInfoClock;

	Sprite gunInfoImgSprite;
	Text gunLvlInfoTxt;
	Text gunDescriptionText;

	Sprite gameOverBGSprite;
	vector<Button*> m_gameoverButtons;
	Sprite m_scoreVarBGSprite;
	Sprite m_scoreSprite;
	Sprite m_highscoreSprite;
	Text m_highscoreText;
	Text m_scoreRank;

	SoundBuffer m_menuScrollBuffer;
	Sound m_menuScrollSound;

	SoundBuffer m_menuSelectBuffer;
	Sound m_menuSelectSound;
	//Text for all the things to add

	Sprite m_cursorSprite;
	int m_cursorPos;
	ButtonType m_cursorButtonType;

	bool inMenu;

	Clock m_gameOverTimer;
	bool hasGameOverTimerStarted;
	bool isGameOver;
	bool firstStart;

	bool isFirstPage;

	int score;
	int smallEK;
	int bigEK;
	int bossEK;
	int totalEK;
	int updateCount;
	int timeSurvived;
	int timeSurvivedSeconds;
	int timeSurvivedMinutes;
	int highScore;

	Text m_scoreText;
	Text m_smallEKText;
	Text m_bigEKText;
	Text m_bossEKText;
	Text m_totalEKText;
	Text m_playerLevelText;
	Text m_timeSurvivedText;
	Text m_statsText;
	Text m_inGameTimerText;
	Text m_currentLevelText;

	Sprite m_rankBadgeSprite;
	int m_playerRank;


	sf::RenderTexture m_renderTexture;
	sf::Shader m_outlineShader;
};