#include "Weapon.h"
#include <iostream>

Weapon::Weapon(WeaponType t_type, sf::Texture& t_texture)
{

	m_type = t_type;
	m_firing = false;
	m_fireRateModifier = 1;
	m_dmgRadiusModifier = 1;
	m_arCooldown = seconds(0.15);
	m_arBulletCounter = 0;
	m_maxArBullets = 3;
	isFullyUpgraded = false;


	m_starterAtlas = t_texture;

	m_weaponSprite.setTexture(m_starterAtlas);

	switch (m_type)
	{
#pragma region Pistol
	case WeaponType::Pistol:
		m_fireRate = 2.0f / m_fireRateModifier;
		m_weaponSprite.setTextureRect(IntRect{ 0,0,128,32 });

		if (!m_shootingSoundBuffer.loadFromFile("resources/sounds/pistol_shoot.wav"))
		{
			std::cout << "error loading pistl shooting sound";
		}
		m_shootingSound.setBuffer(m_shootingSoundBuffer);
		m_shootingSound.setVolume(2.0f);
		break;

#pragma endregion

#pragma region AR
	case WeaponType::AssaultRifle:
		m_fireRate = 3.0f / m_fireRateModifier;
		m_weaponSprite.setTextureRect(IntRect{ 0,32,128,32 });

		if (!m_shootingSoundBuffer.loadFromFile("resources/sounds/ar_shoot.wav"))
		{
			std::cout << "error loading ar shooting sound";
		}
		m_shootingSound.setBuffer(m_shootingSoundBuffer);
		m_shootingSound.setVolume(1.5f);
		break;

#pragma endregion

#pragma region Sniper
	case WeaponType::Sniper:
		m_fireRate = 4.0f / m_fireRateModifier;
		m_weaponSprite.setTextureRect(IntRect{ 0,32,128,32 }); // needs to change when sniper sprite is added

		if (!m_shootingSoundBuffer.loadFromFile("resources/sounds/sniper_shoot.wav"))
		{
			std::cout << "error loading sniper shooting sound";
		}
		m_shootingSound.setBuffer(m_shootingSoundBuffer);
		m_shootingSound.setVolume(5.0f);
		break;

#pragma endregion

#pragma region RPG
	case WeaponType::RPG:
		m_fireRate = 6.0f / m_fireRateModifier;
		m_dmgRadius = 50.0f * m_dmgRadiusModifier;
		m_weaponSprite.setTextureRect(IntRect{ 0,32,128,32 }); // needs to change when rpg sprite is added

		if (!m_shootingSoundBuffer.loadFromFile("resources/sounds/rpg_shoot.wav"))
		{
			std::cout << "error loading rpg shooting sound";
		}
		m_shootingSound.setBuffer(m_shootingSoundBuffer);
		m_shootingSound.setVolume(10.0f);

		m_explosion.position = sf::Vector2f(-1000.0f, -1000.0f);
		
		m_explosion.radius.setRadius(m_dmgRadius);
		m_explosion.radius.setOrigin(m_explosion.radius.getRadius(), m_explosion.radius.getRadius());
		m_explosion.radius.setFillColor(sf::Color::White);
		m_explosion.radius.setPosition(m_explosion.position);

		m_explosion.sprite.setTexture(m_starterAtlas);
		m_explosion.sprite.setTextureRect(IntRect{ 0, 2864, 500, 500 });
		m_explosion.sprite.setOrigin(250.0f, 250.0f);
		m_explosion.sprite.setScale(((0.5f * m_dmgRadiusModifier) * 0.75), ((0.5f * m_dmgRadiusModifier) * 0.75));
		m_explosion.sprite.setPosition(m_explosion.position);

		for (int i = 0; i < 10; i++)
		{
			m_explosion.frames.push_back(IntRect{ 500 * i, 2864, 500, 500 });
		}

		m_explosion.currentFrame = 0;
		m_explosion.time = seconds(0.15f);

		m_explosion.animationOver = false;

		break;

#pragma endregion

	default:
		break;
	}

	m_weaponSprite.setOrigin(64, 16);
	m_weaponSprite.setScale(5.0f, 5.0f);
	//CHECK SNIPER AND RPG AS THEY ARE DIFFERENT

	//WE WILL BE DOING ANIMATIONS LOCALLY IN THIS CLASS THEN RENDERING IN GAME.CPP USING GETWEAPON()[var that checked cursorType against gunType]

	m_weaponLvl = 1;
}

Weapon::~Weapon()
{
}

void Weapon::update(double dt, sf::Vector2f t_playerPos, std::vector<Enemy*> t_enemies, Direction t_direction)
{
	switch (m_type)
	{
#pragma region Pistol
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

			m_bullets.push_back(new Bullet(m_type, m_starterAtlas, t_playerPos, t_enemies, t_direction));
			
			m_shootingSound.stop();
			m_shootingSound.play();
		}
		break;

#pragma endregion

#pragma region Assault Rifle
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
			if (m_arTimer.getElapsedTime() > m_arCooldown)
			{
				m_arTimer.restart();
				m_bullets.push_back(new Bullet(m_type, m_starterAtlas, t_playerPos, t_enemies, t_direction));
				m_arBulletCounter++;
				
				m_shootingSound.stop();
				m_shootingSound.play();
			}

			if (m_arBulletCounter == m_maxArBullets)
			{
				m_arBulletCounter = 0;
				m_firing = false;
				m_timer.restart();
			}
		}
		break;

