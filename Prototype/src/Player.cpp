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

	m_direction = Direction::East;
	m_canDash = false;

	m_weapons.push_back(new Weapon(WeaponType::Pistol));
	
	m_playerState = CharacterState::IdleState;
	m_previousState = CharacterState::None;

	m_position = sf::Vector2f(ScreenSize::s_width / 2.0f, ScreenSize::s_height / 2.0f);

	for (int i = 0; i < 6; i++)
	{
		m_haloFrames.push_back(IntRect{ 160 * i,1350,160,64 });
	}
	m_currentHaloFrame = 0;

	m_playerTime = seconds(0.1f);
	m_haloTime = seconds(0.2f);

	m_rectangle.setSize(sf::Vector2f(48.0f, 100.0f));
	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::White);
	m_rectangle.setPosition(m_position);

	sf::Texture& playerTextures = m_holder["starterAtlas"];

	m_playerSprite.setTexture(playerTextures);
	m_playerSprite.setTextureRect(IntRect{ 0,416,160,203 });
	m_playerSprite.setOrigin(80.0f, 101.5f);
	m_playerSprite.setScale(0.5f, 0.5f);
	m_playerSprite.setPosition(m_position);

	m_xpBarSprite.setTexture(playerTextures);
	m_xpBarSprite.setTextureRect(IntRect{ 0,626,500,32 });
	m_xpBarSprite.setOrigin(250, 16);
	m_xpBarSprite.setScale(2.25f, 2.0f);
	m_xpBarSprite.setPosition(800.0f, 40.0f);

	m_haloSprite.setTexture(playerTextures);
	m_haloSprite.setTextureRect(IntRect{ 0,1350,160,64 });
	m_haloSprite.setOrigin(80, 32);
	m_haloSprite.setScale(0.6f, 0.6f);
	m_haloSprite.setPosition(sf::Vector2f(m_position.x, m_position.y + 42.0f));

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

	m_emptyXPBar.setSize(sf::Vector2f(1000.0f, 20.0f));
	m_emptyXPBar.setOrigin(m_emptyXPBar.getSize().x / 2.0f, m_emptyXPBar.getSize().y / 2.0f);
	m_emptyXPBar.setFillColor(Color::Black);
	m_emptyXPBar.setPosition(800.0f, 40.0f);

	m_xpBar.setSize(sf::Vector2f(m_xp / m_xpRequired * 1000.0f, 20.0f));
	m_xpBar.setOrigin(500.0f, m_xpBar.getSize().y / 2.0f);
	m_xpBar.setFillColor(sf::Color::Green);
	m_xpBar.setPosition(800.0f, 40.0f);

	m_dashRect.setSize(sf::Vector2f(100.0f, 10.0f));
	m_dashRect.setOrigin(m_dashRect.getSize().x / 2.0f, m_dashRect.getSize().y / 2.0f);
	m_dashRect.setFillColor(sf::Color::White);
	m_dashRect.setPosition(-1000.0f,-1000.0f);
}

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
}

