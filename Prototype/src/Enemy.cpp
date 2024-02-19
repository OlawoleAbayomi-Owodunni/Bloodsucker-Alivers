#include "Enemy.h"
#include "Player.h"
#include <iostream>

Enemy::Enemy(sf::Texture& t_texture)
{
	m_speed = 1 + ((rand() % 10) / 10 + 0.1);

	m_rectangle.setSize(sf::Vector2f(40.0f, 40.0f));
	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::Red);
	m_rectangle.setPosition(m_position);

	m_enemySprite.setTexture(t_texture);
	m_enemySprite.setTextureRect(IntRect{ 0,0,64,64 });
	m_enemySprite.setOrigin(32, 32);
	m_enemySprite.setScale(3.0f, 3.0f);
	m_enemySprite.setPosition(m_position);
}

Enemy::~Enemy()
{
}

void Enemy::update(double dt, Player& t_player)
{
	move(t_player);
}

void Enemy::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_rectangle);
	t_window.draw(m_enemySprite);
}

void Enemy::initialisePosition()
{
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
		m_position.x += m_speed;
	}
	else
	{
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
