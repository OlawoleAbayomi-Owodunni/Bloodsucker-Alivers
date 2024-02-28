#pragma once
#include <SFML/Graphics.hpp>
#include "ScreenSize.h"
#include "Player.h"
#include "Enemy.h"
#include "XPOrb.h"
#include "Pickup.h"
#include "Obstacle.h"

class CollisionDetection
{
public:
	CollisionDetection();
	~CollisionDetection();

	bool static playerEnemyCollision(Player& t_player, Enemy* t_enemy);
	bool static bulletEnemyCollision(Bullet* t_bullet, Enemy* t_enemy);
	bool static playerOrbCollision(Player& t_player, XPOrb* t_orb);
	bool static playerPickupCollision(Player& t_player, Pickup* t_pickup);
	bool static playerDashEnemyCollision(Player& t_player, Enemy* t_enemy);
	bool static playerObstacleCollision(Player& t_player, Obstacle* t_obstacle);
};