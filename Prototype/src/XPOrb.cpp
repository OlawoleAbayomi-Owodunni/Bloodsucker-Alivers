#include "XPOrb.h"

XPOrb::XPOrb(sf::Vector2f t_position)
{
	m_position = t_position;

	m_circle.setRadius(5.0f);
	m_circle.setOrigin(m_circle.getRadius(), m_circle.getRadius());
	m_circle.setFillColor(sf::Color::Green);
	m_circle.setPosition(m_position);
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
}

sf::CircleShape XPOrb::getCircle()
{
	return m_circle;
}