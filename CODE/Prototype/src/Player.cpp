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
const float STRONG_RUMBLE_THRESHOLD = 32000;
const float WEAK_RUMBLE_THRESHOLD = 10000;

#pragma region CONSTRUCTOR
Player::Player()
{
	//THOR
	m_holder.acquire("starterAtlas", thor::Resources::fromFile<sf::Texture>("resources/sprites/StarterAtlas.png"));

	//SOUND
	if (!m_dashSoundBuffer.loadFromFile("resources/sounds/dash.wav"))
	{
		std::cout << "error loading dash sound";
	}
	m_dashSound.setBuffer(m_dashSoundBuffer);
	m_dashSound.setVolume(3.0f);

	if (!m_deathSoundBuffer.loadFromFile("resources/sounds/player_death.wav"))
	{
		std::cout << "error loading death sound";
	}
	m_deathSound.setBuffer(m_deathSoundBuffer);
	m_deathSound.setVolume(5.0f);

	//FONT & TEXT
	if (!m_font.loadFromFile("resources/BebasNeue.otf"))
	{
		std::cout << "Error loading font file";
	}

	//Base variable initialiser
	isPlayerAlive = true;
	isPistolEquipped = true;
	isAREquipped = false;
	isSniperEquipped = false;
	isRPGEquipped = false;
	m_maxHealth = 200.0f;
	m_health = m_maxHealth;
	m_speed = 2.0f;
	m_level = 1;
	m_xp = 0;
	m_xpRequired = 10.0f;

	//Upgrade modifiers
	m_speedModifier = 1.0f;
	m_xpModifier = 1.0f;
	m_armorModifier = 1.0f;
	m_magnetModifier = 1.0f;

	m_weapons.push_back(new Weapon(WeaponType::Pistol, m_holder["starterAtlas"])); // all we need to do to player to add a new weapon

	//FSM setup
	m_direction = Direction::East;
	m_canDash = false;
	m_dashBarFillAmount = 0;
	m_maxDashCharges = 0;
	m_currentDashCharges = 0;
	m_dashDistance = 150.0f;
	m_dashHasAOE = false;
	m_magnetDistance = 100.0f;

	m_playerState = CharacterState::IdleState;
	m_previousState = CharacterState::None;

	m_position = sf::Vector2f(ScreenSize::s_width / 2.0f, ScreenSize::s_height / 2.0f);
	m_previousPosition = m_position;

	for (int i = 0; i < 6; i++)
	{
		m_haloFrames.push_back(IntRect{ 160 * i,1351,160,64 });
	}
	for (int i = 0; i < 7; i++)
	{
		m_slashFrames.push_back(IntRect{ 1712 + 400 * i, 464, 400, 200 });
	}

	m_currentPlayerFrame = 0;
	m_currentHaloFrame = 0;
	m_currentSlashFrame = 0;

	m_playerTime = seconds(0.1f);
	m_haloTime = seconds(0.2f);
	m_slashTime = seconds(0.05f);

	m_dashStateTime = seconds(1.0f);
	m_dashCooldownTime = seconds(4.0f);
	m_dashAOETime = seconds(0.5f);

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

	m_slashSprite.setTexture(playerTextures);
	m_slashSprite.setTextureRect(IntRect{ 1560, 484, 400, 200 });
	m_slashSprite.setOrigin(200.0f, 100.0f);
	m_slashSprite.setScale(1.0f, 1.0f);
	m_slashSprite.setPosition(-1000.0f, -1000.0f);

	m_rectangle.setSize(sf::Vector2f(48.0f, 60.0f));
	m_rectangle.setOrigin(m_rectangle.getSize().x / 2.0f, m_rectangle.getSize().y / 2.0f);
	m_rectangle.setFillColor(sf::Color::White);
	m_rectangle.setPosition(m_position.x, m_position.y + 20.0f);

	m_slashCircle.setRadius(130.0f);
	m_slashCircle.setOrigin(130.0f, 130.0f);
	m_slashCircle.setFillColor(sf::Color::Red);
	m_slashCircle.setPosition(-1000.0f, -1000.0f);

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
	m_dashBarLeftSprite.setTexture(playerTextures);
	m_dashBarLeftSprite.setTextureRect(IntRect{ 0, 1614, 34, 164 });
	m_dashBarLeftSprite.setOrigin(17, 82);
	m_dashBarLeftSprite.setScale(2.0f, 2.0f);
	m_dashBarLeftSprite.setPosition(1480.0f, 680.0f);
	m_dashBarLeftSprite.setRotation(270.0f);

	m_emptyDashBarLeft.setSize(sf::Vector2f(24.0f, 244.0f));
	m_emptyDashBarLeft.setOrigin(m_emptyDashBarLeft.getSize().x / 2.0f, 136);
	m_emptyDashBarLeft.setFillColor(Color::Black);
	m_emptyDashBarLeft.setPosition(1480.0f, 680.0f);
	m_emptyDashBarLeft.setRotation(270.0f);

	m_dashBarLeft.setSize(sf::Vector2f(24.0f, m_dashBarFillAmount));
	m_dashBarLeft.setOrigin(m_dashBarLeft.getSize().x / 2.0f, -108.0f);
	m_dashBarLeft.setFillColor(sf::Color::White);
	m_dashBarLeft.setPosition(1480.0f, 680.0f);
	m_dashBarLeft.setRotation(270.0f);

	m_dashBarRightSprite.setTexture(playerTextures);
	m_dashBarRightSprite.setTextureRect(IntRect{ 0, 1614, 34, 164 });
	m_dashBarRightSprite.setOrigin(17, 82);
	m_dashBarRightSprite.setScale(2.0f, 2.0f);
	m_dashBarRightSprite.setPosition(1480.0f, 680.0f);
	m_dashBarRightSprite.setRotation(90.0f);

	m_emptyDashBarRight.setSize(sf::Vector2f(24.0f, 244.0f));
	m_emptyDashBarRight.setOrigin(m_emptyDashBarLeft.getSize().x / 2.0f, 136);
	m_emptyDashBarRight.setFillColor(Color::Black);
	m_emptyDashBarRight.setPosition(1480.0f, 680.0f);
	m_emptyDashBarRight.setRotation(90.0f);

	m_dashBarRight.setSize(sf::Vector2f(24.0f, m_dashBarFillAmount));
	m_dashBarRight.setOrigin(m_dashBarLeft.getSize().x / 2.0f, -108.0f);
	m_dashBarRight.setFillColor(sf::Color::White);
	m_dashBarRight.setPosition(1480.0f, 680.0f);
	m_dashBarRight.setRotation(90.0f);

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

	m_currentHealthBar.setSize(sf::Vector2f(m_health / m_maxHealth * 50.0f, 6.0f));
	m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
	m_currentHealthBar.setFillColor(sf::Color::Green);
	m_currentHealthBar.setPosition(m_position.x, m_position.y - 60.0f);

	//Dash setup
	m_dashRect.setSize(sf::Vector2f(100.0f, 10.0f));
	m_dashRect.setOrigin(m_dashRect.getSize().x / 2.0f, m_dashRect.getSize().y / 2.0f);
	m_dashRect.setFillColor(sf::Color::White);
	m_dashRect.setPosition(-1000.0f, -1000.0f);

	//XP Bar Text
	m_xpBarText.setFont(m_font);
	m_xpBarText.setCharacterSize(18.0f);
	m_xpBarText.setFillColor(sf::Color::White);
	m_xpBarText.setOutlineColor(sf::Color::Black);
	m_xpBarText.setOutlineThickness(2.0f);
	m_xpBarText.setString(std::to_string(static_cast<int>(m_xp)) + " / " + std::to_string(static_cast<int>(m_xpRequired)));
	m_xpBarText.setOrigin(m_xpBarText.getGlobalBounds().width / 2.0f, m_xpBarText.getGlobalBounds().height / 2.0f);
	m_xpBarText.setPosition(800.0f, 40.0f);
#pragma endregion

	score = 0;
	smallEK = 0;
	normalEK = 0;
	bigEK = 0;
	bossEK = 0;
	timeSurvived = 0;
}

