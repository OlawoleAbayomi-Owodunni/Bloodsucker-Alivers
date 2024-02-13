#include "Enemy.h"
#include <iostream>

Enemy::Enemy()
{
	m_speed = 2.0f;

	m_sprite.setSize(sf::Vector2f(40.0f, 40.0f));
	m_sprite.setOrigin(m_sprite.getSize().x / 2.0f, m_sprite.getSize().y / 2.0f);
	m_sprite.setFillColor(sf::Color::Red);
	m_sprite.setPosition(m_position);
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
	t_window.draw(m_sprite);
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

	m_sprite.setPosition(m_position);
}

void Enemy::setPosition(float t_x, float t_y)
{
	m_position = sf::Vector2f(t_x, t_y);
}

sf::Vector2f Enemy::getPosition()
{
	return m_position;
}
