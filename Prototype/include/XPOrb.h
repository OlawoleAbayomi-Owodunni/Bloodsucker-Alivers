#pragma once

#include <SFML/Graphics.hpp>
#include "ScreenSize.h"

class XPOrb
{
public:
	XPOrb(sf::Vector2f t_position);
	~XPOrb();

	void update(double dt);
	void render(sf::RenderWindow& t_window);

	sf::CircleShape getCircle();

private:
	sf::CircleShape m_circle;
	sf::Vector2f m_position;
};