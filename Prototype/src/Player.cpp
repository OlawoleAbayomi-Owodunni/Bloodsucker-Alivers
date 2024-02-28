#include "Player.h"
#include "Game.h"
#include <iostream>
#include "Windows.h"
#include <SFML/Window/Joystick.hpp>
#include "Xinput.h"

//#pragma comment(lib, "XInput.lib") // Link XInput library

const float JOYSTICK_THRESHOLD = 50.0f; // Adjust as needed
// Define the rumble intensity (in the range [0, 65535])
const float RUMBLE_THRESHOLD = 16000;

#pragma region CONSTRUCTOR
Player::Player()
{
	//THOR
	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));
	m_holder.acquire("mapSprite", thor::Resources::fromFile<sf::Texture>("resources/sprites/Map.png"));
	m_holder.acquire("playerAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/differentCharacterangle.png"));

	//SOUND
	if (!m_dashSoundBuffer.loadFromFile("resources/sounds/dash.wav"))
	{
		std::cout << "error loading dash sound";
	}
	m_dashSound.setBuffer(m_dashSoundBuffer);
	m_dashSound.setVolume(3.0f);

	//Base variable initialiser
	m_maxHealth = 100.0f;
	m_health = m_maxHealth;
	m_speed = 2.0f;
	m_level = 1;
	m_xp = 0;
	m_xpRequired = 10.0f;

	//Upgrade modifiers
	m_speedModifier = 1;
	m_xpModifier = 1;
	m_armorModifier = 1;

	m_weapons.push_back(new Weapon(WeaponType::AssaultRifle)); // all we need to do to player to add a new weapon

	//FSM setup
	m_direction = Direction::East;
	m_canDash = false;
	m_dashBarFillAmount = 0;
	
	m_playerState = CharacterState::IdleState;
	m_previousState = CharacterState::None;

	m_position = sf::Vector2f(ScreenSize::s_width / 2.0f, ScreenSize::s_height / 2.0f);

	for (int i = 0; i < 6; i++)
	{
		m_haloFrames.push_back(IntRect{ 160 * i,1351,160,64 });
	}
	m_currentHaloFrame = 0;

	m_playerTime = seconds(0.1f);
	m_haloTime = seconds(0.2f);
	m_dashStateTime = seconds(1.0f);
	m_dashCooldownTime = seconds(4.0f);

