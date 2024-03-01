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
	CarePackage
};

class Game
{
public:
	/// <summary>
	/// @brief Default constructor that initialises the SFML window, 
	///   and sets vertical sync enabled. 
	/// </summary>
	Game();

	/// <summary>
	/// @brief the main game loop.
	/// 
	/// A complete loop involves processing SFML events, updating and drawing all game objects.
	/// The actual elapsed time for a single game loop is calculated. If this value is 
	///  greater than the target time for one loop (1 / 60), then (and only then) is an update 
	///  operation performed.
	/// The target is at least one update and one render cycle per game loop, but typically 
	///  more render than update operations will be performed as we expect our game loop to
	///  complete in less than the target time.
	/// </summary>
	void run();

protected:
	/// <summary>
	/// @brief Once-off game initialisation code
	/// </summary>	
	void init();
	/// <summary>
	/// @brief Placeholder to perform updates to all game objects.
	/// </summary>
	/// <param name="time">update delta time</param>
	void update(double dt);

	/// <summary>
	/// @brief Draws the background and foreground game objects in the SFML window.
	/// The render window is always cleared to black before anything is drawn.
	/// </summary>
	void render();

	/// <summary>
	/// @brief Checks for events.
	/// Allows window to function and exit. 
	/// Events are passed on to the Game::processGameEvents() method.
	/// </summary>	
	void processEvents();

	/// <summary>
	/// @brief Handles all user input.
	/// </summary>
	/// <param name="event">system event</param>
	void processGameEvents(sf::Event&);

	void checkCollisions();

	void levelUpSpawner();
	void createRandomUpgrades();

	void createRandomWeapons();

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

	sf::SoundBuffer m_pickupSoundBuffer;
	sf::Sound m_pickupSound;

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

	bool oIsRumbling, pIsRumbling, eIsRumbling;

#ifdef TEST_FPS
	sf::Text x_updateFPS;					// text used to display updates per second.
	sf::Text x_drawFPS;						// text used to display draw calls per second.
	sf::Time x_secondTime {sf::Time::Zero};	// counter used to establish when a second has passed.
	int x_updateFrameCount{ 0 };			// updates per second counter.
	int x_drawFrameCount{ 0 };				// draws per second counter.
#endif // TEST_FPS

	//MENU VARIABLES
	Sprite menuBgSprite;
	vector<Button*> m_menuButtons;

	Sprite pauseBgSprite;
	vector<Button*> m_pauseButtons;

	Sprite levelUpBGSprite;
	vector<Button*> m_upgradeButtons;
	vector<Button*> m_weaponButtons;


	Sprite m_cursorSprite;
	int m_cursorPos; 
	ButtonType m_cursorButtonType;
};