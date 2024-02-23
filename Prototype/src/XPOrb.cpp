#include "XPOrb.h"

XPOrb::XPOrb(sf::Texture& t_texture, sf::Vector2f t_position)
{
	m_position = t_position;

	m_circle.setRadius(5.0f);
	m_circle.setOrigin(m_circle.getRadius(), m_circle.getRadius());
	m_circle.setFillColor(sf::Color::Green);
	m_circle.setPosition(m_position);

	m_orbSprite.setTexture(t_texture);
	m_orbSprite.setTextureRect(IntRect{ 0,128,64,64 });
	m_orbSprite.setOrigin(32, 32);
	m_orbSprite.setScale(1.0f, 1.0f);
	m_orbSprite.setPosition(m_position);
}

XPOrb::~XPOrb()
{
}

void XPOrb::update(double dt)
{

}

void XPOrb::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_circle);
	t_window.draw(m_orbSprite);
}

sf::CircleShape XPOrb::getCircle()
{
	return m_circle;
}