#pragma once
#include <SFML/Graphics.hpp>

class Bullet
{
public:
	Bullet();
	~Bullet();

	void update(double dt, bool t_firing, sf::Vector2f t_playerPos);
	void render(sf::RenderWindow& t_window);

private:
	sf::CircleShape m_circle;
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};