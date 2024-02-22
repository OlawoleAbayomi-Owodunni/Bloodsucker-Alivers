#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include "ScreenSize.h"

using namespace sf;
using namespace std;

class XPOrb
{
public:
	XPOrb(sf::Texture& t_texture, sf::Vector2f t_position);
	~XPOrb();

	void update(double dt);
	void render(sf::RenderWindow& t_window);

	CircleShape getCircle();

private:
	CircleShape m_circle;
	Vector2f m_position;

	vector<sf::IntRect> m_frames;
	int m_currentFrame;
	Time m_time;
	Clock m_clock;

	Sprite m_sprite;
};