#pragma endregion


Player::~Player()
{
}

void Player::initialise()
{
	isPlayerAlive = true;
	isPistolEquipped = true;
	isAREquipped = false;
	isSniperEquipped = false;
	isRPGEquipped = false;

	m_maxHealth = 400.0f;
	m_health = m_maxHealth;
	m_speed = 2.0f;
	m_level = 1;
	m_xp = 0;
	m_xpRequired = 10.0f;

	m_position = sf::Vector2f(ScreenSize::s_width / 2.0f, ScreenSize::s_height / 2.0f);
	m_previousPosition = m_position;
	m_movementVector = sf::Vector2f(0.0f, 0.0f);

	m_speedModifier = 1.0f;
	m_xpModifier = 1.0f;
	m_armorModifier = 1.0f;
	m_magnetModifier = 1.0f;

	m_direction = Direction::East;
	m_canDash = false;
	m_dashBarFillAmount = 0;
	m_maxDashCharges = 0;
	m_currentDashCharges = 0;
	m_dashDistance = 150.0f;
	m_dashHasAOE = false;

	m_playerState = CharacterState::IdleState;
	m_previousState = CharacterState::None;

	m_afterImages.clear();

	m_dashCooldownClock.restart();
	//Time m_dashCooldownTime; couldn't find initialiser

	//m_weapons.clear();
	for (auto it = m_weapons.begin(); it != m_weapons.end();)
	{
		if (*it)
		{
			delete* it; // Delete the object
		}
		it = m_weapons.erase(it); // Remove the pointer from the vector
	}

	m_weapons.push_back(new Weapon(WeaponType::Pistol, m_holder["starterAtlas"])); // all we need to do to player to add a new weapon

	m_currentPlayerFrame = 0;
	m_currentHaloFrame = 0;
	m_currentSlashFrame = 0;

	m_playerTime = seconds(0.1f);
	m_haloTime = seconds(0.2f);
	m_slashTime = seconds(0.05f);

	m_dashStateTime = seconds(0.6f);
	m_dashCooldownTime = seconds(4.0f);
	m_dashAOETime = seconds(0.1f);

	//RUMBLE TIMERS
	isDashRumbling = false;
	dashRumbleTimer.restart();

#pragma region Sprite Initialiser
	//SPRITE INITIALISER
	m_playerSprite.setPosition(m_position);

	m_rectangle.setPosition(m_position.x, m_position.y + 20.0f);

	m_emptyHealthBar.setPosition(m_position.x, m_position.y - 60.0f);
	m_currentHealthBar.setPosition(m_position.x, m_position.y - 60.0f);

	m_xpBar.setSize(sf::Vector2f(m_xp / m_xpRequired * 1000.0f, 20.0f));
	m_xpBar.setOrigin(500.0f, m_xpBar.getSize().y / 2.0f);
	m_xpBar.setPosition(800.0f, 40.0f);
	m_emptyXPBar.setPosition(800.0f, 40.0f);
	m_xpBarSprite.setPosition(800.0f, 40.0f);

	m_dashRect.setPosition(-1000.0f, -1000.0f);
	m_dashRectBounds.setPosition(m_dashRect.getPosition());

	m_dashSprite.setPosition(m_position);
	m_slashSprite.setPosition(-1000.0f, -1000.0f);
	m_slashCircle.setPosition(-1000.0f, -1000.0f);

	//Dash Bar sprite setup
	m_dashBarLeftSprite.setPosition(1480.0f, 680.0f);

	m_emptyDashBarLeft.setOrigin(m_emptyDashBarLeft.getSize().x / 2.0f, 136);
	m_emptyDashBarLeft.setPosition(1480.0f, 680.0f);

	m_dashBarLeft.setSize(sf::Vector2f(24.0f, m_dashBarFillAmount));
	m_dashBarLeft.setOrigin(m_dashBarLeft.getSize().x / 2.0f, -108.0f);
	m_dashBarLeft.setPosition(1480.0f, 680.0f);

	m_dashBarRightSprite.setPosition(1480.0f, 680.0f);

	m_emptyDashBarRight.setOrigin(m_emptyDashBarLeft.getSize().x / 2.0f, 136);
	m_emptyDashBarRight.setPosition(1480.0f, 680.0f);

	m_dashBarRight.setSize(sf::Vector2f(24.0f, m_dashBarFillAmount));
	m_dashBarRight.setOrigin(m_dashBarLeft.getSize().x / 2.0f, -108.0f);
	m_dashBarRight.setPosition(1480.0f, 680.0f);

	//Halo Sprite setup
	m_haloSprite.setPosition(sf::Vector2f(m_position.x, m_position.y + 42.0f));

	//Health bar setup
	m_emptyHealthBar.setOrigin(m_emptyHealthBar.getSize().x / 2.0f, m_emptyHealthBar.getSize().y / 2.0f);
	m_emptyHealthBar.setPosition(m_position.x, m_position.y - 60.0f);

	m_currentHealthBar.setSize(sf::Vector2f(m_health / m_maxHealth * 50.0f, 6.0f));
	m_currentHealthBar.setOrigin(m_currentHealthBar.getSize().x / 2.0f, m_currentHealthBar.getSize().y / 2.0f);
	m_currentHealthBar.setPosition(m_position.x, m_position.y - 60.0f);

	//Dash setup
	m_dashRect.setOrigin(m_dashRect.getSize().x / 2.0f, m_dashRect.getSize().y / 2.0f);
	m_dashRect.setPosition(-1000.0f, -1000.0f);
#pragma endregion


}

