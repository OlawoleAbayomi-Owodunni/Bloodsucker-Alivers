#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"

using namespace sf;
using namespace std;

enum class PickupType
{
	Health
};

class Pickup
{
public:
	Pickup(sf::Texture& t_texture, sf::Vector2f t_position, PickupType t_type);
	~Pickup();

	void update(double dt);
	void render(sf::RenderWindow& t_window);

	sf::RectangleShape getRectangle();
	PickupType getType();

private:
	PickupType m_type;
	sf::Vector2f m_position;

	sf::RectangleShape m_rectangle;

	vector<sf::IntRect> m_frames;
	int m_currentFrame;
	Time m_time;
	Clock m_clock;

	sf::Sprite m_sprite;
};