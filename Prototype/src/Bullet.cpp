#include "Bullet.h"
#include <iostream>

Bullet::Bullet()
{
	m_velocity = sf::Vector2f(0.0f, 0.0f);
	m_position = sf::Vector2f(0.0f, 0.0f);

	m_circle.setRadius(5.0f);
	m_circle.setOrigin(m_circle.getRadius(), m_circle.getRadius());
	m_circle.setFillColor(sf::Color::Blue);
	m_circle.setPosition(m_position);
}

Bullet::~Bullet()
{
}

void Bullet::update(double dt, bool t_firing, sf::Vector2f t_playerPos, Enemy t_enemies[], WeaponType t_type, Direction t_direction)
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

			for (int i = 0; i < 6; i++)
			{
				displacement.x = t_enemies[i].getPosition().x - t_playerPos.x;
				displacement.y = t_enemies[i].getPosition().y - t_playerPos.y;

				distance = std::sqrtf(displacement.x * displacement.x + displacement.y * displacement.y);

				if (i == 0)
				{
					shortestDistance = distance;
					shortestDisplacement.x = t_enemies[i].getPosition().x - t_playerPos.x;
					shortestDisplacement.y = t_enemies[i].getPosition().y - t_playerPos.y;
				}

				if (distance < shortestDistance)
				{
					shortestDistance = distance;
					shortestDisplacement.x = t_enemies[i].getPosition().x - t_playerPos.x;
					shortestDisplacement.y = t_enemies[i].getPosition().y - t_playerPos.y;
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
}

void Bullet::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_circle);
}

sf::CircleShape Bullet::getCircle()
{
	return m_circle;
}