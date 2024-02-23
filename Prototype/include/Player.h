#pragma once

#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"
#include "Weapon.h"

enum class Gamemode;

enum class CharacterState
{
	None,
	IdleState,
	WalkState
};

enum class PlayerUpgrade
{
	Health,
	Speed,
	XP,
	Armor,

	Count
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
	void levelUp(Gamemode& t_gamemode);
	void upgradePlayer(PlayerUpgrade t_type);

	void animate();
	void addFrame(sf::IntRect& t_frame);
	void setFrames();

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
	sf::RectangleShape m_emptyXPBar;

	std::vector<Weapon*> m_weapons;

	thor::ResourceHolder<sf::Texture, std::string> m_holder;

	CharacterState m_playerState;
	CharacterState m_previousState;

	std::vector<sf::IntRect> m_frames;
	int m_currentFrame;
	Time m_time;
	Clock m_clock;
	
	Sprite m_sprite;
	Sprite m_xpBarSprite;
};