#include "Enemy.h"
#include "Player.h"
#include <iostream>

Enemy::Enemy(sf::Texture& t_texture)
{
	m_health = 100.0f;
	m_speed = 1 + ((rand() % 10) / 10 + 0.1);

	m_rectangle.setSize(sf::Vector2f(40.0f, 80.0f));
	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::Red);
	m_rectangle.setPosition(m_position);

	m_emptyHealthBar.setSize(sf::Vector2f(50.0f, 6.0f));
	m_emptyHealthBar.setOrigin(m_emptyHealthBar.getSize().x / 2.0f, m_emptyHealthBar.getSize().y / 2.0f);
	m_emptyHealthBar.setFillColor(sf::Color::Red);
	m_emptyHealthBar.setOutlineColor(sf::Color::Black);
	m_emptyHealthBar.setOutlineThickness(2.0f);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 35.0f);

	m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));
	m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
	m_currentHealthBar.setFillColor(sf::Color::Green);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 50.0f);

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
	m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));

	move(t_player);
}

void Enemy::render(sf::RenderWindow& t_window)
{
	//t_window.draw(m_rectangle);
	t_window.draw(m_enemySprite);
	t_window.draw(m_emptyHealthBar);
	t_window.draw(m_currentHealthBar);
}

void Enemy::initialisePosition()
{
	m_health = 100.0f;

	float xPos = rand() % 2400 + 100;
	float yPos = rand() % 1350 + 100;
	
	if (xPos < ScreenSize::s_width && xPos > 0)
	{
		xPos = -(rand() % 800 + 100);
	}
	if (yPos < ScreenSize::s_height && yPos > 0)
	{
		yPos = -(rand() % 450 + 100);
	}
	
	m_position = sf::Vector2f(xPos, yPos);
}

void Enemy::move(Player& t_player)
{
	if (m_position.x < t_player.getPosition().x)
	{
		m_enemySprite.setScale(0.5f, 0.5f);
		m_position.x += m_speed;
	}
	else
	{
		m_enemySprite.setScale(-0.5f, 0.5f);
		m_position.x -= m_speed;
	}

	if (m_position.y < t_player.getPosition().y)
	{
		m_position.y += m_speed;
	}
	else
	{
		m_position.y -= m_speed;
	}

	m_rectangle.setPosition(m_position);
	m_enemySprite.setPosition(m_position);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 50.0f);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 50.0f);
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