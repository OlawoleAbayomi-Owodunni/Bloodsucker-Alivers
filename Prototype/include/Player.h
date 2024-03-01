#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
	WalkUpState,
	DashState,
	DeadState
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

	void initialise();

	void update(double dt, sf::View& t_view, std::vector<Enemy*> t_enemies);
	void renderPlayer(sf::RenderWindow& t_window);
	void renderHUD(sf::RenderWindow& t_window);
	
	void handleKeyInput();
	void rumbleStart();
	void strongRumbleStart();
	void weakRumbleStart();
	void rumbleStop();
	
	void setPosition(sf::View& t_view);
	sf::Vector2f getPosition();
	void pushBack();

	void setHealth();
	void decreaseHealth();
	void increaseHealth();

	void dash();
	void updateDashbar();

	void increaseXP();
	void checkXP();
	int getLevel();
	void levelUp(Gamemode& t_gamemode);
	void upgradePlayer(PlayerUpgrade t_type);
	void upgradeGun(WeaponType t_type);
	void upgradeDash();
	void carePackage(Gamemode& t_gamemode);
	void giveWeapon(WeaponType t_type);

	void animate();
	void addFrame(sf::IntRect& t_frame);
	void setFrames();

	void playSound(sf::Sound& t_sound);

	sf::RectangleShape getRectangle();
	sf::RectangleShape getDashCollider();
	sf::CircleShape getSlashCollider();
	std::vector<Weapon*> getWeapon();
	bool& getRumbleState();
	bool& getAliveState();
private:
	const float AFTERIMAGE_COUNT{ 16.0f };

	std::vector<AfterImageData> m_afterImages{};
	
	float m_maxHealth;
	float m_health;
	float m_speed;
	sf::Vector2f m_position;
	sf::Vector2f m_previousPosition;
	sf::Vector2f m_movementVector;
	
	int m_level;
	float m_xp;
	float m_xpRequired;

	float m_speedModifier;
	float m_xpModifier;
	float m_armorModifier;

	Direction m_direction;

	float m_dashDistance;
	bool m_canDash;
	Clock m_dashCooldownClock;
	Time m_dashCooldownTime;
	float m_dashBarFillAmount;
	int m_maxDashCharges;
	int m_currentDashCharges;
	bool m_dashHasAOE;

	sf::RectangleShape m_rectangle;

	sf::RectangleShape m_emptyHealthBar;
	sf::RectangleShape m_currentHealthBar;
	
	sf::RectangleShape m_xpBar;
	sf::RectangleShape m_emptyXPBar;

	sf::RectangleShape m_dashRect;
	sf::RectangleShape m_dashRectBounds;

	sf::CircleShape m_slashCircle;

	sf::RectangleShape m_dashBarLeft;
	sf::RectangleShape m_emptyDashBarLeft;
	sf::RectangleShape m_dashBarRight;
	sf::RectangleShape m_emptyDashBarRight;

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

	std::vector<sf::IntRect> m_slashFrames;
	int m_currentSlashFrame;
	Time m_slashTime;
	Clock m_slashClock;

	Time m_dashStateTime;
	Clock m_dashStateClock;

	Time m_dashAOETime;
	Clock m_dashAOEClock;

	SoundBuffer m_dashSoundBuffer;
	Sound m_dashSound;

	Sprite m_playerSprite;
	Sprite m_dashSprite;
	Sprite m_haloSprite;
	Sprite m_slashSprite;

	Sprite m_xpBarSprite;

	Sprite m_dashBarLeftSprite;
	Sprite m_dashBarRightSprite;

	//RUMBLE TIMERS
	bool isDashRumbling;
	Clock dashRumbleTimer;

	bool isPlayerAlive;
	Clock gameOverTimer;

	int score;
	int smallEK;
	int normalEK;
	int bigEK;
	int bossEK;
	int timeSurvived;
	//run a math based timer for time survived
};