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
	Menu,
	Upgrade
};
//I'm thinking we make every button in the constructor based off the button type passed. A lot of enums but automation assured
//either check if we can do an enum within an enum or have a different class of buttons that handle upgrades

class Button
{
public:
	Button(ButtonType t_type, ResourceHolder<Texture, string>& t_holder, Font& t_font, Vector2f t_pos); //pass in texture, font, 

private:
	Sprite m_bgSprite;
	Text m_Text;
};

