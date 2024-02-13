#include "Weapon.h"

Weapon::Weapon()
{
	m_type = Type::Pistol;
	m_firing = false;
}

Weapon::~Weapon()
{
}

void Weapon::update(double dt, sf::Vector2f t_playerPos)
{


	m_bullet.update(dt, m_firing, t_playerPos);
}

void Weapon::render(sf::RenderWindow& t_window)
{
	m_bullet.render(t_window);
}