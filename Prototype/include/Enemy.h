#pragma once

#include <SFML/Graphics.hpp>
#include "ScreenSize.h"
#include "Player.h"

class Enemy
{
public:
	Enemy();
	~Enemy();

	void update(double dt, Player& t_player);
	void render(sf::RenderWindow& t_window);

	void move(Player& t_player);

private:
	float m_speed;
	sf::Vector2f m_position;

	sf::RectangleShape m_sprite;
};