#pragma region SPRITES
	sf::Texture& playerTextures = m_holder["starterAtlas"];

	m_playerSprite.setTexture(playerTextures);
	m_playerSprite.setTextureRect(IntRect{ 0,416,160,203 });
	m_playerSprite.setOrigin(80.0f, 101.5f);
	m_playerSprite.setScale(0.5f, 0.5f);
	m_playerSprite.setPosition(m_position);

	m_dashSprite.setTexture(playerTextures);
	m_dashSprite.setTextureRect(IntRect{ 200, 1778, 200, 160 });
	m_dashSprite.setOrigin(100.0f, 80.0f);
	m_dashSprite.setScale(0.5f, 0.5f);
	m_dashSprite.setPosition(m_position);

	m_rectangle.setSize(sf::Vector2f(48.0f, 100.0f));
	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::White);
	m_rectangle.setPosition(m_position);

	//XP Bar sprite setup
	m_xpBarSprite.setTexture(playerTextures);
	m_xpBarSprite.setTextureRect(IntRect{ 0,626,500,32 });
	m_xpBarSprite.setOrigin(250, 16);
	m_xpBarSprite.setScale(2.25f, 2.0f);
	m_xpBarSprite.setPosition(800.0f, 40.0f);

	m_emptyXPBar.setSize(sf::Vector2f(1000.0f, 20.0f));
	m_emptyXPBar.setOrigin(m_emptyXPBar.getSize().x / 2.0f, m_emptyXPBar.getSize().y / 2.0f);
	m_emptyXPBar.setFillColor(Color::Black);
	m_emptyXPBar.setPosition(800.0f, 40.0f);

	m_xpBar.setSize(sf::Vector2f(m_xp / m_xpRequired * 1000.0f, 20.0f));
	m_xpBar.setOrigin(500.0f, m_xpBar.getSize().y / 2.0f);
	m_xpBar.setFillColor(sf::Color::Green);
	m_xpBar.setPosition(800.0f, 40.0f);

	//Dash Bar sprite setup
	m_dashBarSprite.setTexture(playerTextures);
	m_dashBarSprite.setTextureRect(IntRect{ 0, 1614, 34, 164 });
	m_dashBarSprite.setOrigin(17, 82);
	m_dashBarSprite.setScale(2.0f, 2.0f);
	m_dashBarSprite.setPosition(1480.0f, 680.0f);

	m_emptyDashBar.setSize(sf::Vector2f(24.0f, 244.0f));
	m_emptyDashBar.setOrigin(m_emptyDashBar.getSize().x / 2.0f, m_emptyDashBar.getSize().y / 2.0f);
	m_emptyDashBar.setFillColor(Color::Black);
	m_emptyDashBar.setPosition(1480.0f, 666.0f);

	m_dashBar.setSize(sf::Vector2f(24.0f, m_dashBarFillAmount));
	m_dashBar.setOrigin(m_dashBar.getSize().x / 2.0f, -122.0f);
	m_dashBar.setFillColor(sf::Color::White);
	m_dashBar.setPosition(1480.0f, 666.0f);

	//Halo Sprite setup
	m_haloSprite.setTexture(playerTextures);
	m_haloSprite.setTextureRect(IntRect{ 0,1350,160,64 });
	m_haloSprite.setOrigin(80, 32);
	m_haloSprite.setScale(0.6f, 0.6f);
	m_haloSprite.setPosition(sf::Vector2f(m_position.x, m_position.y + 42.0f));

	//Health bar setup
	m_emptyHealthBar.setSize(sf::Vector2f(50.0f, 6.0f));
	m_emptyHealthBar.setOrigin(m_emptyHealthBar.getSize().x / 2.0f, m_emptyHealthBar.getSize().y / 2.0f);
	m_emptyHealthBar.setFillColor(sf::Color::Red);
	m_emptyHealthBar.setOutlineColor(sf::Color::Black);
	m_emptyHealthBar.setOutlineThickness(2.0f);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y - 60.0f);

	m_currentHealthBar.setSize(sf::Vector2f(m_health / 2.0f, 6.0f));
	m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
	m_currentHealthBar.setFillColor(sf::Color::Green);
	m_currentHealthBar.setPosition(m_position.x, m_position.y - 60.0f);

	//Dash setup
	m_dashRect.setSize(sf::Vector2f(100.0f, 10.0f));
	m_dashRect.setOrigin(m_dashRect.getSize().x / 2.0f, m_dashRect.getSize().y / 2.0f);
	m_dashRect.setFillColor(sf::Color::White);
	m_dashRect.setPosition(-1000.0f, -1000.0f);
#pragma endregion

}

#pragma endregion


Player::~Player()
{
}

void Player::update(double dt, sf::View& t_view, std::vector<Enemy*> t_enemies)
{
	handleKeyInput();

	for (auto weapon : m_weapons)
	{
		weapon->update(dt, m_position, t_enemies, m_direction);
	}
	
	m_dashRectBounds.setFillColor(sf::Color::Transparent);
	m_dashRectBounds.setOutlineColor(sf::Color::Green);
	m_dashRectBounds.setOutlineThickness(2.0f);
	m_dashRectBounds.setOrigin(m_dashRect.getGlobalBounds().width / 2.0f, m_dashRect.getGlobalBounds().height / 2.0f);
	m_dashRectBounds.setPosition(m_dashRect.getPosition());
	m_dashRectBounds.setSize(sf::Vector2f(m_dashRect.getGlobalBounds().width, m_dashRect.getGlobalBounds().height));

	setHealth();
	setPosition(t_view);

	updateDashbar();
	checkXP();

	if (m_playerState != m_previousState)
	{
		setFrames();
	}
	animate();
}

