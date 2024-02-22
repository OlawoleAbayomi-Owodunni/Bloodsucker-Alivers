#include "XPOrb.h"

XPOrb::XPOrb(sf::Texture& t_texture, sf::Vector2f t_position)
{
	m_position = t_position;

	for (int i = 0; i < 10; i++)
	{
		m_frames.push_back(IntRect{64*i,128,64,64});
	}
	m_currentFrame = 0;
	m_time = seconds(0.1f);

	m_circle.setRadius(10.0f);
	m_circle.setOrigin(m_circle.getRadius(), m_circle.getRadius());
	m_circle.setFillColor(sf::Color::Green);
	m_circle.setPosition(m_position);

	m_sprite.setTexture(t_texture);
	m_sprite.setTextureRect(IntRect{ 0,128,64,64 });
	m_sprite.setOrigin(32, 32);
	m_sprite.setScale(1.0f, 1.0f);
	m_sprite.setPosition(m_position);
}

XPOrb::~XPOrb()
{
}

void XPOrb::update(double dt)
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

void XPOrb::render(sf::RenderWindow& t_window)
{
	//t_window.draw(m_circle);
	t_window.draw(m_sprite);
}

sf::CircleShape XPOrb::getCircle()
{
	return m_circle;
}