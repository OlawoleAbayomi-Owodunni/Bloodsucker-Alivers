#pragma once

#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"
#include "Weapon.h"

enum class PlayerUpgrades
{
	Health,
	Speed,
	XP,
	Armor
};

class Player
{
public:
	Player();
	~Player();

	void update(double dt, std::vector<Enemy*> t_enemies);
	void render(sf::RenderWindow& t_window);
	void handleKeyInput();
	void rumbleStart();
	void rumbleStop();
	
	void setPosition(float t_x, float t_y);
	sf::Vector2f getPosition();

	void setHealth();
	void decreaseHealth();
	void increaseHealth();

	void increaseXP();
	void checkXP();
	int getLevel();
	void levelUp(bool& t_menu);
	void upgradePlayer(PlayerUpgrades t_type);

	sf::RectangleShape getRectangle();
	std::vector<Weapon*> getWeapon();

private:
	float m_maxHealth;
	float m_health;
	float m_speed;
	sf::Vector2f m_position;
	
	int m_level;
	float m_xp;
	float m_xpRequired;

	float m_speedModifier;
	float m_xpModifier;
	float m_armorModifier;

	Direction m_direction;

	sf::RectangleShape m_rectangle;
	sf::RectangleShape m_emptyHealthBar;
	sf::RectangleShape m_currentHealthBar;
	sf::RectangleShape m_xpBar;
	sf::RectangleShape m_emptyxphBar;

	std::vector<Weapon*> m_weapons;

	thor::ResourceHolder<sf::Texture, std::string> m_holder;
	
	Sprite m_playerSprite;
	Sprite m_levelBarSprite;
};