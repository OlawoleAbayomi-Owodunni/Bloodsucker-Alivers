#include "Weapon.h"
#include <iostream>

Weapon::Weapon()
{
	m_type = Type::Pistol;
	m_firing = false;
}

Weapon::~Weapon()
{
}

void Weapon::update(double dt, sf::Vector2f t_playerPos, Enemy t_enemies[])
{	
	if (!m_firing)
	{
		if (m_timer.getElapsedTime().asSeconds() > 1)
		{
			m_firing = true;
			m_timer.restart();
		}
	}
	else if (m_firing)
	{
		if (m_timer.getElapsedTime().asSeconds() > 3)
		{
			m_firing = false;
			m_timer.restart();
		}
	}

	std::cout << m_timer.getElapsedTime().asSeconds() << std::endl;

	m_bullet.update(dt, m_firing, t_playerPos, t_enemies);
}

void Weapon::render(sf::RenderWindow& t_window)
{
	m_bullet.render(t_window);
}

Bullet Weapon::getBullet()
{
	return m_bullet;
}