void Player::update(double dt, sf::View& t_view, std::vector<Enemy*> t_enemies)
{
	if (m_playerState != CharacterState::DeadState)
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

		if (m_damageClock.getElapsedTime().asSeconds() > 0.1f &&
			m_healClock.getElapsedTime().asSeconds() > 0.3f)
		{
			m_playerSprite.setColor(sf::Color::White);
		}

		setHealth();
		setPosition(t_view);

		updateDashbar();
		checkXP();
	}

	if (m_playerState != m_previousState)
	{
		setFrames();
	}
	animate();
}

void Player::renderPlayer(sf::RenderWindow& t_window)
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

	t_window.draw(m_playerSprite);

	if (m_dashHasAOE)
	{
		if (m_playerState == CharacterState::DashState)
		{
			if (m_dashAOEClock.getElapsedTime() > m_dashAOETime)
			{
				t_window.draw(m_slashCircle);
				t_window.draw(m_slashSprite);
			}
		}
	}
}

void Player::renderHUD(sf::RenderWindow& t_window)
{
	t_window.draw(m_emptyHealthBar);
	t_window.draw(m_currentHealthBar);

	t_window.draw(m_emptyXPBar);
	t_window.draw(m_xpBar);
	t_window.draw(m_xpBarSprite);
	t_window.draw(m_xpBarText);

	if (m_level > 1)
	{
		t_window.draw(m_emptyDashBarLeft);
		t_window.draw(m_dashBarLeft);
		t_window.draw(m_dashBarLeftSprite);

		t_window.draw(m_emptyDashBarRight);
		t_window.draw(m_dashBarRight);
		t_window.draw(m_dashBarRightSprite);
	}
}