void Player::render(sf::RenderWindow& t_window)
{
	for (auto weapon : m_weapons)
	{
		weapon->render(t_window);
	}

	const sf::Color baseColor = m_playerSprite.getColor();
	for (AfterImageData data : m_afterImages)
	{
		const float colorScale = data.m_lifetime * 255;
		const auto offsetColor = baseColor * sf::Color(255, 255, 255, colorScale);

		data.m_sprite.setPosition(data.m_position);
		data.m_sprite.setColor(offsetColor);
		t_window.draw(data.m_sprite);
	}

	// Dash collider and bounds checker
	/*t_window.draw(m_dashRect);
	t_window.draw(m_dashRectBounds);*/

	t_window.draw(m_haloSprite);

	//t_window.draw(m_rectangle);
	t_window.draw(m_emptyHealthBar);
	t_window.draw(m_currentHealthBar);

	t_window.draw(m_playerSprite);

	t_window.draw(m_emptyXPBar);
	t_window.draw(m_xpBar);
	t_window.draw(m_xpBarSprite);

	if (m_level > 1)
	{
		t_window.draw(m_emptyDashBar);
		t_window.draw(m_dashBar);
		t_window.draw(m_dashBarSprite);
	}
}

#pragma region INPUT MANAGER
void Player::handleKeyInput()
{
	XINPUT_STATE state;
	memset(&state, 0, sizeof(XINPUT_STATE));

	///// Movement
	// Keyboard
	m_movementVector = sf::Vector2f(0.0f, 0.0f);
	// Read input from the Xbox controller
	float xAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
	float yAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);

	if (m_dashStateClock.getElapsedTime() > m_dashStateTime)
	{
		m_playerState = CharacterState::None;
	}

	if (m_playerState != CharacterState::DashState)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || yAxis < -JOYSTICK_THRESHOLD)
		{
			m_direction = Direction::North;
			m_playerState = CharacterState::WalkUpState;
			m_playerSprite.setScale(0.5f, 0.5f);
			m_movementVector.y -= m_speed;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || yAxis > JOYSTICK_THRESHOLD)
		{
			m_direction = Direction::South;
			m_playerState = CharacterState::WalkDownState;
			m_playerSprite.setScale(0.5f, 0.5f);
			m_movementVector.y += m_speed;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || xAxis < -JOYSTICK_THRESHOLD)
		{
			m_direction = Direction::West;
			m_playerState = CharacterState::WalkSideState;
			m_playerSprite.setScale(-0.5f, 0.5f);
			m_dashSprite.setScale(-0.5f, 0.5f);
			m_movementVector.x -= m_speed;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || xAxis > JOYSTICK_THRESHOLD)
		{
			m_direction = Direction::East;
			m_playerState = CharacterState::WalkSideState;
			m_playerSprite.setScale(0.5f, 0.5f);
			m_dashSprite.setScale(0.5f, 0.5f);
			m_movementVector.x += m_speed;
		}
	}

	//	// Map input to movement controls
	//	if (xAxis == 100) { 
	//		m_direction = Direction::East; 
	//	}
	//	else if (xAxis == -100) { 
	//		m_direction = Direction::West;
	//	}
	//	else if (yAxis == 100) { 
	//		m_direction = Direction::South;
	//	}
	//	else { 
	//		m_direction = Direction::North;
	//	}

	//	//cout << xAxis << "\n" << yAxis << "\n\n";
	//	if (std::abs(xAxis) > JOYSTICK_THRESHOLD) {
	//		m_position.x += (xAxis / 100) * m_speed;
	//}

	m_position += m_movementVector * m_speedModifier;

	// Checking Idle state
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
		!sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
		!sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
		!sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
		abs(xAxis) < JOYSTICK_THRESHOLD && abs(yAxis) < JOYSTICK_THRESHOLD)
	{
		m_playerState = CharacterState::IdleState;
	}

	// Dash
	if (m_level > 1)
	{
		if (m_dashStateClock.getElapsedTime() < m_dashStateTime)
		{
			m_playerState = CharacterState::DashState;
		}

		/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (m_dashCooldownClock.getElapsedTime() > m_dashCooldownTime)
			{
				dash();
				m_dashCooldownClock.restart();
			}
		}
		else if (!m_canDash && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_canDash = true;
			m_dashRect.setPosition(-1000.0f,-1000.0f);
		}*/

		if (m_movementVector.x != 0.0f || m_movementVector.y != 0.0f)
		{
			if (m_dashCooldownClock.getElapsedTime() > m_dashCooldownTime)
			{
				dash();
				m_dashCooldownClock.restart();
			}
		}
		else if (!m_canDash && m_movementVector.x == 0.0f && m_movementVector.y == 0.0f)
		{
			m_canDash = true;
			m_dashRect.setPosition(-1000.0f, -1000.0f);
		}

		for (int i = 0; i < m_afterImages.size(); i++)
		{
			AfterImageData& data = m_afterImages[i];

			data.m_lifetime -= 0.016f;

			if (data.m_lifetime < 0.0f)
			{
				m_afterImages.erase(m_afterImages.begin() + i);
				i--;
			}
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

#pragma region GETTERS & SETTERS
#pragma region Getters
sf::Vector2f Player::getPosition()
{
	return m_position;
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

sf::RectangleShape Player::getDashCollider()
{
	return m_dashRect;
}

#pragma endregion

#pragma region Setters
void Player::setPosition(sf::View& t_view)
{
	sf::Vector2f cameraPos(t_view.getCenter());

	m_rectangle.setPosition(m_position);
	m_playerSprite.setPosition(m_position);

	m_emptyHealthBar.setPosition(m_position.x, m_position.y - 60.0f);
	m_currentHealthBar.setPosition(m_position.x, m_position.y - 60.0f);

	m_xpBar.setPosition(cameraPos.x, cameraPos.y - 410.0F);
	m_emptyXPBar.setPosition(cameraPos.x, cameraPos.y - 410.0F);
	m_xpBarSprite.setPosition(cameraPos.x, cameraPos.y - 410.0f);

	m_dashBarSprite.setPosition(cameraPos.x + 680.0f, cameraPos.y + 230.0f);
	m_emptyDashBar.setPosition(cameraPos.x + 680.0f, cameraPos.y + 216.0f);
	m_dashBar.setPosition(cameraPos.x + 680.0f, cameraPos.y + 216.0f);

	m_haloSprite.setPosition(sf::Vector2f(m_position.x, m_position.y + 42.0f));
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


#pragma endregion

#pragma endregion

#pragma region UPGRADES
#pragma region Player Upgrades
void Player::upgradePlayer(PlayerUpgrade t_type)
{
	switch (t_type)
	{
	case PlayerUpgrade::Health:
		m_maxHealth += 50;
		//m_health += 50;
		break;
	case PlayerUpgrade::Speed:
		m_speedModifier += 0.5f;
		break;
	case PlayerUpgrade::XP:
		m_xpModifier += 0.5f;
		break;
	case PlayerUpgrade::Armor:
		m_armorModifier -= 0.1;
		break;
	default:
		break;
	}
}

void Player::upgradeGun(WeaponType t_type)
{
	switch (t_type)
	{
	case WeaponType::Pistol:
		for (int i = 0; i < m_weapons.size(); i++) {
			if (m_weapons[i]->getType() == t_type) {
				m_weapons[i]->upgradeWeapon(t_type);
				cout << "Weapon Actually upgraded\n";
			}
		}
		break;
	case WeaponType::AssaultRifle:
		for (int i = 0; i < m_weapons.size(); i++) {
			if (m_weapons[i]->getType() == t_type) {
				m_weapons[i]->upgradeWeapon(t_type);
				cout << "Weapon Actually upgraded\n";
			}
		}
	}
}

void Player::giveWeapon(WeaponType t_type)
{
	bool weaponEquipped = false;

	switch (t_type)
	{
	case WeaponType::Pistol:
		for (auto weapon : m_weapons)
		{
			if (weapon->getType() == t_type) {
				weaponEquipped = true;
			}
		}
		
		if (weaponEquipped) {
			cout << "You already have that weapon equipped.\n";
		}
		else {
			m_weapons.push_back(new Weapon(WeaponType::Pistol)); // all we need to do to player to add a new weapon
			cout << "Pistol added to arsenal.\n";
		}
		break;
	case WeaponType::AssaultRifle:

		for (auto weapon : m_weapons)
		{
			if (weapon->getType() == t_type) {
				weaponEquipped = true;
			}

		}

		if (weaponEquipped) {
			cout << "You already have that weapon equipped.\n";
		}
		else {
			m_weapons.push_back(new Weapon(WeaponType::AssaultRifle)); // all we need to do to player to add a new weapon
			cout << "Assault Rifle added to arsenal.\n";
		}
		break;
	default:
		break;
	}
}

#pragma endregion

#pragma endregion


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

void Player::dash()
{
	if (m_canDash)
	{
		m_canDash = false;
		m_dashStateClock.restart();

		m_dashSound.stop();
		m_dashSound.play();

		sf::Vector2f heading = m_movementVector * DASH_DISTANCE;

		for (unsigned int i = 0; i < AFTERIMAGE_COUNT; i++)
		{
			AfterImageData data{
				m_position + (m_movementVector * (DASH_DISTANCE * (static_cast<float>(i) / AFTERIMAGE_COUNT))),
				static_cast<float>(i) / AFTERIMAGE_COUNT, m_dashSprite
			};

			m_afterImages.push_back(data);
		}

		float distance = std::sqrtf(heading.x * heading.x + heading.y * heading.y);
		float dashAngle = atan2(heading.y, heading.x) * (180.0f / 3.14159);

		if (abs(static_cast<int>(dashAngle)) % 90 == 0)
		{
			m_dashRect.setSize(sf::Vector2f(distance, 60.0f));
		}
		else
		{
			m_dashRect.setSize(sf::Vector2f(distance, 10.0f));
		}

		m_dashRect.setOrigin(distance / 2.0f, m_dashRect.getSize().y / 2.0f);
		m_dashRect.setRotation(dashAngle);
		m_dashRect.setPosition(m_position.x + heading.x / 2.0f, m_position.y + heading.y / 2.0f);

		m_position += heading;
	}
}

void Player::updateDashbar()
{
	m_dashBarFillAmount = -(m_dashCooldownClock.getElapsedTime().asSeconds() / m_dashCooldownTime.asSeconds() * 244.0f);

	if (m_dashBarFillAmount <= -244.0f)
	{
		m_dashBarFillAmount = -244.0f;
		m_dashBar.setFillColor(sf::Color::White);
	}
	else
	{
		m_dashBar.setFillColor(sf::Color::Red);
	}

	m_dashBar.setSize(sf::Vector2f(24.0f, m_dashBarFillAmount));
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
	m_xpBar.setSize(sf::Vector2f(m_xp / m_xpRequired * 1000.0f, 20.0f));
}

#pragma region FSM
void Player::animate()
{
	if (m_playerClock.getElapsedTime() > m_playerTime)
	{
		if (m_currentPlayerFrame + 1 < m_playerFrames.size())
		{
			m_currentPlayerFrame++;
		}
		else
		{
			m_currentPlayerFrame = 0;
		}
		m_playerClock.restart();
	}

	m_playerSprite.setTextureRect(m_playerFrames[m_currentPlayerFrame]);
	m_previousState = m_playerState;

	if (m_haloClock.getElapsedTime() > m_haloTime)
	{
		if (m_currentHaloFrame + 1 < m_haloFrames.size())
		{
			m_currentHaloFrame++;
		}
		else
		{
			m_currentHaloFrame = 0;
		}
		m_haloClock.restart();
	}

	m_haloSprite.setTextureRect(m_haloFrames[m_currentHaloFrame]);
}

void Player::addFrame(sf::IntRect& t_frame)
{
	m_playerFrames.push_back(t_frame);
}

void Player::setFrames()
{
	m_playerFrames.clear();
	m_currentPlayerFrame = 0;

	switch (m_playerState)
	{
	case CharacterState::IdleState:
		addFrame(IntRect{ 0,744,160,203 });
		addFrame(IntRect{ 160,744,160,203 });
		addFrame(IntRect{ 320,744,160,203 });
		addFrame(IntRect{ 480,744,160,203 });
		addFrame(IntRect{ 640,744,160,203 });
		addFrame(IntRect{ 800,744,160,203 });
		addFrame(IntRect{ 960,744,160,203 });
		addFrame(IntRect{ 1120,744,160,203 });
		addFrame(IntRect{ 1280,744,160,203 });

		addFrame(IntRect{ 0,947,160,203 });
		addFrame(IntRect{ 160,947,160,203 });
		addFrame(IntRect{ 320,947,160,203 });
		addFrame(IntRect{ 480,947,160,203 });
		addFrame(IntRect{ 640,947,160,203 });
		addFrame(IntRect{ 800,947,160,203 });
		addFrame(IntRect{ 960,947,160,203 });
		addFrame(IntRect{ 1120,947,160,203 });
		addFrame(IntRect{ 1280,947,160,203 });

		addFrame(IntRect{ 0,1150,160,203 });
		addFrame(IntRect{ 160,1150,160,203 });
		addFrame(IntRect{ 320,1150,160,203 });
		addFrame(IntRect{ 480,1150,160,203 });
		addFrame(IntRect{ 640,1150,160,203 });
		m_playerSprite.setOrigin(80.0f, 101.5f);
		break;
	case CharacterState::WalkSideState:
		addFrame(IntRect{ 160,416,160,203 });
		addFrame(IntRect{ 320,416,160,203 });
		addFrame(IntRect{ 480,416,160,203 });
		addFrame(IntRect{ 640,416,160,203 });
		addFrame(IntRect{ 800,416,160,203 });
		addFrame(IntRect{ 960,416,160,203 });
		addFrame(IntRect{ 1120,416,160,203 });
		addFrame(IntRect{ 1280,416,160,203 });
		m_playerSprite.setOrigin(80.0f, 101.5f);
		break;
	case CharacterState::WalkDownState:
		addFrame(IntRect{ 0, 1408, 160, 200 });
		addFrame(IntRect{ 160, 1408, 160, 200 });
		addFrame(IntRect{ 320, 1408, 160, 200 });
		addFrame(IntRect{ 480, 1408, 160, 200 });
		addFrame(IntRect{ 640, 1408, 160, 200 });
		addFrame(IntRect{ 800, 1408, 160, 200 });
		addFrame(IntRect{ 960, 1408, 160, 200 });
		addFrame(IntRect{ 1120, 1408, 160, 200 });
		addFrame(IntRect{ 1280, 1408, 160, 200 });
		addFrame(IntRect{ 1440, 1408, 160, 200 });
		addFrame(IntRect{ 1600, 1408, 160, 200 });
		addFrame(IntRect{ 1760, 1408, 160, 200 });
		addFrame(IntRect{ 1920, 1408, 160, 200 });
		m_playerSprite.setOrigin(80.0f, 100.0f);
		break;
	case CharacterState::WalkUpState:
		addFrame(IntRect{ 0, 1938, 160, 200 });
		addFrame(IntRect{ 160, 1938, 160, 200 });
		addFrame(IntRect{ 320, 1938, 160, 200 });
		addFrame(IntRect{ 480, 1938, 160, 200 });
		addFrame(IntRect{ 640, 1938, 160, 200 });
		addFrame(IntRect{ 800, 1938, 160, 200 });
		addFrame(IntRect{ 960, 1938, 160, 200 });
		m_playerSprite.setOrigin(80.0f, 100.0f);
		break;
	case CharacterState::DashState:
		addFrame(IntRect{ 0, 1778, 200, 160 });
		m_playerSprite.setOrigin(100.0f, 60.0f);
		break;
	default:
		break;
	}
}

#pragma endregion


void Player::playSound(sf::Sound& t_sound)
{
	t_sound.play();
}