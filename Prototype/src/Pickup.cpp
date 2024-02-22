#include "Pickup.h"

Pickup::Pickup(sf::Texture& t_texture, sf::Vector2f t_position, PickupType t_type) : m_type(t_type)
{
	m_position = sf::Vector2f(t_position.x + 20.0f, t_position.y);

	m_rectangle.setSize(sf::Vector2f(40.0f, 40.0f));
	m_rectangle.setOrigin(20.0f, 20.0f);
	m_rectangle.setPosition(m_position);

	for (int i = 0; i < 10; i++)
	{
		m_frames.push_back(IntRect{ 64 * i,352,64,64 });
	}
	m_currentFrame = 0;
	m_time = seconds(0.2f);

	switch (m_type)
	{
	case PickupType::Health:
		m_rectangle.setFillColor(sf::Color::Green);

		m_sprite.setTexture(t_texture);
		m_sprite.setTextureRect(sf::IntRect{ 0,352,64,64 });
		m_sprite.setOrigin(32, 32);
		break;
	default:
		break;
	}
	
	m_sprite.setPosition(m_position);
}

Pickup::~Pickup()
{
}

void Pickup::update(double dt)
{
	if (m_clock.getElapsedTime() > m_time)
	{
		if (m_currentFrame + 1 < m_frames.size())
		{
			m_currentFrame++;
		}
		else
		{
			m_currentFrame = 0;
		}
		m_clock.restart();
	}

	m_sprite.setTextureRect(m_frames[m_currentFrame]);
}

void Pickup::render(sf::RenderWindow& t_window)
{
	//t_window.draw(m_rectangle);
	t_window.draw(m_sprite);
}

sf::RectangleShape Pickup::getRectangle()
{
	return m_rectangle;
}

PickupType Pickup::getType()
{
	return m_type;
}