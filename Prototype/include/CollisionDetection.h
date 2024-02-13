#pragma once
#include <SFML/Graphics.hpp>
#include "ScreenSize.h"
#include "Player.h"
#include "Enemy.h"

class CollisionDetection
{
public:
	CollisionDetection();
	~CollisionDetection();

	bool static playerEnemyCollision(Player& t_player, Enemy& t_enemy);
	bool static bulletEnemyCollision(Bullet& t_bullet, Enemy& t_enemy);

private:

};