#include "Bullet.h"

Bullet::Bullet()
{
	m_velocity = sf::Vector2f(0.0f, 0.0f);
	m_position = sf::Vector2f(100.0f, 100.0f);

	m_circle.setRadius(5.0f);
	m_circle.setOrigin(m_circle.getRadius(), m_circle.getRadius());
	m_circle.setFillColor(sf::Color::Blue);
	m_circle.setPosition(m_position);
}

Bullet::~Bullet()
{
}

void Bullet::update(double dt, bool t_firing, sf::Vector2f t_playerPos, Enemy t_enemies[])
{
	if (!t_firing)
	{
		m_position = t_playerPos;
	}
	else if (t_firing)
	{

	}

	m_circle.setPosition(m_position);
}

void Bullet::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_circle);
}
