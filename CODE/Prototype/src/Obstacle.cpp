#include "Obstacle.h"

std::vector<Obstacle*> Obstacle::m_allObstacles{};

Obstacle::Obstacle(sf::Texture& t_texture, ObstacleType t_type, sf::Vector2f t_playerPos)
{
	m_type = t_type;

	float shortestDistance;
	float distance;

	do
	{
		shortestDistance = 6000000.0f;
		m_position.x = rand() % 3200 + 1;
		m_position.y = rand() % 1800 + 1;

		// * STEP 1: Get the closest obstacle and it's distance from this spawning point
		for (Obstacle const* other : m_allObstacles)
		{
			const sf::Vector2f difference = other->m_position - m_position;
			const float distanceToObject = std::sqrt(difference.x * difference.x + difference.y * difference.y);

			if (shortestDistance > distanceToObject)
				shortestDistance = distanceToObject;
		}

		// * STEP 2: Get the distance to the player spawn 
		const sf::Vector2f difference = t_playerPos - m_position;
		distance = std::sqrt(difference.x * difference.x + difference.y * difference.y);

	} 
	while ((distance < 400.0f) || (shortestDistance < 400.0f)); // Loop while obstacle's position is within 400 pixels of player
	
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
		m_topSprite.setTextureRect(IntRect{ 0,321,320,240 });
		m_bottomSprite.setTextureRect(IntRect{ 0,561,320,80 });
		
		m_rectangle.setSize(sf::Vector2f(100.0f, 30.0f));
		m_rectangle.setPosition(m_position.x - 30.0f, m_position.y - 25.0f);
		break;
	case ObstacleType::Building:
		m_topSprite.setTextureRect(IntRect{ 0,642,320,160 });
		m_bottomSprite.setTextureRect(IntRect{ 0,802,320,160 });

		m_rectangle.setSize(sf::Vector2f(240.0f, 160.0f));
		m_rectangle.setPosition(m_position.x, m_position.y - 20.0f);
		break;
	default:
		break;
	}

	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::White);

	m_topSprite.setOrigin(m_topSprite.getTextureRect().width / 2.0f, m_topSprite.getTextureRect().height / 2.0f);
	m_bottomSprite.setOrigin(m_bottomSprite.getTextureRect().width / 2.0f, m_bottomSprite.getTextureRect().height / 2.0f);

	m_topSprite.setPosition(m_position.x, m_position.y - 160.0f);
	m_bottomSprite.setPosition(m_position);

	m_allObstacles.push_back(this);
}

Obstacle::~Obstacle()
{
	// Find the position of the current enemy in the vector
	auto it = std::find(m_allObstacles.begin(), m_allObstacles.end(), this);
	if (it != m_allObstacles.end()) {
		// Remove the enemy from the vector
		m_allObstacles.erase(it);
	}
}

void Obstacle::update(double dt)
{
	//for (Obstacle const* other : m_allObstacles)
	//{
	//	if (m_position != other->m_position)
	//	{
	//		if (m_position.x > other->m_position.x - 160.0f && m_position.x < other->m_position.x + 160.0f &&
	//			m_position.y > other->m_position.y - 160.0f && m_position.y < other->m_position.y + 160.0f)
	//		{
	//			m_position.x = rand() % 3200 + 1;
	//			m_position.y = rand() % 1800 + 1;
	//		}
	//	}
	//}

	//switch (m_type)
	//{
	//case ObstacleType::Rock1:
	//	m_rectangle.setPosition(m_position.x, m_position.y);
	//	break;
	//case ObstacleType::Rock2:
	//	m_rectangle.setPosition(m_position.x, m_position.y - 20.0f);
	//	break;
	//case ObstacleType::Rock3:
	//	m_rectangle.setPosition(m_position.x, m_position.y);
	//	break;
	//case ObstacleType::Tree:
	//	m_rectangle.setPosition(m_position.x - 30.0f, m_position.y - 25.0f);
	//	break;
	//case ObstacleType::Building:
	//	m_rectangle.setPosition(m_position.x, m_position.y - 20.0f);
	//default:
	//	break;
	//}

	//m_topSprite.setPosition(m_position.x, m_position.y - 160.0f);
	//m_bottomSprite.setPosition(m_position);
}

void Obstacle::renderBottom(sf::RenderWindow& t_window)
{
	//t_window.draw(m_rectangle);
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

Vector2f Obstacle::getPosition()
{
	return m_position;
}
