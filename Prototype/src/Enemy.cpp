#include "Enemy.h"
#include "Player.h"
#include <iostream>

std::vector<Enemy*> Enemy::m_allEnemies{};

Enemy::Enemy(sf::Texture& t_texture)
{
	m_allEnemies.push_back(this);

	m_health = 100.0f;
	m_speed = 1.0f + ((rand() % 10) / 10.0f + 0.1f);

	m_enemyState = CharacterState::WalkSideState;
	m_previousState = CharacterState::None;

	m_enemyTime = seconds(0.2f);

	m_rectangle.setSize(sf::Vector2f(40.0f, 80.0f));
	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::Red);
	m_rectangle.setPosition(m_position);

	/*m_emptyHealthBar.setSize(sf::Vector2f(50.0f, 6.0f));
	m_emptyHealthBar.setOrigin(m_emptyHealthBar.getSize().x / 2.0f, m_emptyHealthBar.getSize().y / 2.0f);
	m_emptyHealthBar.setFillColor(sf::Color::Red);
	m_emptyHealthBar.setOutlineColor(sf::Color::Black);
	m_emptyHealthBar.setOutlineThickness(2.0f);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 35.0f);

	m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));
	m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
	m_currentHealthBar.setFillColor(sf::Color::Green);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 50.0f);*/

	m_enemySprite.setTexture(t_texture);
	m_enemySprite.setTextureRect(IntRect{ 0,192,160,160 });
	m_enemySprite.setOrigin(80, 80);
	m_enemySprite.setScale(0.5f, 0.5f);
	m_enemySprite.setPosition(m_position);
}

Enemy::~Enemy()
{
}

void Enemy::update(double dt, Player& t_player)
{
	//m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));

	move(t_player);

	if (m_enemyState != m_previousState)
	{
		setFrames();
	}
	animate();
}

void Enemy::render(sf::RenderWindow& t_window)
{
	//t_window.draw(m_rectangle);
	t_window.draw(m_enemySprite);
	//t_window.draw(m_emptyHealthBar);
	//t_window.draw(m_currentHealthBar);
}

void Enemy::initialisePosition(sf::Vector2f t_playerPos)
{
	m_health = 100.0f;

	float xPos = rand() % 3200 + 1;
	float yPos = rand() % 1800 + 1;
	
	while (xPos > t_playerPos.x - 800 && xPos < t_playerPos.x + 800 &&
		   yPos > t_playerPos.y - 450 && yPos < t_playerPos.y + 450)
	{
		xPos = rand() % 3200 + 1;
		yPos = rand() % 1800 + 1;
	}
	
	m_position = sf::Vector2f(xPos, yPos);
}

void Enemy::move(Player& t_player)
{
	sf::Vector2f playerPos = t_player.getPosition();
	sf::Vector2f displacement;
	float distance;

	displacement.x = playerPos.x - m_position.x;
	displacement.y = playerPos.y - m_position.y;

	distance = std::sqrtf(displacement.x * displacement.x + displacement.y * displacement.y);

	calculatePushFactor();

	m_velocity = (displacement / distance) * m_speed;
	m_position += m_velocity;
	m_position += m_pushFactor;

	if (m_position.x < playerPos.x)
	{
		m_enemySprite.setScale(0.5f, 0.5f);
	}
	else
	{
		m_enemySprite.setScale(-0.5f, 0.5f);
	}

	m_rectangle.setPosition(m_position);
	m_enemySprite.setPosition(m_position);
	//m_emptyHealthBar.setPosition(m_position.x, m_position.y + 50.0f);
	//m_currentHealthBar.setPosition(m_position.x, m_position.y + 50.0f);
}

void Enemy::calculatePushFactor()
{
	m_pushFactor.x = 0;
	m_pushFactor.y = 0;

	for (Enemy const* other : m_allEnemies)
	{
		sf::Vector2f difference = m_position - other->m_position;
		float magnitudeSqrd = difference.x * difference.x + difference.y * difference.y;

		if (magnitudeSqrd > 2000)
		{
			continue;
		}
		m_pushFactor += difference;
	}

	m_pushFactor *= 0.06f;
}

void Enemy::setPosition(float t_x, float t_y)
{
	m_position = sf::Vector2f(t_x, t_y);
}

sf::Vector2f Enemy::getPosition()
{
	return m_position;
}

sf::RectangleShape Enemy::getRectangle()
{
	return m_rectangle;
}

void Enemy::decreaseHealth(float t_damage)
{
	m_health -= t_damage;
}

float Enemy::getHealth()
{
	return m_health;
}

void Enemy::animate()
{
	if (m_enemyClock.getElapsedTime() > m_enemyTime)
	{
		if (m_currentEnemyFrame + 1 < m_enemyFrames.size())
		{
			m_currentEnemyFrame++;
		}
		else
		{
			m_currentEnemyFrame = 0;
		}
		m_enemyClock.restart();
	}

	m_enemySprite.setTextureRect(m_enemyFrames[m_currentEnemyFrame]);
	m_previousState = m_enemyState;
}

void Enemy::addFrame(sf::IntRect& t_frame)
{
	m_enemyFrames.push_back(t_frame);
}

void Enemy::setFrames()
{
	m_enemyFrames.clear();
	m_currentEnemyFrame = 0;

	switch (m_enemyState)
	{
	case CharacterState::IdleState:
		break;
	case CharacterState::WalkSideState:
		addFrame(IntRect{ 0,192,160,160 });
		addFrame(IntRect{ 160,192,160,160 });
		addFrame(IntRect{ 320,192,160,160 });
		addFrame(IntRect{ 480,192,160,160 });
		addFrame(IntRect{ 640,192,160,160 });
		break;
	default:
		break;
	}
}