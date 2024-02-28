#pragma once
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include <string>

using namespace sf;
using namespace std;
using namespace thor;

//maybe an enum for button types
enum ButtonType
{
	//Main menu buttons
	Play,
	Exit,
	Tutorial,
	Credits,
	HighScore,

	//Pause menu buttons
	Resume,
	ToMenu,

	//Game over menu buttons
	NewGame, //new game will eventually be moved to main menu section
	BackToMenu, //eventually to be removed cuz it exists in pause menu

	//UPGRADE BUTTONS
	UpgradeHealth,
	UpgradeSpeed,
	UpgradeXP,
	UpgradeArmor,
	UpgradePistol,
	UpgradeAR
};
//I'm thinking we make every button in the constructor based off the button type passed. A lot of enums but automation assured
//either check if we can do an enum within an enum or have a different class of buttons that handle upgrades

class Button
{
public:
	Button(ButtonType t_type, Texture& t_holder, Font& t_font, Vector2f t_pos); //pass in texture, font, 

	void render(sf::RenderWindow& t_window);

	void setPosition(Vector2f t_pos);

	Vector2f getPositon();
	ButtonType getType();

private:
	Sprite m_bgSprite;
	Text m_text;

	Vector2f m_pos;
	ButtonType m_type;
};

