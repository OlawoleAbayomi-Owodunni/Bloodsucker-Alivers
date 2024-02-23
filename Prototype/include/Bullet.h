#pragma once
#include <SFML/Graphics.hpp>
#include "Enemy.h"

enum class WeaponType
{
	Pistol,
	AssaultRifle
};

enum class Direction
{
	North,
	West,
	South,
	East
};

class Bullet
{
public:
	Bullet(WeaponType t_weaponType, sf::Texture& t_texture);
	~Bullet();

	void update(double dt, bool t_firing, sf::Vector2f t_playerPos, std::vector<Enemy*> t_enemies, WeaponType t_type, Direction t_direction);
	void render(sf::RenderWindow& t_window);

	sf::CircleShape getCircle();
	float getDamage();

private:
	sf::CircleShape m_circle;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	float m_damage;

	Sprite m_bulletSprite;
};