#include "Weapon.h"
#include <iostream>

Weapon::Weapon(WeaponType t_type)
{
	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));
	m_holder.acquire("mapSprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/Map.png"));

	m_type = t_type;
	m_firing = false;
	counter = 0;
	m_fireRateModifier = 1;

	m_weaponSprite.setTexture(m_weaponTexture);
	m_weaponSprite.setOrigin(16, 16);
	m_weaponSprite.setScale(5.0f, 5.0f);

	switch (m_type)
	{
	case WeaponType::Pistol:
		m_fireRate = 3.0f * m_fireRateModifier;
		m_weaponSprite.setTextureRect(IntRect{ 0,0,128,32 });
		break;
	case WeaponType::AssaultRifle:
		m_fireRate = 1.0f / m_fireRateModifier;
		m_weaponSprite.setTextureRect(IntRect{ 0,32,128,32 });
		break;
	default:
		break;
	}

	m_weaponLvl = 1;
}

Weapon::~Weapon()
{
}

void Weapon::update(double dt, sf::Vector2f t_playerPos, std::vector<Enemy*> t_enemies, Direction t_direction)
{	
	switch (m_type)
	{
	case WeaponType::Pistol:
		if (!m_firing)
		{
			if (m_timer.getElapsedTime().asSeconds() > (m_fireRate / m_fireRateModifier))
			{
				m_firing = true;
			}
		}
		else if (m_firing)
		{
			m_firing = false;
			m_timer.restart();

			m_bullets.push_back(new Bullet(m_type, m_holder["starterAtlas"], t_playerPos, t_enemies, t_direction));
		}
		break;
	case WeaponType::AssaultRifle:
		if (!m_firing)
		{
			if (m_timer.getElapsedTime().asSeconds() > (m_fireRate / m_fireRateModifier))
			{
				m_firing = true;
			}
		}
		else if (m_firing)
		{
				m_firing = false;
				m_timer.restart();
		}
		break;
	default:
		break;
	}
	
	//std::cout << m_timer.getElapsedTime().asSeconds() << std::endl;

	for (auto it = m_bullets.begin(); it != m_bullets.end();)
	{
		(*it)->update(dt, m_type);

		if ((*it)->getPosition().x < 0 || (*it)->getPosition().x > 3200 ||
			(*it)->getPosition().y < 0 || (*it)->getPosition().y > 1800)
		{
			delete* it; // Delete the orb object
			it = m_bullets.erase(it); // Remove the orb pointer from the vector
		}
		else
		{
			++it;
		}
	}
}

void Weapon::render(sf::RenderWindow& t_window)
{
	for (auto bullet : m_bullets)
	{
		bullet->render(t_window);
	}
}

std::vector<Bullet*> Weapon::getBullets()
{
	return m_bullets;
}

WeaponType Weapon::getType()
{
	return m_type;
}

int Weapon::getWeaponLevel()
{
	return m_weaponLvl;
}

void Weapon::upgradeWeapon() //probably pass in which weapon ID is coming from the player and based off that modify that guns properties in here
{
	m_weaponLvl++;
	switch (m_weaponLvl)
	{
	case 1:
		m_fireRateModifier = 2.0f;
		break;
	case 2:
		m_fireRateModifier = 2.5f;
		break;
	case 3:
		m_fireRateModifier = 3.0f;
		break;
	default:
		break;
	}
}
