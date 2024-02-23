#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"

using namespace sf;
using namespace std;

class XPOrb
{
public:
	XPOrb(sf::Texture& t_texture, sf::Vector2f t_position);
	~XPOrb();

	void update(double dt);
	void render(sf::RenderWindow& t_window);

	sf::CircleShape getCircle();

private:
	sf::CircleShape m_circle;
	sf::Vector2f m_position;

	sf::Sprite m_orbSprite;
};