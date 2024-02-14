#pragma once

#include <SFML/Graphics.hpp>
#include "ScreenSize.h"

using namespace std;
using namespace sf;

class Player;

class Enemy
{
public:
	Enemy();
	~Enemy();

	void update(double dt, Player& t_player);
	void render(sf::RenderWindow& t_window);

	void initialisePosition();
	void move(Player& t_player);
	void setPosition(float t_x, float t_y);
	sf::Vector2f getPosition();

	sf::RectangleShape getRectangle();

private:
	float m_speed;
	sf::Vector2f m_position;

	sf::RectangleShape m_rectangle;

	Texture m_enemyTexture;
	Sprite m_enemySprite;
};