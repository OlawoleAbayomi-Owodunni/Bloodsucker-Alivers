#pragma once

#include <SFML/Graphics.hpp>
#include "ScreenSize.h"

class Player
{
public:
	Player();
	~Player();

	void update(double dt);
	void render(sf::RenderWindow& t_window);
	void handleKeyInput();
	
	void setPosition(float t_x, float t_y);
	sf::Vector2f getPosition();

	void setHealth();


private:
	float m_health;
	float m_speed;
	sf::Vector2f m_position;

	sf::RectangleShape m_sprite;
	sf::RectangleShape m_emptyHealthBar;
	sf::RectangleShape m_currentHealthBar;
};