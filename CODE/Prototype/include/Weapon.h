#pragma once
#include <SFML/Graphics.hpp>
#include "Bullet.h"

class Weapon
{
public:
	Weapon(WeaponType t_type, sf::Texture& t_texture);
	~Weapon();

	void update(double dt, sf::Vector2f t_playerPos, std::vector<Enemy*> t_enemies, Direction t_direction);
	void render(sf::RenderWindow& t_window);

	std::vector<Bullet*>& getBullets();
	WeaponType getType();
	int getWeaponLevel();

	void equipWeapon();
	bool isWeaponEquipped();
	void upgradeWeapon(WeaponType t_type);


private:
	sf::Clock m_timer;
	sf::Clock m_arTimer;
	sf::Time m_arCooldown;

	WeaponType m_type;
	std::vector<Bullet*> m_bullets;
	bool m_firing;
	float m_fireRate;
	float m_fireRateModifier;
	int m_arBulletCounter;
	int m_maxArBullets;

	Texture m_starterAtlas;
	Sprite m_weaponSprite;

	int m_weaponLvl;

	bool isGunEquipped;
};