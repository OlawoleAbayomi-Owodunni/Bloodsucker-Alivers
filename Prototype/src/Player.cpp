#include "Player.h"
#include <iostream>

Player::Player()
{
	m_health = 100.0f;
	m_speed = 3.0f;
	m_position = sf::Vector2f(ScreenSize::s_width / 2.0f, ScreenSize::s_height / 2.0f);

	m_sprite.setSize(sf::Vector2f(40.0f, 40.0f));
	m_sprite.setOrigin(m_sprite.getSize().x / 2.0f, m_sprite.getSize().y / 2.0f);
	m_sprite.setFillColor(sf::Color::White);
	m_sprite.setPosition(m_position);

	m_emptyHealthBar.setSize(sf::Vector2f(50.0f, 6.0f));
	m_emptyHealthBar.setOrigin(m_emptyHealthBar.getSize().x / 2.0f, m_emptyHealthBar.getSize().y / 2.0f);
	m_emptyHealthBar.setFillColor(sf::Color::Red);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 35.0f);

	m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));
	m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
	m_currentHealthBar.setFillColor(sf::Color::Green);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 35.0f);
}

Player::~Player()
{
}

void Player::update(double dt)
{
	handleKeyInput();

	

	setHealth();
	setPosition(m_position.x, m_position.y);
}

void Player::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_sprite);
	t_window.draw(m_emptyHealthBar);
	t_window.draw(m_currentHealthBar);
}

void Player::handleKeyInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_position.x -= m_speed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_position.x += m_speed;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		m_position.y -= m_speed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		m_position.y += m_speed;
	}
}

void Player::setPosition(float t_x, float t_y)
{
	m_sprite.setPosition(m_position);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 35.0f);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 35.0f);
}

sf::Vector2f Player::getPosition()
{
	return m_position;
}

void Player::setHealth()
{
	if (m_health < 0)
	{
		m_health = 0;
	}

	m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));
}