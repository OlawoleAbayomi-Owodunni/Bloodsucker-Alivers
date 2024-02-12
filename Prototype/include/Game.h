#pragma once

// If VS Debug build is enabled, then any block of code enclosed within 
//  the preprocessor directive #ifdef TEST_FPS is compiled and executed.
#ifdef _DEBUG
#define TEST_FPS
#endif // _DEBUG

#include <SFML/Graphics.hpp>
#include <string>
#include "ScreenSize.h"
#include "Player.h"
#include "Enemy.h"

/// <summary>
/// @brief Main class for the SFML Playground project.
/// 
/// This will be a single class framework for learning about SFML. 
/// Example usage:
///		Game game;
///		game.run();
/// </summary>

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

	// Font used for all text
	sf::Font m_arialFont;
	// main window
	sf::RenderWindow m_window;

	Player m_player;
	Enemy m_enemies[6];

#ifdef TEST_FPS
	sf::Text x_updateFPS;					// text used to display updates per second.
	sf::Text x_drawFPS;						// text used to display draw calls per second.
	sf::Time x_secondTime {sf::Time::Zero};	// counter used to establish when a second has passed.
	int x_updateFrameCount{ 0 };			// updates per second counter.
	int x_drawFrameCount{ 0 };				// draws per second counter.
#endif // TEST_FPS

};