#pragma region INPUT MANAGER
void Player::handleKeyInput()
{
	XINPUT_STATE state;
	memset(&state, 0, sizeof(XINPUT_STATE));

#pragma region Movement
	// Keyboard
	m_movementVector = sf::Vector2f(0.0f, 0.0f);
	m_previousPosition = m_position;
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
			m_slashSprite.setScale(-1.0f, 1.0f);
			m_movementVector.x -= m_speed;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || xAxis > JOYSTICK_THRESHOLD)
		{
			m_direction = Direction::East;
			m_playerState = CharacterState::WalkSideState;
			m_playerSprite.setScale(0.5f, 0.5f);
			m_dashSprite.setScale(0.5f, 0.5f);
			m_slashSprite.setScale(1.0f, 1.0f);
			m_movementVector.x += m_speed;
		}
	}

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
#pragma endregion

#pragma region Dash Timer Logic
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
			if (m_currentDashCharges > 0 && m_canDash)
			{
				dash();
				m_dashCooldownClock.restart();
				strongRumbleStart();
				dashRumbleTimer.restart();
				isDashRumbling = true;
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

		if (dashRumbleTimer.getElapsedTime().asSeconds() > 0.5f && isDashRumbling) {
			isDashRumbling = false;
		}
	}
}

#pragma endregion

#pragma region Rumble
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

void Player::strongRumbleStart()
{
	XINPUT_VIBRATION vibration;
	memset(&vibration, 0, sizeof(XINPUT_VIBRATION)); // Clear memory using memset
	vibration.wLeftMotorSpeed = STRONG_RUMBLE_THRESHOLD;
	vibration.wRightMotorSpeed = STRONG_RUMBLE_THRESHOLD;
	XInputSetState(0, &vibration);
}

void Player::weakRumbleStart()
{
	XINPUT_VIBRATION vibration;
	memset(&vibration, 0, sizeof(XINPUT_VIBRATION)); // Clear memory using memset
	vibration.wLeftMotorSpeed = WEAK_RUMBLE_THRESHOLD;
	vibration.wRightMotorSpeed = WEAK_RUMBLE_THRESHOLD;
	XInputSetState(0, &vibration);
}
#pragma endregion


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

