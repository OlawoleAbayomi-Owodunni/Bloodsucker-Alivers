#pragma once

#include <SFML/Graphics.hpp>
#include "ScreenSize.h"
#include "Weapon.h"

class Player
{
public:
	Player();
	~Player();

	void update(double dt, Enemy t_enemies[]);
	void render(sf::RenderWindow& t_window);
	void handleKeyInput();
	
	void setPosition(float t_x, float t_y);
	sf::Vector2f getPosition();

	void setHealth();
	void decreaseHealth();

	sf::RectangleShape getRectangle();

private:
	float m_health;
	float m_speed;
	sf::Vector2f m_position;

	sf::RectangleShape m_rectangle;
	sf::RectangleShape m_emptyHealthBar;
	sf::RectangleShape m_currentHealthBar;

	Weapon m_weapon;
};