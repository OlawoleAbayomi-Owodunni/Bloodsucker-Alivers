#include "CollisionDetection.h"

CollisionDetection::CollisionDetection()
{
}

CollisionDetection::~CollisionDetection()
{
}

bool CollisionDetection::playerEnemyCollision(Player& t_player, Enemy* t_enemy)
{
	sf::FloatRect player = t_player.getRectangle().getGlobalBounds();
	sf::FloatRect enemy = t_enemy->getRectangle().getGlobalBounds();

	if (player.intersects(enemy))
	{
		return true;
	}

	return false;
}

bool CollisionDetection::bulletEnemyCollision(Bullet* t_bullet, Enemy* t_enemy)
{
	sf::FloatRect bullet = t_bullet->getCircle().getGlobalBounds();
	sf::FloatRect enemy = t_enemy->getRectangle().getGlobalBounds();

	if (bullet.intersects(enemy))
	{
		return true;
	}

	return false;
}

bool CollisionDetection::playerOrbCollision(Player& t_player, XPOrb* t_orb)
{
	sf::FloatRect player = t_player.getRectangle().getGlobalBounds();
	sf::FloatRect orb = t_orb->getCircle().getGlobalBounds();

	if (player.intersects(orb))
	{
		return true;
	}

	return false;
}

bool CollisionDetection::playerPickupCollision(Player& t_player, Pickup* t_pickup)
{
	sf::FloatRect player = t_player.getRectangle().getGlobalBounds();
	sf::FloatRect pickup = t_pickup->getRectangle().getGlobalBounds();

	if (player.intersects(pickup))
	{
		return true;
	}

	return false;
}

bool CollisionDetection::playerDashEnemyCollision(Player& t_player, Enemy* t_enemy)
{
	sf::FloatRect playerDash = t_player.getDashCollider().getGlobalBounds();
	sf::FloatRect enemy = t_enemy->getRectangle().getGlobalBounds();

	if (playerDash.intersects(enemy))
	{
		return true;
	}

	return false;
}

bool CollisionDetection::playerObstacleCollision(Player& t_player, Obstacle* t_obstacle)
{
	sf::FloatRect player = t_player.getRectangle().getGlobalBounds();
	sf::FloatRect obstacle = t_obstacle->getRectangle().getGlobalBounds();

	if (player.intersects(obstacle))
	{
		return true;
	}

	return false;
}