#pragma region INPUT MANAGER
void Player::handleKeyInput()
{
	XINPUT_STATE state;
	memset(&state, 0, sizeof(XINPUT_STATE));

	///// Movement
	// Keyboard
	m_movementVector = sf::Vector2f(0.0f, 0.0f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		m_direction = Direction::West;
		m_playerState = CharacterState::WalkState;
		m_playerSprite.setScale(-0.5f, 0.5f);
		m_movementVector.x -= m_speed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		m_direction = Direction::East;
		m_playerState = CharacterState::WalkState;
		m_playerSprite.setScale(0.5f, 0.5f);
		m_movementVector.x += m_speed;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		m_direction = Direction::North;
		m_playerState = CharacterState::WalkState;
		m_movementVector.y -= m_speed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		m_direction = Direction::South;
		m_playerState = CharacterState::WalkState;
		m_movementVector.y += m_speed;
	}

	// Controller
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

		//cout << xAxis << "\n" << yAxis << "\n\n";
		if (std::abs(xAxis) > JOYSTICK_THRESHOLD) {
			m_movementVector.x += (xAxis / 100) * m_speed;

			rumbleStart();
		}
		if (std::abs(yAxis) > JOYSTICK_THRESHOLD) {
			m_movementVector.y += (yAxis / 100) * m_speed;

			rumbleStart();
		}
		else {
			// Stop rumble if no movement
			rumbleStop();
		}
	}

	m_position += m_movementVector * m_speedModifier;

	// Checking Idle state
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
		!sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
		!sf::Keyboard::isKeyPressed(sf::Keyboard::Up) &&
		!sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		m_playerState = CharacterState::IdleState;
	}

	// Dash
	if (m_level > 1)
	{
		if (m_canDash && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_canDash = false;
			sf::Vector2f heading = m_movementVector * DASH_DISTANCE;

			for (unsigned int i = 0; i < AFTERIMAGE_COUNT; i++)
			{
				AfterImageData data{
					m_position + (m_movementVector * (DASH_DISTANCE * (static_cast<float>(i) / AFTERIMAGE_COUNT))),
					static_cast<float>(i) / AFTERIMAGE_COUNT, m_playerSprite
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
		else if (!m_canDash && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_canDash = true;
			m_dashRect.setPosition(-1000.0f,-1000.0f);
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

	m_haloSprite.setPosition(sf::Vector2f(m_position.x, m_position.y + 42.0f));
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
	float fillAmount = m_xp / m_xpRequired;

	m_xpBar.setSize(sf::Vector2f(fillAmount * 1000.0f, 20.0f));

	if (m_xp >= m_xpRequired)
	{
		m_level++;
		m_xpRequired *= 2;
		m_xp = 0;

		if (m_level == 2)
		{
			m_canDash = true;
		}
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

sf::RectangleShape Player::getDashCollider()
{
	return m_dashRect;
}

void Player::levelUp(Gamemode& t_gamemode)
{
	int playerChoice;
	std::string text1;
	std::string text2;
	std::string text3;

	PlayerUpgrade choice1 = static_cast<PlayerUpgrade>(rand() % static_cast<int>(PlayerUpgrade::Count)); // static_cast<int>(PlayerUpgrades::Count) will give the amount of items in the class
	PlayerUpgrade choice2 = static_cast<PlayerUpgrade>(rand() % static_cast<int>(PlayerUpgrade::Count));
	PlayerUpgrade choice3 = static_cast<PlayerUpgrade>(rand() % static_cast<int>(PlayerUpgrade::Count));

	while (choice2 == choice1)
	{
		choice2 = static_cast<PlayerUpgrade>(rand() % static_cast<int>(PlayerUpgrade::Count));
	}
	while (choice3 == choice1 || choice3 == choice2)
	{
		choice3 = static_cast<PlayerUpgrade>(rand() % static_cast<int>(PlayerUpgrade::Count));
	}

	std::cout << "Choose an upgrade:\n";
	
	switch (choice1)
	{
	case PlayerUpgrade::Health:
		text1 = "1. Health";
		break;
	case PlayerUpgrade::Speed:
		text1 = "1. Speed";
		break;
	case PlayerUpgrade::XP:
		text1 = "1. XP";
		break;
	case PlayerUpgrade::Armor:
		text1 = "1. Armor";
		break;
	default:
		break;
	}

	switch (choice2)
	{
	case PlayerUpgrade::Health:
		text2 = "2. Health";
		break;
	case PlayerUpgrade::Speed:
		text2 = "2. Speed";
		break;
	case PlayerUpgrade::XP:
		text2 = "2. XP";
		break;
	case PlayerUpgrade::Armor:
		text2 = "2. Armor";
		break;
	default:
		break;
	}

	switch (choice3)
	{
	case PlayerUpgrade::Health:
		text3 = "3. Health";
		break;
	case PlayerUpgrade::Speed:
		text3 = "3. Speed";
		break;
	case PlayerUpgrade::XP:
		text3 = "3. XP";
		break;
	case PlayerUpgrade::Armor:
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
		break;
	case CharacterState::WalkState:
		addFrame(IntRect{ 160,416,160,203 });
		addFrame(IntRect{ 320,416,160,203 });
		addFrame(IntRect{ 480,416,160,203 });
		addFrame(IntRect{ 640,416,160,203 });
		addFrame(IntRect{ 800,416,160,203 });
		addFrame(IntRect{ 960,416,160,203 });
		addFrame(IntRect{ 1120,416,160,203 });
		addFrame(IntRect{ 1280,416,160,203 });
		break;
	default:
		break;
	}
}

void Player::playSound(sf::Sound& t_sound)
{
	t_sound.play();
}