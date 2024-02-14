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

	void increaseXP();
	void checkXP();

	sf::RectangleShape getRectangle();
	Weapon getWeapon();

private:
	float m_health;
	float m_speed;
	sf::Vector2f m_position;
	int m_level;
	int m_xp;
	float m_xpRequired;

	sf::RectangleShape m_rectangle;
	sf::RectangleShape m_emptyHealthBar;
	sf::RectangleShape m_currentHealthBar;
	sf::RectangleShape m_xpBar;

	Weapon m_weapon;

	Texture m_playerTexture;
	Sprite m_playerSprite;
};