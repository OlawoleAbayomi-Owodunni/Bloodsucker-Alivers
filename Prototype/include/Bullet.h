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
	Bullet(WeaponType t_weaponType, sf::Texture& t_texture, sf::Vector2f t_playerPos, std::vector<Enemy*> t_enemies, Direction t_direction);
	~Bullet();

	void update(double dt, WeaponType t_type);
	void render(sf::RenderWindow& t_window);

	sf::CircleShape getCircle();
	float getDamage();
	sf::Vector2f getPosition();

	void animate();

private:
	sf::CircleShape m_circle;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	float m_damage;

	vector<sf::IntRect> m_frames;
	int m_currentFrame;
	Time m_time;
	Clock m_clock;

	Sprite m_sprite;

	float pistolSpeed;
	float arSpeed;
};