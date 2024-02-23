#include "Pickup.h"

Pickup::Pickup(sf::Texture& t_texture, sf::Vector2f t_position, PickupType t_type) : m_type(t_type)
{
	m_position = sf::Vector2f(t_position.x + 20.0f, t_position.y);

	m_rectangle.setSize(sf::Vector2f(40.0f, 40.0f));
	m_rectangle.setOrigin(20.0f, 20.0f);
	m_rectangle.setPosition(m_position);

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

}

void Pickup::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_rectangle);
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