std::vector<Weapon*> Player::getWeapons()
{
	return m_weapons;
}

bool& Player::getRumbleState()
{
	return isDashRumbling;
}

bool& Player::getAliveState()
{
	return isPlayerAlive;
}

sf::RectangleShape Player::getDashCollider()
{
	return m_dashRect;
}

sf::CircleShape Player::getSlashCollider()
{
	return m_slashCircle;
}

#pragma endregion

#pragma region Setters
void Player::setPosition(sf::View& t_view)
{
	if (m_position.x < 40)
	{
		m_position.x = 40;
	}
	else if (m_position.x > 3160)
	{
		m_position.x = 3160;
	}

	if (m_position.y < 50)
	{
		m_position.y = 50;
	}
	else if (m_position.y > 1750)
	{
		m_position.y = 1750;
	}

	sf::Vector2f cameraPos(t_view.getCenter());
	sf::Vector2f xpBarPos = sf::Vector2f(cameraPos.x, cameraPos.y - 410.0f);
	sf::Vector2f dashBarLeftPos = sf::Vector2f(cameraPos.x - 250.0f, cameraPos.y + 400.0f);
	sf::Vector2f dashBarRightPos = sf::Vector2f(cameraPos.x + 250.0f, cameraPos.y + 400.0f);

	m_rectangle.setPosition(m_position.x, m_position.y + 20.0f);
	m_playerSprite.setPosition(m_position);

	m_emptyHealthBar.setPosition(m_position.x, m_position.y - 60.0f);
	m_currentHealthBar.setPosition(m_position.x, m_position.y - 60.0f);

	m_xpBar.setPosition(xpBarPos);
	m_emptyXPBar.setPosition(xpBarPos);
	m_xpBarSprite.setPosition(xpBarPos);
	m_xpBarText.setPosition(xpBarPos.x, xpBarPos.y - 2.0f);

	m_dashBarLeftSprite.setPosition(dashBarLeftPos);
	m_emptyDashBarLeft.setPosition(dashBarLeftPos);
	m_dashBarLeft.setPosition(dashBarLeftPos);

	m_dashBarRightSprite.setPosition(dashBarRightPos);
	m_emptyDashBarRight.setPosition(dashBarRightPos);
	m_dashBarRight.setPosition(dashBarRightPos);

	m_haloSprite.setPosition(sf::Vector2f(m_position.x, m_position.y + 42.0f));

	if (m_dashHasAOE)
	{
		if (m_playerState == CharacterState::DashState)
		{
			if (m_dashAOEClock.getElapsedTime() > m_dashAOETime)
			{
				m_slashSprite.setPosition(m_position);
				//m_slashCircle.setPosition(m_position);
			}
		}
		else
		{
			m_slashSprite.setPosition(sf::Vector2f(-1000.0f, -1000.0f));
			m_slashCircle.setPosition(sf::Vector2f(-1000.0f, -1000.0f));
		}
	}
}

void Player::setHealth()
{
	if (m_health <= 0)
	{
		m_health = 0;
		m_playerState = CharacterState::DeadState;
		isPlayerAlive = false;
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
		m_maxHealth *= 1.25;
		//m_health += 50;
		break;
	case PlayerUpgrade::Speed:
		m_speedModifier *= 1.25f;
		break;
	case PlayerUpgrade::XP:
		m_xpModifier *= 1.25f;
		break;
	case PlayerUpgrade::Armor:
		if (m_armorModifier > 0.2)
		{
			m_armorModifier -= 0.1f;
		}
		break;
	case PlayerUpgrade::Magnet:
		m_magnetModifier += 0.5f;
		break;
	default:
		break;
	}
}

#pragma endregion

#pragma region Gun Upgrades
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
#pragma endregion

