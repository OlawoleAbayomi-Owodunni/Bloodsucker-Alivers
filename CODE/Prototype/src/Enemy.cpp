#include "Enemy.h"
#include "Player.h"
#include <iostream>

std::vector<Enemy*> Enemy::m_allEnemies{};

Enemy::Enemy(sf::Texture& t_texture, sf::Vector2f t_playerPos, EnemyType t_type)
{
	m_allEnemies.push_back(this);

	if (!m_hitSoundBuffer.loadFromFile("resources/sounds/enemy_hit.wav"))
	{
		std::cout << "error loading enemy hit sound";
	}
	m_hitSound.setBuffer(m_hitSoundBuffer);
	m_hitSound.setVolume(2.0f);

	initialisePosition(t_playerPos);

	m_type = t_type;

	m_enemyState = CharacterState::WalkSideState;
	m_previousState = CharacterState::None;

	m_deathFrameCounter = 0;
	m_currentEnemyFrame = 0;

	m_enemySprite.setTexture(t_texture);

	switch (m_type)
	{
	case EnemyType::Small:
		m_maxHealth = 100.0f;
		m_health = 100.0f;
		m_speed = 1.0f + ((rand() % 10) / 10.0f + 0.1f);
		m_enemyTime = seconds(0.1f);
		m_rectangle.setSize(sf::Vector2f(40.0f, 80.0f));
		m_enemySprite.setTextureRect(IntRect{ 0,192,160,160 });
		m_enemySprite.setScale(0.5f, 0.5f);
		break;
	case EnemyType::Big:
		m_maxHealth = 100.0f;
		m_health = 100.0f;
		m_speed = 0.5f + ((rand() % 10) / 10.0f + 0.1f);
		m_enemyTime = seconds(0.2f);
		m_rectangle.setSize(sf::Vector2f(90.0f, 140.0f));
		m_enemySprite.setTextureRect(IntRect{ 1587, 0, 320, 320 });
		m_enemySprite.setScale(0.5f, 0.5f);
		break;
	case EnemyType::Boss:
		m_maxHealth = 1000.0f;
		m_health = 1000.0f;
		m_speed = 1.5f + ((rand() % 10) / 10.0f + 0.1f);
		m_enemyTime = seconds(0.2f);
		m_rectangle.setSize(sf::Vector2f(180.0f, 280.0f));
		m_enemySprite.setTextureRect(IntRect{ 1587, 0, 320, 320 });
		m_enemySprite.setScale(1.0f, 1.0f);

		m_emptyHealthBar.setSize(sf::Vector2f(50.0f, 6.0f));
		m_emptyHealthBar.setOrigin(m_emptyHealthBar.getSize().x / 2.0f, m_emptyHealthBar.getSize().y / 2.0f);
		m_emptyHealthBar.setFillColor(sf::Color::Red);
		m_emptyHealthBar.setOutlineColor(sf::Color::Black);
		m_emptyHealthBar.setOutlineThickness(2.0f);
		m_emptyHealthBar.setPosition(m_position.x, m_position.y - 100.0f);

		m_currentHealthBar.setSize(sf::Vector2f(m_health / m_maxHealth * 50.0f, 6.0f));
		m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
		m_currentHealthBar.setFillColor(sf::Color::Green);
		m_currentHealthBar.setPosition(m_position.x, m_position.y - 100.0f);
		break;
	default:
		break;
	}
	
	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::Red);
	m_rectangle.setPosition(m_position);
	
	m_enemySprite.setOrigin(m_enemySprite.getTextureRect().width / 2.0f, m_enemySprite.getTextureRect().height / 2.0f);
	//m_enemySprite.setScale(0.5f, 0.5f);
	m_enemySprite.setPosition(m_position);

	/*m_emptyHealthBar.setSize(sf::Vector2f(50.0f, 6.0f));
	m_emptyHealthBar.setOrigin(m_emptyHealthBar.getSize().x / 2.0f, m_emptyHealthBar.getSize().y / 2.0f);
	m_emptyHealthBar.setFillColor(sf::Color::Red);
	m_emptyHealthBar.setOutlineColor(sf::Color::Black);
	m_emptyHealthBar.setOutlineThickness(2.0f);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 35.0f);

	m_currentHealthBar.setSize(sf::Vector2f(m_health / m_maxHealth * 100.0f, 6.0f));
	m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
	m_currentHealthBar.setFillColor(sf::Color::Green);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 50.0f);*/
}

