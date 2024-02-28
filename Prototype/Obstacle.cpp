#include "Obstacle.h"

Obstacle::Obstacle(sf::Texture& t_texture, ObstacleType t_type)
{
	m_position.x = rand() % 3200 + 1;
	m_position.y = rand() % 1800 + 1;
	
	m_topSprite.setTexture(t_texture);
	m_bottomSprite.setTexture(t_texture);
	
	switch (t_type)
	{
	case ObstacleType::Rock1:
		m_topSprite.setTextureRect(IntRect{ 0,0,320,160 });
		m_bottomSprite.setTextureRect(IntRect{ 0,160,320,160 });
		
		m_rectangle.setSize(sf::Vector2f(240.0f, 160.0f));
		m_rectangle.setPosition(m_position.x, m_position.y);
		break;
	case ObstacleType::Rock2:
		m_topSprite.setTextureRect(IntRect{ 320,0,320,160 });
		m_bottomSprite.setTextureRect(IntRect{ 320,160,320,160 });
		
		m_rectangle.setSize(sf::Vector2f(180.0f, 120.0f));
		m_rectangle.setPosition(m_position.x, m_position.y - 20.0f);
		break;
	case ObstacleType::Rock3:
		m_topSprite.setTextureRect(IntRect{ 640,0,320,160 });
		m_bottomSprite.setTextureRect(IntRect{ 640,160,320,160 });
		
		m_rectangle.setSize(sf::Vector2f(280.0f, 160.0f));
		m_rectangle.setPosition(m_position.x, m_position.y);
		break;
	case ObstacleType::Tree:
		m_topSprite.setTextureRect(IntRect{ 0,321,320,160 });
		m_bottomSprite.setTextureRect(IntRect{ 0,481,320,160 });
		
		m_rectangle.setSize(sf::Vector2f(120.0f, 110.0f));
		m_rectangle.setPosition(m_position.x - 20.0f, m_position.y - 25.0f);
		break;
	default:
		break;
	}

	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::White);

	m_topSprite.setOrigin(160, 80);
	m_bottomSprite.setOrigin(160, 80);

	m_topSprite.setPosition(m_position.x, m_position.y - 160.0f);
	m_bottomSprite.setPosition(m_position);
}

Obstacle::~Obstacle()
{
}

void Obstacle::update(double dt)
{

}

void Obstacle::renderBottom(sf::RenderWindow& t_window)
{
	t_window.draw(m_rectangle);
	t_window.draw(m_bottomSprite);
}

void Obstacle::renderTop(sf::RenderWindow& t_window)
{
	t_window.draw(m_topSprite);
}

RectangleShape Obstacle::getRectangle()
{
	return m_rectangle;
}