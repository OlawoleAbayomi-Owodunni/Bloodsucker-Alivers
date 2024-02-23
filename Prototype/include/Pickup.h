#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"

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

	sf::Sprite m_sprite;
	sf::RectangleShape m_rectangle;
};