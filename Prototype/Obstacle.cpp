#include "Obstacle.h"

Obstacle::Obstacle(sf::Texture& t_texture, ObstacleType t_type)
{
	m_position.x = rand() % 3200 + 1;
	m_position.y = rand() % 1800 + 1;

	m_sprite.setTexture(t_texture);
	
	switch (t_type)
	{
	case ObstacleType::Rock1:
		m_sprite.setTextureRect(IntRect{ 0,0,320,320 });
		break;
	case ObstacleType::Rock2:
		m_sprite.setTextureRect(IntRect{ 320,0,320,320 });
		break;
	case ObstacleType::Rock3:
		m_sprite.setTextureRect(IntRect{ 640,0,320,320 });
		break;
	case ObstacleType::Tree:
		m_sprite.setTextureRect(IntRect{ 0,321,320,320 });
		break;
	default:
		break;
	}

	m_sprite.setOrigin(160, 160);
	m_sprite.setPosition(m_position);
}

Obstacle::~Obstacle()
{
}

void Obstacle::update(double dt)
{

}

void Obstacle::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_rectangle);
	t_window.draw(m_sprite);
}