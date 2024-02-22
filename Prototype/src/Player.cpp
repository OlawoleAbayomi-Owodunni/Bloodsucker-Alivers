#include "Player.h"
#include <iostream>
#include <10.0.22621.0/um/Windows.h>
#include <SFML/Window/Joystick.hpp>
#include <10.0.22621.0/um/Xinput.h>

//#pragma comment(lib, "XInput.lib") // Link XInput library

const float JOYSTICK_THRESHOLD = 50.0f; // Adjust as needed
// Define the rumble intensity (in the range [0, 65535])
const float RUMBLE_THRESHOLD = 16000;

Player::Player()
{
	m_health = 100.0f;
	m_speed = 2.0f;
	m_level = 1;
	m_xp = 0;
	m_xpRequired = 10.0f;
	m_levelUp = false;
	
	m_weapons.push_back(new Weapon(WeaponType::Pistol));
	m_direction = Direction::East;

	m_position = sf::Vector2f(ScreenSize::s_width / 2.0f, ScreenSize::s_height / 2.0f);

	m_rectangle.setSize(sf::Vector2f(40.0f, 40.0f));
	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::White);
	m_rectangle.setPosition(m_position);
	// Load a sprite to display
	//sf::Texture playerTexture;
	if (!m_playerTexture.loadFromFile("./resources/sprites/plyrPh.png"))
	{
		cout << "Failed to load file\n";
	}

	m_playerSprite.setTexture(m_playerTexture);
	m_playerSprite.setTextureRect(IntRect{ 0,0,32,32 });
	m_playerSprite.setOrigin(16, 16);
	m_playerSprite.setScale(5.0f, 5.0f);
	m_playerSprite.setPosition(m_position);

	m_emptyHealthBar.setSize(sf::Vector2f(50.0f, 6.0f));
	m_emptyHealthBar.setOrigin(m_emptyHealthBar.getSize().x / 2.0f, m_emptyHealthBar.getSize().y / 2.0f);
	m_emptyHealthBar.setFillColor(sf::Color::Red);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 35.0f);

	m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));
	m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
	m_currentHealthBar.setFillColor(sf::Color::Green);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 35.0f);

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

	t_window.draw(m_rectangle);
	t_window.draw(m_emptyHealthBar);
	t_window.draw(m_currentHealthBar);

	t_window.draw(m_playerSprite);

	t_window.draw(m_emptyxphBar);
	t_window.draw(m_xpBar);
}

#pragma region INPUT MANAGER
void Player::handleKeyInput()
{
	XINPUT_STATE state;
	memset(&state, 0, sizeof(XINPUT_STATE));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_direction = Direction::West;
		m_position.x -= m_speed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_direction = Direction::East;
		m_position.x += m_speed;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		m_direction = Direction::North;
		m_position.y -= m_speed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		m_direction = Direction::South;
		m_position.y += m_speed;
	}


	if (XInputGetState(0, &state) == ERROR_SUCCESS) {
		//float xAxis = state.Gamepad.sThumbLX;
		//float yAxis = state.Gamepad.sThumbLY;

		// Read input from the Xbox controller
		float xAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
		float yAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);

		// Map input to movement controls
		if (xAxis == 100) { m_direction = Direction::East; }
		else if (xAxis == -100) { m_direction = Direction::West; }
		else if (yAxis == 100) { m_direction = Direction::South; }
		else { m_direction = Direction::North; }

		cout << xAxis << "\n" << yAxis << "\n\n";
		if (std::abs(xAxis) > JOYSTICK_THRESHOLD) {
			m_position.x += (xAxis / 100) * m_speed;

			XINPUT_VIBRATION vibration;
			memset(&vibration, 0, sizeof(XINPUT_VIBRATION)); // Clear memory using memset
			vibration.wLeftMotorSpeed = RUMBLE_THRESHOLD;
			vibration.wRightMotorSpeed = RUMBLE_THRESHOLD;
			XInputSetState(0, &vibration);
		}
		if (std::abs(yAxis) > JOYSTICK_THRESHOLD) {
			m_position.y += (yAxis / 100) * m_speed;

			rumbleStart();
		}
		else {
			// Stop rumble if no movement
			rumbleStop();
		}
	}
}

void Player::rumbleStart()
{
	XINPUT_VIBRATION vibration;
	memset(&vibration, 0, sizeof(XINPUT_VIBRATION)); // Clear memory using memset
	vibration.wLeftMotorSpeed = RUMBLE_THRESHOLD;
	vibration.wRightMotorSpeed = RUMBLE_THRESHOLD;
	XInputSetState(0, &vibration);
}

void Player::rumbleStop()
{
	XINPUT_VIBRATION vibration;
	memset(&vibration, 0, sizeof(XINPUT_VIBRATION)); // Clear memory using memset
	XInputSetState(0, &vibration);
}
#pragma endregion


void Player::setPosition(float t_x, float t_y)
{
	m_rectangle.setPosition(m_position);
	m_playerSprite.setPosition(m_position);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y + 35.0f);
	m_currentHealthBar.setPosition(m_position.x, m_position.y + 35.0f);
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

	m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));
}

void Player::decreaseHealth()
{
	m_health -= 1.0f;
}

void Player::increaseXP()
{
	m_xp += 2;
}

void Player::checkXP()
{
	if (m_xp >= m_xpRequired)
	{
		m_level++;
		m_xpRequired *= 2;
		m_xp = 0;
		m_levelUp = true;
	}

	if (m_levelUp)
	{
		if (m_level == 2)
		{
			m_weapons.push_back(new Weapon(WeaponType::AssaultRifle));
		}
		m_levelUp = false;
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