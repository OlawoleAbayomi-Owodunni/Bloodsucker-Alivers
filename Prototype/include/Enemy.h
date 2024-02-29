#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"
#include <vector>

using namespace std;
using namespace sf;

class Player;
enum class CharacterState;

enum class EnemyType
{
	Small,
	Boss
};

class Enemy
{
public:
	Enemy(sf::Texture& t_texture, sf::Vector2f t_playerPos, EnemyType t_type);
	~Enemy();

	void update(double dt, Player& t_player);
	void render(sf::RenderWindow& t_window);

	void initialisePosition(sf::Vector2f t_playerPos);
	void move(Player& t_player);
	void setPosition(float t_x, float t_y);
	sf::Vector2f getPosition();

	void setState(CharacterState t_state);
	CharacterState getState();

	EnemyType getType();

	void calculatePushFactor();

	void decreaseHealth(float t_damage);
	float getHealth();

	sf::RectangleShape getRectangle();

	void animate();
	void addFrame(sf::IntRect& t_frame);
	void setFrames();
	int getDeathFrameCounter();

	void playHitSound();

	void setColour(sf::Color t_colour);

private:
	static std::vector<Enemy*> m_allEnemies;

	EnemyType m_type;

	float m_maxHealth;
	float m_health;
	float m_speed;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_pushFactor;

	sf::SoundBuffer m_hitSoundBuffer;
	sf::Sound m_hitSound;

	sf::RectangleShape m_rectangle;
	sf::RectangleShape m_emptyHealthBar;
	sf::RectangleShape m_currentHealthBar;

	Clock m_damageClock;

	CharacterState m_enemyState;
	CharacterState m_previousState;

	std::vector<sf::IntRect> m_enemyFrames;
	int m_currentEnemyFrame;
	Time m_enemyTime;
	Clock m_enemyClock;

	int m_deathFrameCounter;

	thor::ResourceHolder<sf::Texture, std::string> m_holder;
	
	Sprite m_enemySprite;
};