#include "CollisionDetection.h"

CollisionDetection::CollisionDetection()
{
}

CollisionDetection::~CollisionDetection()
{
}

bool CollisionDetection::playerEnemyCollision(Player& t_player, Enemy& t_enemy)
{
	sf::FloatRect player = t_player.getRectangle().getGlobalBounds();
	sf::FloatRect enemy = t_enemy.getRectangle().getGlobalBounds();

	if (player.intersects(enemy))
	{
		return true;
	}

	return false;
}
