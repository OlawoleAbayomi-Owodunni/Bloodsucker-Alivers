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

	if (!m_weaponTexture.loadFromFile("./resources/sprites/plyrPh.png"))
	{
		cout << "Failed to load file\n";
	}

	m_weaponSprite.setTexture(m_weaponTexture);

	m_weaponSprite.setOrigin(16, 16);
	m_weaponSprite.setScale(5.0f, 5.0f);

	switch (m_type)
	{
	case WeaponType::Pistol:
		m_fireRate = 3.0f * m_fireRateModifier;
		m_bullets.push_back(new Bullet(WeaponType::Pistol, m_holder["starterAtlas"]));
		m_weaponSprite.setTextureRect(IntRect{ 0,0,128,32 });
		break;
	case WeaponType::AssaultRifle:
		m_fireRate = 1.0f / m_fireRateModifier;
		m_bullets.push_back(new Bullet(WeaponType::AssaultRifle, m_holder["starterAtlas"]));
		m_bullets.push_back(new Bullet(WeaponType::AssaultRifle, m_holder["starterAtlas"]));
		m_bullets.push_back(new Bullet(WeaponType::AssaultRifle, m_holder["starterAtlas"]));
		m_weaponSprite.setTextureRect(IntRect{ 0,32,128,32 });
		break;
	default:
		break;
	}

	m_weaponLvl = 0;
}

Weapon::~Weapon()
{
}

void Weapon::update(double dt, sf::Vector2f t_playerPos, std::vector<Enemy*> t_enemies, Direction t_direction)
{	
	if (!m_firing)
	{
		if (m_timer.getElapsedTime().asSeconds() > (m_fireRate / m_fireRateModifier))
		{
			m_firing = true;
			m_timer.restart();
		}
	}
	else if (m_firing)
	{
		if (m_timer.getElapsedTime().asSeconds() > (m_fireRate / m_fireRateModifier))
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
		m_fireRateModifier = 3.0f;
		break;
	case 3:
		m_fireRateModifier = 4.0f;
		break;
	default:
		break;
	}
}
