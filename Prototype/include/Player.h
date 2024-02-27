#pragma once

#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"
#include "Weapon.h"

enum class Gamemode;

enum class CharacterState
{
	None,
	IdleState,
	WalkSideState,
	WalkDownState,
	WalkUpState
};

enum class PlayerUpgrade
{
	Health,
	Speed,
	XP,
	Armor,
	Weapon,

	Count
};

class AfterImageData
{
public:
	AfterImageData(sf::Vector2f t_position, float t_lifetime, sf::Sprite t_sprite) :
		m_position(t_position), m_lifetime(t_lifetime), m_sprite(t_sprite)
	{};

	sf::Vector2f m_position;
	float m_lifetime;
	sf::Sprite m_sprite;

};

class Player
{
public:
	Player();
	~Player();

	void update(double dt, sf::View& t_view, std::vector<Enemy*> t_enemies);
	void render(sf::RenderWindow& t_window);
	void handleKeyInput();
	void rumbleStart();
	void rumbleStop();
	
	void setPosition(sf::View& t_view);
	sf::Vector2f getPosition();

	void setHealth();
	void decreaseHealth();
	void increaseHealth();

	void increaseXP();
	void checkXP();
	int getLevel();
	void levelUp(Gamemode& t_gamemode);
	void upgradePlayer(PlayerUpgrade t_type);
	void carePackage(Gamemode& t_gamemode);
	void giveWeapon(WeaponType t_type);

	void animate();
	void addFrame(sf::IntRect& t_frame);
	void setFrames();

	void playSound(sf::Sound& t_sound);

	sf::RectangleShape getRectangle();
	sf::RectangleShape getDashCollider();
	std::vector<Weapon*> getWeapon();

private:
	const float DASH_DISTANCE{ 150.0f };
	const float AFTERIMAGE_COUNT{ 16.0f };

	std::vector<AfterImageData> m_afterImages{};
	
	float m_maxHealth;
	float m_health;
	float m_speed;
	sf::Vector2f m_position;
	sf::Vector2f m_movementVector;
	
	int m_level;
	float m_xp;
	float m_xpRequired;

	float m_speedModifier;
	float m_xpModifier;
	float m_armorModifier;

	Direction m_direction;

	bool m_canDash;

	sf::RectangleShape m_rectangle;

	sf::RectangleShape m_emptyHealthBar;
	sf::RectangleShape m_currentHealthBar;
	
	sf::RectangleShape m_xpBar;
	sf::RectangleShape m_emptyXPBar;

	sf::RectangleShape m_dashRect;
	sf::RectangleShape m_dashRectBounds;

	std::vector<Weapon*> m_weapons;

	thor::ResourceHolder<sf::Texture, std::string> m_holder;

	CharacterState m_playerState;
	CharacterState m_previousState;

	std::vector<sf::IntRect> m_playerFrames;
	int m_currentPlayerFrame;
	Time m_playerTime;
	Clock m_playerClock;

	std::vector<sf::IntRect> m_haloFrames;
	int m_currentHaloFrame;
	Time m_haloTime;
	Clock m_haloClock;

	Sprite m_playerSprite;
	Sprite m_haloSprite;
	Sprite m_xpBarSprite;
};