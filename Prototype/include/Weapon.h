#pragma once
#include <SFML/Graphics.hpp>
#include "Bullet.h"

enum class Type
{
	Pistol
};

class Weapon
{
public:
	Weapon();
	~Weapon();

private:
	Type m_type;
};