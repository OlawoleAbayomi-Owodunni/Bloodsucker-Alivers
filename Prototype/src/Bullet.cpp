#include "Bullet.h"
#include <iostream>

Bullet::Bullet(sf::Texture& t_texture)
{
	m_velocity = sf::Vector2f(0.0f, 0.0f);
	m_position = sf::Vector2f(0.0f, 0.0f);

	m_circle.setRadius(5.0f);
	m_circle.setOrigin(m_circle.getRadius(), m_circle.getRadius());
	m_circle.setFillColor(sf::Color::Blue);
	m_circle.setPosition(m_position);

	m_bulletSprite.setTexture(t_texture);
	m_bulletSprite.setTextureRect(IntRect{ 0,64,64,64 });
	m_bulletSprite.setOrigin(32, 32);
	m_bulletSprite.setScale(0.5f, 0.5f);
	m_bulletSprite.setPosition(m_position);
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

	m_circle.setPosition(m_position);
	m_bulletSprite.setPosition(m_position);
}

void Bullet::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_circle);
	t_window.draw(m_bulletSprite);
}

sf::CircleShape Bullet::getCircle()
{
	return m_circle;
}