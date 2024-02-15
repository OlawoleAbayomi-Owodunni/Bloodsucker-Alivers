#include "Weapon.h"
#include <iostream>

Weapon::Weapon(WeaponType t_type)
{
	m_type = t_type;
	m_firing = false;
	counter = 0;
	
	switch (m_type)
	{
	case WeaponType::Pistol:
		m_fireRate = 3.0f;
		m_bullets.push_back(new Bullet);
		break;
	case WeaponType::AssaultRifle:
		m_fireRate = 1.0f;
		m_bullets.push_back(new Bullet);
		m_bullets.push_back(new Bullet);
		m_bullets.push_back(new Bullet);
		break;
	default:
		break;
	}
}

Weapon::~Weapon()
{
}

void Weapon::update(double dt, sf::Vector2f t_playerPos, Enemy t_enemies[], Direction t_direction)
{	
	if (!m_firing)
	{
		if (m_timer.getElapsedTime().asSeconds() > m_fireRate)
		{
			m_firing = true;
			m_timer.restart();
		}
	}
	else if (m_firing)
	{
		if (m_timer.getElapsedTime().asSeconds() > 2)
		{
			m_firing = false;
			m_timer.restart();
		}
	}

	//std::cout << m_timer.getElapsedTime().asSeconds() << std::endl;

	switch (m_type)
	{
	case WeaponType::Pistol:
		for (auto bullet : m_bullets)
		{
			bullet->update(dt, m_firing, t_playerPos, t_enemies, m_type, t_direction);
		}
		break;
	case WeaponType::AssaultRifle:
		/*if (counter > 0)
		{
			m_bullets.at(0)->update(dt, m_firing, t_playerPos, t_enemies, m_type, t_direction);
		}
		if (counter > 100)
		{
			m_bullets.at(1)->update(dt, m_firing, t_playerPos, t_enemies, m_type, t_direction);
		}
		if (counter > 200)
		{
			m_bullets.at(2)->update(dt, m_firing, t_playerPos, t_enemies, m_type, t_direction);
		}
		counter++;*/

		//std::cout << std::endl;

		for (auto bullet : m_bullets)
		{
			bullet->update(dt, m_firing, t_playerPos, t_enemies, m_type, t_direction);
		}
		break;
	default:
		break;
	}

	

}

void Weapon::render(sf::RenderWindow& t_window)
{
	for (auto bullet : m_bullets)
	{
		bullet->render(t_window);
	}
}

std::vector<Bullet*> Weapon::getBullet()
{
	return m_bullets;
}
