#pragma once
#include <SFML/Graphics.hpp>
#include "Bullet.h"

enum class Type
{
	Pistol
};

class Weapon
{
public:
	Weapon();
	~Weapon();

	void update(double dt, sf::Vector2f t_playerPos, Enemy t_enemies[]);
	void render(sf::RenderWindow& t_window);

	Bullet getBullet();

private:
	sf::Clock m_timer;
	Type m_type;
	Bullet m_bullet;
	bool m_firing;
};