#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"

using namespace sf;
using namespace std;

enum class ObstacleType
{
	Rock1,
	Rock2,
	Rock3,
	Tree,
	Building
};

class Obstacle
{
public:
	Obstacle(sf::Texture& t_texture, ObstacleType t_type);
	~Obstacle();

	void update(double dt);
	void renderBottom(sf::RenderWindow& t_window);
	void renderTop(sf::RenderWindow& t_window);

	RectangleShape getRectangle();

private:
	static std::vector<Obstacle*> m_allObstacles;

	ObstacleType m_type;

	RectangleShape m_rectangle;
	Vector2f m_position;

	Sprite m_topSprite;
	Sprite m_bottomSprite;
};