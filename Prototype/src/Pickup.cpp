#include "Pickup.h"

Pickup::Pickup(sf::Vector2f t_position, PickupType t_type) : m_type(t_type)
{
	m_position = sf::Vector2f(t_position.x + 20.0f, t_position.y);

	m_rectangle.setSize(sf::Vector2f(30.0f, 20.0f));
	m_rectangle.setOrigin(10.0f, 10.0f);
	m_rectangle.setPosition(m_position);

	switch (m_type)
	{
	case PickupType::Health:
		m_rectangle.setFillColor(sf::Color::Green);
		break;
	default:
		break;
	}
	
}

Pickup::~Pickup()
{
}

void Pickup::update(double dt)
{

}

void Pickup::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_rectangle);
}

sf::RectangleShape Pickup::getRectangle()
{
	return m_rectangle;
}

PickupType Pickup::getType()
{
	return m_type;
}