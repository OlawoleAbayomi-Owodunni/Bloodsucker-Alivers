#include "Player.h"
#include "Game.h"
#include <iostream>

Player::Player()
{
	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));
	m_holder.acquire("mapSprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/Map.png"));

	m_maxHealth = 100.0f;
	m_health = m_maxHealth;
	m_speed = 2.0f;
	m_level = 1;
	m_xp = 0;
	m_xpRequired = 10.0f;

	m_speedModifier = 1;
	m_xpModifier = 1;
	m_armorModifier = 1;
	
	m_weapons.push_back(new Weapon(WeaponType::Pistol));
	m_direction = Direction::East;

	m_position = sf::Vector2f(ScreenSize::s_width / 2.0f, ScreenSize::s_height / 2.0f);

	m_rectangle.setSize(sf::Vector2f(48.0f, 100.0f));
	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::White);
	m_rectangle.setPosition(m_position);

	sf::Texture& playerTexture = m_holder["starterAtlas"];
	sf::Texture& levelBarTexture = m_holder["starterAtlas"];

	m_playerSprite.setTexture(playerTexture);
	m_playerSprite.setTextureRect(IntRect{ 0,416,160,200 });
	m_playerSprite.setOrigin(80, 100);
	m_playerSprite.setScale(0.5f, 0.5f);
	m_playerSprite.setPosition(m_position);

	m_levelBarSprite.setTexture(levelBarTexture);
	m_levelBarSprite.setTextureRect(IntRect{ 0,616,500,32 });
	m_levelBarSprite.setOrigin(250, 16);
	m_levelBarSprite.setScale(2.25f, 2.0f);
	m_levelBarSprite.setPosition(800.0f, 40.0f);

	m_emptyHealthBar.setSize(sf::Vector2f(50.0f, 6.0f));
	m_emptyHealthBar.setOrigin(m_emptyHealthBar.getSize().x / 2.0f, m_emptyHealthBar.getSize().y / 2.0f);
	m_emptyHealthBar.setFillColor(sf::Color::Red);
	m_emptyHealthBar.setOutlineColor(sf::Color::Black);
	m_emptyHealthBar.setOutlineThickness(2.0f);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 35.0f);

	m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));
	m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
	m_currentHealthBar.setFillColor(sf::Color::Green);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 60.0f);

	m_emptyxphBar.setSize(sf::Vector2f(1000.0f, 20.0f));
	m_emptyxphBar.setOrigin(m_emptyxphBar.getSize().x / 2.0f, m_emptyxphBar.getSize().y / 2.0f);
	m_emptyxphBar.setFillColor(Color::Black);
	m_emptyxphBar.setPosition(800.0f, 40.0f);

	m_xpBar.setSize(sf::Vector2f(m_xp / m_xpRequired * 1000.0f, 20.0f));
	m_xpBar.setOrigin(500.0f, m_xpBar.getSize().y / 2.0f);
	m_xpBar.setFillColor(sf::Color::Green);
	m_xpBar.setPosition(800.0f, 40.0f);
}

Player::~Player()
{
}

void Player::update(double dt, std::vector<Enemy*> t_enemies)
{
	handleKeyInput();

	for (auto weapon : m_weapons)
	{
		weapon->update(dt, m_position, t_enemies, m_direction);
	}
	
	setHealth();
	setPosition(m_position.x, m_position.y);

	m_xpBar.setSize(sf::Vector2f(m_xp / m_xpRequired * 1000.0f, 20.0f));
	checkXP();
}

void Player::render(sf::RenderWindow& t_window)
{
	for (auto weapon : m_weapons)
	{
		weapon->render(t_window);
	}

	//t_window.draw(m_rectangle);
	t_window.draw(m_emptyHealthBar);
	t_window.draw(m_currentHealthBar);

	t_window.draw(m_playerSprite);

	t_window.draw(m_emptyxphBar);
	t_window.draw(m_xpBar);
	t_window.draw(m_levelBarSprite);
}

void Player::handleKeyInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_direction = Direction::West;
		m_playerSprite.setScale(-0.5f, 0.5f);
		m_position.x -= m_speed * m_speedModifier;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_direction = Direction::East;
		m_playerSprite.setScale(0.5f, 0.5f);
		m_position.x += m_speed * m_speedModifier;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		m_direction = Direction::North;
		m_position.y -= m_speed * m_speedModifier;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		m_direction = Direction::South;
		m_position.y += m_speed * m_speedModifier;
	}
}

