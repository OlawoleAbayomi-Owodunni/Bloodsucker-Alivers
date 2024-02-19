#pragma once

#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"

using namespace std;
using namespace sf;

class Player;

class Enemy
{
public:
	Enemy(sf::Texture& t_texture);
	~Enemy();

	void update(double dt, Player& t_player);
	void render(sf::RenderWindow& t_window);

	void initialisePosition();
	void move(Player& t_player);
	void setPosition(float t_x, float t_y);
	sf::Vector2f getPosition();

	void decreaseHealth(float t_damage);
	float getHealth();

	sf::RectangleShape getRectangle();

private:
	float m_health;
	float m_speed;
	sf::Vector2f m_position;

	sf::RectangleShape m_rectangle;
	sf::RectangleShape m_emptyHealthBar;
	sf::RectangleShape m_currentHealthBar;

	thor::ResourceHolder<sf::Texture, std::string> m_holder;
	
	Sprite m_enemySprite;
};