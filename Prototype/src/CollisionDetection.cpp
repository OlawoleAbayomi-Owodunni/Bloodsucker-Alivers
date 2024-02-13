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

bool CollisionDetection::bulletEnemyCollision(Bullet& t_bullet, Enemy& t_enemy)
{
	sf::FloatRect player = t_bullet.getCircle().getGlobalBounds();
	sf::FloatRect enemy = t_enemy.getRectangle().getGlobalBounds();

	if (player.intersects(enemy))
	{
		return true;
	}

	return false;
}