#pragma region Dash Upgrades
void Player::upgradeDash()
{
	//include damage upgrades
	switch (m_level)
	{
	case 2:		// 1st charge
		m_maxDashCharges = 1;
		break;
	case 3:		// cd -0.5s
		m_dashCooldownTime = seconds(3.5f);
		break;
	case 4:		// 2nd charge
		m_maxDashCharges = 2;
		break;
	case 5:		// AOE
		m_dashHasAOE = true;
		break;
	case 6:		// distance & 3rd charge
		m_maxDashCharges = 3;
		m_dashDistance = 200.0f;
		break;
	case 7:		// cd -0.5s
		m_dashCooldownTime = seconds(3.0f);
		break;
	case 8:		// 4th charge
		m_maxDashCharges = 4;
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region Care Package
void Player::giveWeapon(WeaponType t_type)
{
	bool weaponEquipped = false;

	switch (t_type)
	{
	case WeaponType::Pistol:
		for (auto weapon : m_weapons)
		{
			if (weapon->getType() == t_type) {
				weapon->equipWeapon();
				weaponEquipped = true;
			}
		}

		if (weaponEquipped) {
			cout << "You already have that weapon equipped.\n";
		}
		else {
			m_weapons.push_back(new Weapon(WeaponType::Pistol, m_holder["starterAtlas"])); // all we need to do to player to add a new weapon
			cout << "Pistol added to arsenal.\n";
		}
		break;
	case WeaponType::AssaultRifle:
		for (auto weapon : m_weapons)
		{
			if (weapon->getType() == t_type) {
				weapon->equipWeapon();
				weaponEquipped = true;
			}
		}

		if (weaponEquipped) {
			cout << "You already have that weapon equipped.\n";
		}
		else {
			m_weapons.push_back(new Weapon(WeaponType::AssaultRifle, m_holder["starterAtlas"])); // all we need to do to player to add a new weapon
			cout << "Assault Rifle added to arsenal.\n";
		}
		break;
	case WeaponType::Sniper:
		for (auto weapon : m_weapons)
		{
			if (weapon->getType() == t_type) {
				weapon->equipWeapon();
				weaponEquipped = true;
			}
		}

		if (weaponEquipped) {
			cout << "You already have that weapon equipped.\n";
		}
		else {
			m_weapons.push_back(new Weapon(WeaponType::Sniper, m_holder["starterAtlas"])); // all we need to do to player to add a new weapon
			cout << "Sniper added to arsenal.\n";
		}
		break;
	case WeaponType::RPG:
		for (auto weapon : m_weapons)
		{
			if (weapon->getType() == t_type) {
				weapon->equipWeapon();
				weaponEquipped = true;
			}
		}

		if (weaponEquipped) {
			cout << "You already have that weapon equipped.\n";
		}
		else {
			m_weapons.push_back(new Weapon(WeaponType::RPG, m_holder["starterAtlas"])); // all we need to do to player to add a new weapon
			cout << "RPG added to arsenal.\n";
		}
		break;
	default:
		cout << "ERROR RUNNING TRYING TO ADD WEEAPON. WEAPON TYPE DOES NOT EXIST IN CONTEXT!\n";
		break;
	}
}

#pragma endregion

#pragma endregion

void Player::pushBack()
{
	m_position = m_previousPosition;
}

void Player::decreaseHealth(float t_damage)
{
	m_health -= t_damage * m_armorModifier;
}

void Player::increaseHealth()
{
	if (m_health < m_maxHealth)
	{
		m_health += m_maxHealth / 8.0f;
	}
}

float Player::getMagnetDistance()
{
	return m_magnetDistance * m_magnetModifier;
}

void Player::dash()
{
	m_canDash = false;
	m_dashStateClock.restart();

	m_dashSound.stop();
	m_dashSound.play();

	sf::Vector2f heading = m_movementVector * m_dashDistance;

	for (unsigned int i = 0; i < AFTERIMAGE_COUNT; i++)
	{
		AfterImageData data{
			m_position + (m_movementVector * (m_dashDistance * (static_cast<float>(i) / AFTERIMAGE_COUNT))),
			static_cast<float>(i) / AFTERIMAGE_COUNT, m_dashSprite
		};

		m_afterImages.push_back(data);
	}

	float distance = std::sqrtf(heading.x * heading.x + heading.y * heading.y);
	float dashAngle = atan2(heading.y, heading.x) * (180.0f / 3.14159);

	if (abs(static_cast<int>(dashAngle)) % 90 == 0)
	{
		distance += 200.0f;
		m_dashRect.setSize(sf::Vector2f(distance, 120.0f));
	}
	else
	{
		m_dashRect.setSize(sf::Vector2f(distance, 10.0f));
	}

	m_dashRect.setOrigin(distance / 2.0f, m_dashRect.getSize().y / 2.0f);
	m_dashRect.setRotation(dashAngle);
	m_dashRect.setPosition(m_position.x + heading.x / 2.0f, m_position.y + heading.y / 2.0f);

	m_position += heading;
	m_currentDashCharges--;

}

void Player::updateDashbar()
{
	m_dashBarFillAmount = -(m_dashCooldownClock.getElapsedTime().asSeconds() / m_dashCooldownTime.asSeconds() * 244.0f);

	if (m_dashBarFillAmount <= -244.0f)
	{
		m_dashBarFillAmount = -244.0f;
		m_dashBarLeft.setFillColor(sf::Color::White);
		m_dashBarRight.setFillColor(sf::Color::White);
		m_currentDashCharges = m_maxDashCharges;
	}
	else
	{
		m_dashBarLeft.setFillColor(sf::Color::Red);
		m_dashBarRight.setFillColor(sf::Color::Red);
	}

	m_dashBarLeft.setSize(sf::Vector2f(24.0f, m_dashBarFillAmount));
	m_dashBarRight.setSize(sf::Vector2f(24.0f, m_dashBarFillAmount));
}

int Player::getDashLevel()
{
	return (m_level - 1);
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

		upgradeDash();
	}

	m_xpBarText.setString(std::to_string(static_cast<int>(m_xp)) + " / " + std::to_string(static_cast<int>(m_xpRequired)));
	m_xpBarText.setOrigin(m_xpBarText.getGlobalBounds().width / 2.0f, m_xpBarText.getGlobalBounds().height / 2.0f);

	m_xpBar.setSize(sf::Vector2f(m_xp / m_xpRequired * 1000.0f, 20.0f));
}

#pragma region FSM
void Player::animate()
{
	if (m_playerClock.getElapsedTime() > m_playerTime)
	{
		if (m_playerState != CharacterState::DeadState)
		{
			if (m_currentPlayerFrame + 1 < m_playerFrames.size())
			{
				m_currentPlayerFrame++;
			}
			else
			{
				m_currentPlayerFrame = 0;
			}
		}
		else
		{
			if (m_currentPlayerFrame + 1 < m_playerFrames.size())
			{
				m_currentPlayerFrame++;
			}
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

	if (m_dashHasAOE)
	{
		if (m_playerState == CharacterState::DashState)
		{
			if (m_dashAOEClock.getElapsedTime() > m_dashAOETime)
			{
				if (m_slashClock.getElapsedTime() > m_slashTime)
				{
					if (m_currentSlashFrame + 1 < m_slashFrames.size())
					{
						m_currentSlashFrame++;
					}
					m_slashClock.restart();
				}

				m_slashSprite.setTextureRect(m_slashFrames[m_currentSlashFrame]);
			}
		}
	}
}

void Player::addFrame(sf::IntRect& t_frame)
{
	m_playerFrames.push_back(t_frame);
}

void Player::setFrames()
{
	m_playerFrames.clear();
	m_currentPlayerFrame = 0;
	m_currentSlashFrame = 0;

	m_dashAOEClock.restart();

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
	case CharacterState::DeadState:
		addFrame(IntRect{ 0, 2176, 400, 200 });
		addFrame(IntRect{ 400, 2176, 400, 200 });
		addFrame(IntRect{ 800, 2176, 400, 200 });
		addFrame(IntRect{ 1200, 2176, 400, 200 });
		addFrame(IntRect{ 1600, 2176, 400, 200 });
		addFrame(IntRect{ 2000, 2176, 400, 200 });
		addFrame(IntRect{ 2400, 2176, 400, 200 });
		addFrame(IntRect{ 2800, 2176, 400, 200 });
		addFrame(IntRect{ 3200, 2176, 400, 200 });
		addFrame(IntRect{ 3600, 2176, 400, 200 });
		addFrame(IntRect{ 4000, 2176, 400, 200 });
		addFrame(IntRect{ 4400, 2176, 400, 200 });
		addFrame(IntRect{ 4800, 2176, 400, 200 });
		addFrame(IntRect{ 5200, 2176, 400, 200 });
		addFrame(IntRect{ 5600, 2176, 400, 200 });
		m_playerSprite.setOrigin(200.0f, 100.0f);
		m_deathSound.play();
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

void Player::setDamageIndicator(sf::Color t_colour)
{
	m_playerSprite.setColor(t_colour);
	m_damageClock.restart();
}

void Player::setHealIndicator(sf::Color t_colour)
{
	m_playerSprite.setColor(t_colour);
	m_healClock.restart();
}
