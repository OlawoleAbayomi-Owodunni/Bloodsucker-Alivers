#include "Bullet.h"
#include <iostream>

Bullet::Bullet(WeaponType t_weaponType, sf::Texture& t_texture)
{
	m_velocity = sf::Vector2f(0.0f, 0.0f);
	m_position = sf::Vector2f(0.0f, 0.0f);

	m_circle.setRadius(5.0f);
	m_circle.setOrigin(m_circle.getRadius(), m_circle.getRadius());
	m_circle.setFillColor(sf::Color::Blue);
	m_circle.setPosition(m_position);

	m_sprite.setTexture(t_texture);
	m_sprite.setTextureRect(IntRect{ 0,648,32,32 });
	m_sprite.setOrigin(16, 16);
	m_sprite.setScale(1.0f, 1.0f);
	m_sprite.setPosition(m_position);

	switch (t_weaponType)
	{
	case WeaponType::Pistol:
		m_damage = 32.0f;	// default 12
		break;
	case WeaponType::AssaultRifle:
		m_damage = 24.0f;	// default 24
		break;
	default:
		break;
	}

	for (int i = 0; i < 16; i++)
	{
		m_frames.push_back(IntRect{ 32 * i,648,32,32 });
	}
	m_currentFrame = 0;
	m_time = seconds(0.1f);
}

Bullet::~Bullet()
{
}

void Bullet::update(double dt, bool t_firing, sf::Vector2f t_playerPos, std::vector<Enemy*> t_enemies, WeaponType t_type, Direction t_direction)
{
	switch (t_type)
	{
	case WeaponType::Pistol:
		if (!t_firing)
		{
			m_position = t_playerPos;

			float distance;
			float shortestDistance;
			sf::Vector2f displacement;
			sf::Vector2f shortestDisplacement;

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
			m_velocity = shortestDisplacement * 10.0f;

		}
		else if (t_firing)
		{
			m_position += m_velocity;
		}
		break;
	case WeaponType::AssaultRifle:
		if (!t_firing)
		{
			m_position = t_playerPos;

			switch (t_direction)
			{
			case Direction::North:
				m_velocity = sf::Vector2f(0.0f,-10.0f);
				break;
			case Direction::West:
				m_velocity = sf::Vector2f(-10.0f, 0.0f);
				break;
			case Direction::South:
				m_velocity = sf::Vector2f(0.0f, 10.0f);
				break;
			case Direction::East:
				m_velocity = sf::Vector2f(10.0f, 0.0f);
				break;
			default:
				break;
			}
		}
		else if (t_firing)
		{
			m_position += m_velocity;
		}
		break;
	default:
		break;
	}

	animate();

	m_circle.setPosition(m_position);
	m_sprite.setPosition(m_position);
}

void Bullet::render(sf::RenderWindow& t_window)
{
	//t_window.draw(m_circle);
	t_window.draw(m_sprite);
}

sf::CircleShape Bullet::getCircle()
{
	return m_circle;
}

float Bullet::getDamage()
{
	return m_damage;
}

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