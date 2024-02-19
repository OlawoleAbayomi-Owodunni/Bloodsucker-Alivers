#pragma once

#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"
#include "Weapon.h"

class Player
{
public:
	Player();
	~Player();

	void update(double dt, std::vector<Enemy*> t_enemies);
	void render(sf::RenderWindow& t_window);
	void handleKeyInput();
	
	void setPosition(float t_x, float t_y);
	sf::Vector2f getPosition();

	void setHealth();
	void decreaseHealth();

	void increaseXP();
	void checkXP();
	int getLevel();

	sf::RectangleShape getRectangle();
	std::vector<Weapon*> getWeapon();

private:
	float m_health;
	float m_speed;
	sf::Vector2f m_position;
	
	int m_level;
	int m_xp;
	float m_xpRequired;
	bool m_levelUp;

	Direction m_direction;

	sf::RectangleShape m_rectangle;
	sf::RectangleShape m_emptyHealthBar;
	sf::RectangleShape m_currentHealthBar;
	sf::RectangleShape m_xpBar;
	sf::RectangleShape m_emptyxphBar;

	std::vector<Weapon*> m_weapons;

	thor::ResourceHolder<sf::Texture, std::string> m_holder;
	
	Texture m_texture;
	Sprite m_playerSprite;
};