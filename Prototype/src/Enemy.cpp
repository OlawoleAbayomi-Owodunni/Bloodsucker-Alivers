#include "Enemy.h"
#include <iostream>

Enemy::Enemy()
{
	m_speed = 1.0f;
	m_position = sf::Vector2f(rand() % ScreenSize::s_width, rand() % ScreenSize::s_height);

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