Enemy::~Enemy()
{
	// Find the position of the current enemy in the vector
	auto it = std::find(m_allEnemies.begin(), m_allEnemies.end(), this);
	if (it != m_allEnemies.end()) {
		// Remove the enemy from the vector
		m_allEnemies.erase(it);
	}
}

void Enemy::update(double dt, Player& t_player)
{
	switch (m_type)
	{
	case EnemyType::Big:
		if (m_damageClock.getElapsedTime().asSeconds() > 0.5f)
		{
			m_enemySprite.setColor(sf::Color::White);
		}
		break;
	case EnemyType::Boss:
		m_currentHealthBar.setSize(sf::Vector2f(m_health / m_maxHealth * 50.0f, 6.0f));

		if (m_damageClock.getElapsedTime().asSeconds() > 0.5f)
		{
			m_enemySprite.setColor(sf::Color::White);
		}
		break;
	default:
		break;
	}
	//m_currentHealthBar.setSize(sf::Vector2f(m_health / m_maxHealth * 50.0f, 6.0f));

	if (m_enemyState == CharacterState::WalkSideState)
	{
		move(t_player);
	}
	
	if (m_enemyState != m_previousState)
	{
		setFrames();
	}
	animate();
}

void Enemy::render(sf::RenderWindow& t_window)
{
	//t_window.draw(m_rectangle);
	t_window.draw(m_enemySprite);

	switch (m_type)
	{
	case EnemyType::Boss:
		t_window.draw(m_emptyHealthBar);
		t_window.draw(m_currentHealthBar);
		break;
	default:
		break;
	}
	//t_window.draw(m_emptyHealthBar);
	//t_window.draw(m_currentHealthBar);
}

void Enemy::initialisePosition(sf::Vector2f t_playerPos)
{
	switch (m_type)
	{
	case EnemyType::Small:
		m_health = 100.0f;
		break;
	case EnemyType::Big:
		m_health = 100.0f;
		break;
	case EnemyType::Boss:
		m_health = 1000.0f;
		break;
	default:
		break;
	}
	
	
	m_enemyState = CharacterState::WalkSideState;
	m_deathFrameCounter = 0;

	float xPos = rand() % 3200 + 1;
	float yPos = rand() % 1800 + 1;
	
	while (xPos > t_playerPos.x - 800 && xPos < t_playerPos.x + 800 &&
		   yPos > t_playerPos.y - 450 && yPos < t_playerPos.y + 450)
	{
		xPos = rand() % 3200 + 1;
		yPos = rand() % 1800 + 1;
	}
	
	m_position = sf::Vector2f(xPos, yPos);
}

