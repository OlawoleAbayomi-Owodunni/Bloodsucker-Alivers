#include "Bullet.h"
#include <iostream>

Bullet::Bullet(WeaponType t_weaponType, sf::Texture& t_texture, sf::Vector2f t_playerPos, std::vector<Enemy*> t_enemies, Direction t_direction)
{
	//std::cout << "Bullet created.\n";

	m_velocity = sf::Vector2f(0.0f, 0.0f);
	m_position = sf::Vector2f(0.0f, 0.0f);


#pragma region SPRITE AND SHAPE SETUP (PLUS FRAME INSTANTIATOR)
	m_circle.setRadius(5.0f);
	m_circle.setOrigin(m_circle.getRadius(), m_circle.getRadius());
	m_circle.setFillColor(sf::Color::Blue);
	m_circle.setPosition(m_position);

	m_sprite.setTexture(t_texture);
	m_sprite.setTextureRect(IntRect{ 0,658,32,32 });
	m_sprite.setOrigin(16, 16);
	m_sprite.setScale(1.0f, 1.0f);
	m_sprite.setPosition(m_position);

	for (int i = 0; i < 16; i++)
	{
		m_frames.push_back(IntRect{ 32 * i,658,32,32 });
	}
	m_currentFrame = 0;
	m_time = seconds(0.1f);
#pragma endregion

#pragma region GUN TYPE INITIALISER (CALCULATIONS DONE HERE)
	pistolSpeed = 10.0f;
	arSpeed = 7.5f;

	float distance;
	float shortestDistance;
	sf::Vector2f displacement;
	sf::Vector2f shortestDisplacement;

	switch (t_weaponType)
	{
#pragma region Pistol
	case WeaponType::Pistol:
		m_damage = 32.0f;	// default 12
		
		m_position = t_playerPos;

		for (auto enemy : t_enemies)
		{
			displacement.x = enemy->getPosition().x - t_playerPos.x;
			displacement.y = enemy->getPosition().y - t_playerPos.y;

			distance = std::sqrtf(displacement.x * displacement.x + displacement.y * displacement.y);

			if (enemy == t_enemies.at(0))
			{
				shortestDistance = distance;
				shortestDisplacement.x = enemy->getPosition().x - t_playerPos.x;
				shortestDisplacement.y = enemy->getPosition().y - t_playerPos.y;
			}

			if (distance < shortestDistance)
			{
				shortestDistance = distance;
				shortestDisplacement.x = enemy->getPosition().x - t_playerPos.x;
				shortestDisplacement.y = enemy->getPosition().y - t_playerPos.y;
			}
		}

		shortestDisplacement = shortestDisplacement / shortestDistance;
		m_velocity = shortestDisplacement * pistolSpeed;
		break;

#pragma endregion

#pragma region Assault Rifle
	case WeaponType::AssaultRifle:
		m_damage = 24.0f;	// default 24

		m_position = t_playerPos;

		switch (t_direction)
		{
		case Direction::North:
			m_velocity = sf::Vector2f(0.0f, -arSpeed);
			break;
		case Direction::West:
			m_velocity = sf::Vector2f(-arSpeed, 0.0f);
			break;
		case Direction::South:
			m_velocity = sf::Vector2f(0.0f, arSpeed);
			break;
		case Direction::East:
			m_velocity = sf::Vector2f(arSpeed, 0.0f);
			break;
		default:
			break;
		}
		break;

#pragma endregion

	default:
		break;
	}

#pragma endregion
}

Bullet::~Bullet()
{
	//std::cout << "Bullet destroyed.\n";
}

void Bullet::update(double dt, WeaponType t_type)
{
#pragma region BULLET MOVEMENT
	switch (t_type)
	{
#pragma region Pistol

	case WeaponType::Pistol:
		m_position += m_velocity;
		break;

#pragma endregion

#pragma region Assault Rifle

	case WeaponType::AssaultRifle:
		m_position += m_velocity;
		break;

#pragma endregion

	default:
		break;
	}
#pragma endregion

	animate();

	m_circle.setPosition(m_position);
	m_sprite.setPosition(m_position);
}

void Bullet::render(sf::RenderWindow& t_window)
{
	//t_window.draw(m_circle);
	t_window.draw(m_sprite);
}

#pragma region GETTERS AND SETTERS
#pragma region Getters
sf::CircleShape Bullet::getCircle()
{
	return m_circle;
}

float Bullet::getDamage()
{
	return m_damage;
}

sf::Vector2f Bullet::getPosition()
{
	return m_position;
}

#pragma endregion

#pragma endregion

#pragma region ANIMATIONS
void Bullet::animate()
{
	if (m_clock.getElapsedTime() > m_time)
	{
		if (m_currentFrame + 1 < m_frames.size())
		{
			m_currentFrame++;
		}
		else
		{
			m_currentFrame = 0;
		}
		m_clock.restart();
	}

	m_sprite.setTextureRect(m_frames[m_currentFrame]);
}
#pragma endregion