#pragma endregion

#pragma region Sniper
	case WeaponType::Sniper:
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

			m_bullets.push_back(new Bullet(m_type, m_starterAtlas, t_playerPos, t_enemies, t_direction));
			
			m_shootingSound.stop();
			m_shootingSound.play();
		}
		break;

#pragma endregion

#pragma region RPG
	case WeaponType::RPG:
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

			m_bullets.push_back(new Bullet(m_type, m_starterAtlas, t_playerPos, t_enemies, t_direction));
			m_shootingSound.stop();
			m_shootingSound.play();
		}

		if (m_explosion.currentFrame > 2)
		{
			m_explosion.position = sf::Vector2f(-1000.0f, -1000.0f);
			m_explosion.radius.setPosition(m_explosion.position);
			//m_explosion.sprite.setPosition(m_explosion.position);
		}

		animateExplosion();
		break;

#pragma endregion

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
			delete* it; // Delete the bullet object
			it = m_bullets.erase(it); // Remove the bullet pointer from the vector
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

	if (m_type == WeaponType::RPG)
	{
		//t_window.draw(m_explosion.radius);
		if (!m_explosion.animationOver)
		{
			t_window.draw(m_explosion.sprite);
		}
	}
}

#pragma region Getters
std::vector<Bullet*>& Weapon::getBullets()
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

bool Weapon::isWeaponEquipped()
{
	return isGunEquipped;
}

#pragma endregion


void Weapon::equipWeapon()
{
	isGunEquipped = true;
}


void Weapon::upgradeWeapon(WeaponType t_type) //probably pass in which weapon ID is coming from the player and based off that modify that guns properties in here
{
	if (m_weaponLvl < 5)
		m_weaponLvl++;
	else
		isFullyUpgraded = true;

	if (!isFullyUpgraded) {
		switch (t_type)
		{
		case WeaponType::Pistol:
			switch (m_weaponLvl)
			{
				//only have direct access to fire rate
			case 1:
				m_fireRateModifier = 1.0f;
				break;
			case 2:
				m_fireRateModifier = 1.5f;
				break;
			case 3:
				m_fireRateModifier = 2.0f;
				break;
			case 4:
				m_fireRateModifier = 2.5f;
				break;
			case 5:
				m_fireRateModifier = 3.0f;
				break;
			default:
				break;
			}
			break;
		case WeaponType::AssaultRifle:
			switch (m_weaponLvl)
			{
				//have access to fire rate and burst number
			case 1:
				m_fireRateModifier = 1.0f;
				break;
			case 2:
				m_maxArBullets++;
				break;
			case 3:
				m_fireRateModifier = 2.0f;
				break;
			case 4:
				m_maxArBullets++;
				break;
			case 5:
				m_fireRateModifier = 2.5f;
				m_maxArBullets += 3;
				break;
			default:
				break;
			}
			break;
		case WeaponType::Sniper:
			switch (m_weaponLvl)
			{
		case 1:
			m_fireRateModifier = 1.0f;
			break;
		case 2:
			m_fireRateModifier = 1.5f;
			break;
		case 3:
			m_fireRateModifier = 2.0f;
			break;
		case 4:
			m_fireRateModifier = 2.5f;
			break;
		case 5:
			m_fireRateModifier = 3.0f;
		default:
			break;
			}
			break;
		case WeaponType::RPG:
			switch (m_weaponLvl)
			{	
			case 1: //aoe
				m_dmgRadiusModifier = 1.0f;
				break;
			case 2: //fire rate
				m_fireRateModifier = 1.15f;
				break;
			case 3: //aoe
				m_dmgRadiusModifier = 1.5f;
				break;
			case 4: //fire rate + aoe
				m_fireRateModifier = 1.25f;
				m_dmgRadiusModifier = 2.0f;
				break;
			case 5: //aoe + fire rate
				m_dmgRadiusModifier = 3.0f;
				m_fireRateModifier = 1.5f;
				break;
			default:
				break;
			}
			break;
		}
		//switch statement here based off the weapon (E.G: more bullets in AR)

	}

}

void Weapon::animateExplosion()
{
	if (m_explosion.clock.getElapsedTime() > m_explosion.time)
	{
		if (m_explosion.currentFrame + 1 < m_explosion.frames.size())
		{
			m_explosion.currentFrame++;
		}
		else
		{
			m_explosion.animationOver = true;
		}
		m_explosion.clock.restart();
	}

	m_explosion.sprite.setTextureRect(m_explosion.frames[m_explosion.currentFrame]);
}

Explosion& Weapon::getExplosion()
{
	return m_explosion;
}
