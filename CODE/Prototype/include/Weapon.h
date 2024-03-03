#pragma once
#include <SFML/Graphics.hpp>
#include "Bullet.h"

struct Explosion
{
	sf::CircleShape radius;
	sf::Sprite sprite;
	sf::Vector2f position;

	std::vector<sf::IntRect> frames;
	int currentFrame;
	Time time;
	Clock clock;

	bool animationOver;
};

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

	void animateExplosion();
	Explosion& getExplosion();

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

	Explosion m_explosion;
	float m_dmgRadius;
	float m_dmgRadiusModifier;

	SoundBuffer m_shootingSoundBuffer;
	Sound m_shootingSound;

	Texture m_starterAtlas;
	Sprite m_weaponSprite;

	int m_weaponLvl;

	bool isGunEquipped;
	bool isFullyUpgraded;
};