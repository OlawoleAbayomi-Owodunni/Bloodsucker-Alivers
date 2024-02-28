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
	Tree
};

class Obstacle
{
public:
	Obstacle(sf::Texture& t_texture, ObstacleType t_type);
	~Obstacle();

	void update(double dt);
	void render(sf::RenderWindow& t_window);

private:
	RectangleShape m_rectangle;
	Vector2f m_position;

	Sprite m_sprite;
};