void Player::setPosition(float t_x, float t_y)
{
	m_rectangle.setPosition(m_position);
	m_playerSprite.setPosition(m_position);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 60.0f);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 60.0f);
}

sf::Vector2f Player::getPosition()
{
	return m_position;
}

void Player::setHealth()
{
	if (m_health < 0)
	{
		m_health = 0;
	}
	else if (m_health > m_maxHealth)
	{
		m_health = m_maxHealth;
	}

	m_currentHealthBar.setSize(sf::Vector2f((m_health / m_maxHealth) * 50.0f, 6.0f));
}

void Player::decreaseHealth()
{
	m_health -= 1.0f * m_armorModifier;
}

void Player::increaseHealth()
{
	if (m_health < m_maxHealth)
	{
		m_health += 25.0f;
	}
}

void Player::increaseXP()
{
	m_xp += 2 * m_xpModifier;
}

void Player::checkXP()
{
	if (m_xp >= m_xpRequired)
	{
		m_level++;
		m_xpRequired *= 2;
		m_xp = 0;
	}
}

int Player::getLevel()
{
	return m_level;
}

sf::RectangleShape Player::getRectangle()
{
	return m_rectangle;
}

std::vector<Weapon*> Player::getWeapon()
{
	return m_weapons;
}

void Player::levelUp(Gamemode& t_gamemode)
{
	int playerChoice;
	std::string text1;
	std::string text2;
	std::string text3;

	PlayerUpgrades choice1 = static_cast<PlayerUpgrades>(rand() % static_cast<int>(PlayerUpgrades::Count)); // static_cast<int>(PlayerUpgrades::Count) will give the amount of items in the class
	PlayerUpgrades choice2 = static_cast<PlayerUpgrades>(rand() % static_cast<int>(PlayerUpgrades::Count));
	PlayerUpgrades choice3 = static_cast<PlayerUpgrades>(rand() % static_cast<int>(PlayerUpgrades::Count));

	while (choice2 == choice1)
	{
		choice2 = static_cast<PlayerUpgrades>(rand() % static_cast<int>(PlayerUpgrades::Count));
	}
	while (choice3 == choice1 || choice3 == choice2)
	{
		choice3 = static_cast<PlayerUpgrades>(rand() % static_cast<int>(PlayerUpgrades::Count));
	}

	std::cout << "Choose an upgrade:\n";
	
	switch (choice1)
	{
	case PlayerUpgrades::Health:
		text1 = "1. Health";
		break;
	case PlayerUpgrades::Speed:
		text1 = "1. Speed";
		break;
	case PlayerUpgrades::XP:
		text1 = "1. XP";
		break;
	case PlayerUpgrades::Armor:
		text1 = "1. Armor";
		break;
	default:
		break;
	}

	switch (choice2)
	{
	case PlayerUpgrades::Health:
		text2 = "2. Health";
		break;
	case PlayerUpgrades::Speed:
		text2 = "2. Speed";
		break;
	case PlayerUpgrades::XP:
		text2 = "2. XP";
		break;
	case PlayerUpgrades::Armor:
		text2 = "2. Armor";
		break;
	default:
		break;
	}

	switch (choice3)
	{
	case PlayerUpgrades::Health:
		text3 = "3. Health";
		break;
	case PlayerUpgrades::Speed:
		text3 = "3. Speed";
		break;
	case PlayerUpgrades::XP:
		text3 = "3. XP";
		break;
	case PlayerUpgrades::Armor:
		text3 = "3. Armor";
		break;
	default:
		break;
	}

	std::cout << text1 << "\n";
	std::cout << text2 << "\n";
	std::cout << text3 << "\n";

	std::cin >> playerChoice;

	switch (playerChoice)
	{
	case 1:
		upgradePlayer(choice1);
		break;
	case 2:
		upgradePlayer(choice2);
		break;
	case 3:
		upgradePlayer(choice3);
		break;
	default:
		break;
	}

	if (playerChoice != -1)
	{
		t_gamemode = Gamemode::Gameplay;
	}
}

void Player::upgradePlayer(PlayerUpgrades t_type)
{
	switch (t_type)
	{
	case PlayerUpgrades::Health:
		m_maxHealth += 50;
		//m_health += 50;
		break;
	case PlayerUpgrades::Speed:
		m_speedModifier += 0.5f;
		break;
	case PlayerUpgrades::XP:
		m_xpModifier += 0.5f;
		break;
	case PlayerUpgrades::Armor:
		m_armorModifier -= 0.1;
		break;
	default:
		break;
	}
}