void Enemy::move(Player& t_player)
{
	sf::Vector2f playerPos = t_player.getPosition();
	sf::Vector2f displacement;
	float distance;

	displacement.x = playerPos.x - m_position.x;
	displacement.y = playerPos.y - m_position.y;

	distance = std::sqrtf(displacement.x * displacement.x + displacement.y * displacement.y);

	calculatePushFactor();

	m_velocity = (displacement / distance) * m_speed;
	m_position += m_velocity;

	m_position += m_pushFactor;

	if (m_position.x < playerPos.x)
	{
		switch (m_type)
		{
		case EnemyType::Small:
			m_enemySprite.setScale(0.5f, 0.5f);
			break;
		case EnemyType::Big:
			m_enemySprite.setScale(0.5f, 0.5f);
			break;
		case EnemyType::Boss:
			m_enemySprite.setScale(1.0f, 1.0f);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (m_type)
		{
		case EnemyType::Small:
			m_enemySprite.setScale(-0.5f, 0.5f);
			break;
		case EnemyType::Big:
			m_enemySprite.setScale(-0.5f, 0.5f);
			break;
		case EnemyType::Boss:
			m_enemySprite.setScale(-1.0f, 1.0f);
			break;
		default:
			break;
		}
	}

	m_rectangle.setPosition(m_position);
	m_enemySprite.setPosition(m_position);

	switch (m_type)
	{
	case EnemyType::Small:
		break;
	case EnemyType::Boss:
		m_emptyHealthBar.setPosition(m_position.x, m_position.y - 100.0f);
		m_currentHealthBar.setPosition(m_position.x, m_position.y - 100.0f);
		break;
	default:
		break;
	}

	//m_emptyHealthBar.setPosition(m_position.x, m_position.y - 100.0f);
	//m_currentHealthBar.setPosition(m_position.x, m_position.y - 100.0f);
}

void Enemy::calculatePushFactor()
{
	m_pushFactor.x = 0;
	m_pushFactor.y = 0;

	for (Enemy const* other : m_allEnemies)
	{
		sf::Vector2f difference = m_position - other->m_position;
		float magnitudeSqrd = difference.x * difference.x + difference.y * difference.y;

		if (magnitudeSqrd > 2000)
		{
			continue;
		}
		m_pushFactor += difference;
	}

	m_pushFactor *= 0.06f;
}

void Enemy::adjustMovement()
{
	if (abs(m_velocity.x) > abs(m_velocity.y))
	{
		if (m_velocity.y > 0)
		{
			m_velocity.y -= 3.0f;
		}
		else
		{
			m_velocity.y += 3.0f;
		}
		
		m_velocity.x = 0.0f;
	}
	else if (abs(m_velocity.x) < abs(m_velocity.y))
	{
		if (m_velocity.x > 0)
		{
			m_velocity.x -= 3.0f;
		}
		else
		{
			m_velocity.x += 3.0f;
		}

		m_velocity.y = 0.0f;
	}

	m_position += m_velocity;
}

void Enemy::setPosition(float t_x, float t_y)
{
	m_position = sf::Vector2f(t_x, t_y);
}

sf::Vector2f Enemy::getPosition()
{
	return m_position;
}

void Enemy::setState(CharacterState t_state)
{
	m_enemyState = t_state;
}

CharacterState Enemy::getState()
{
	return m_enemyState;
}

EnemyType Enemy::getType()
{
	return m_type;
}

sf::RectangleShape Enemy::getRectangle()
{
	return m_rectangle;
}

void Enemy::decreaseHealth(float t_damage)
{
	m_health -= t_damage;
}

float Enemy::getHealth()
{
	return m_health;
}

void Enemy::animate()
{
	if (m_enemyClock.getElapsedTime() > m_enemyTime)
	{
		if (m_currentEnemyFrame + 1 < m_enemyFrames.size())
		{
			m_currentEnemyFrame++;
		}
		else
		{
			m_currentEnemyFrame = 0;
		}

		if (m_enemyState == CharacterState::DeadState)
		{
			m_deathFrameCounter++;
		}

		m_enemyClock.restart();
	}

	m_enemySprite.setTextureRect(m_enemyFrames[m_currentEnemyFrame]);
	m_previousState = m_enemyState;
}

void Enemy::addFrame(sf::IntRect& t_frame)
{
	m_enemyFrames.push_back(t_frame);
}

void Enemy::setFrames()
{
	m_enemyFrames.clear();
	m_currentEnemyFrame = 0;

	switch (m_type)
	{
	case EnemyType::Small:
		switch (m_enemyState)
		{
		case CharacterState::WalkSideState:
			addFrame(IntRect{ 0,192,160,160 });
			addFrame(IntRect{ 160,192,160,160 });
			addFrame(IntRect{ 320,192,160,160 });
			addFrame(IntRect{ 480,192,160,160 });
			addFrame(IntRect{ 640,192,160,160 });
			break;
		case CharacterState::DeadState:
			addFrame(IntRect{ 3744,0,160,160 });
			addFrame(IntRect{ 3904,0,160,160 });
			addFrame(IntRect{ 4064,0,160,160 });
			addFrame(IntRect{ 4224,0,160,160 });
			addFrame(IntRect{ 4384,0,160,160 });

			addFrame(IntRect{ 4544,0,160,160 });
			addFrame(IntRect{ 4704,0,160,160 });
			addFrame(IntRect{ 4864,0,160,160 });
			addFrame(IntRect{ 5024,0,160,160 });
			addFrame(IntRect{ 5184,0,160,160 });

			addFrame(IntRect{ 5344,0,160,160 });
			addFrame(IntRect{ 5504,0,160,160 });
			break;
		default:
			break;
		}
		break;
	case EnemyType::Big:
		switch (m_enemyState)
		{
		case CharacterState::WalkSideState:
			addFrame(IntRect{ 1587,0,320,320 });
			addFrame(IntRect{ 1907,0,320,320 });
			addFrame(IntRect{ 2227,0,320,320 });
			addFrame(IntRect{ 2547,0,320,320 });
			addFrame(IntRect{ 2867,0,320,320 });
			addFrame(IntRect{ 3187,0,320,320 });
			break;
		case CharacterState::DeadState:
			addFrame(IntRect{ 0,2496,320,320 });
			addFrame(IntRect{ 320,2496,320,320 });
			addFrame(IntRect{ 640,2496,320,320 });
			addFrame(IntRect{ 960,2496,320,320 });
			addFrame(IntRect{ 1280,2496,320,320 });

			addFrame(IntRect{ 1600,2496,320,320 });
			addFrame(IntRect{ 1920,2496,320,320 });
			addFrame(IntRect{ 2240,2496,320,320 });
			addFrame(IntRect{ 2560,2496,320,320 });
			addFrame(IntRect{ 2880,2496,320,320 });

			addFrame(IntRect{ 3200,2496,320,320 });
			addFrame(IntRect{ 3520,2496,320,320 });
			addFrame(IntRect{ 3840,2496,320,320 });
			addFrame(IntRect{ 4160,2496,320,320 });
			addFrame(IntRect{ 4480,2496,320,320 });
			addFrame(IntRect{ 4800,2496,320,320 });
			break;
		default:
			break;
		}
	case EnemyType::Boss:
		switch (m_enemyState)
		{
		case CharacterState::WalkSideState:
			addFrame(IntRect{ 1587,0,320,320 });
			addFrame(IntRect{ 1907,0,320,320 });
			addFrame(IntRect{ 2227,0,320,320 });
			addFrame(IntRect{ 2547,0,320,320 });
			addFrame(IntRect{ 2867,0,320,320 });
			addFrame(IntRect{ 3187,0,320,320 });
			break;
		case CharacterState::DeadState:
			addFrame(IntRect{ 0,2496,320,320 });
			addFrame(IntRect{ 320,2496,320,320 });
			addFrame(IntRect{ 640,2496,320,320 });
			addFrame(IntRect{ 960,2496,320,320 });
			addFrame(IntRect{ 1280,2496,320,320 });

			addFrame(IntRect{ 1600,2496,320,320 });
			addFrame(IntRect{ 1920,2496,320,320 });
			addFrame(IntRect{ 2240,2496,320,320 });
			addFrame(IntRect{ 2560,2496,320,320 });
			addFrame(IntRect{ 2880,2496,320,320 });

			addFrame(IntRect{ 3200,2496,320,320 });
			addFrame(IntRect{ 3520,2496,320,320 });
			addFrame(IntRect{ 3840,2496,320,320 });
			addFrame(IntRect{ 4160,2496,320,320 });
			addFrame(IntRect{ 4480,2496,320,320 });
			addFrame(IntRect{ 4800,2496,320,320 });
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	
}

int Enemy::getDeathFrameCounter()
{
	return m_deathFrameCounter;
}

void Enemy::playHitSound()
{
	m_hitSound.stop();
	m_hitSound.play();
}

void Enemy::setColour(sf::Color t_colour)
{
	m_enemySprite.setColor(t_colour);
	m_damageClock.restart();
}