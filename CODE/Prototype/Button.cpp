#include "Button.h"

Button::Button(ButtonType t_type, Texture& t_texture, Font& t_font, Vector2f t_pos, Vector2f t_scale) :m_pos(t_pos), m_type(t_type)
{
	m_bgSprite.setTexture(t_texture);
	m_bgSprite.setPosition(t_pos);

	m_text.setFont(t_font);
	m_text.setStyle(sf::Text::Bold);
	m_text.setCharacterSize(30U);
	m_text.setOutlineColor(sf::Color::Black);
	m_text.setFillColor(sf::Color::Red);
	m_text.setOutlineThickness(2.0f);

	switch (t_type) 
	{
#pragma region Menu Button setup
	case ButtonType::Play:
		m_bgSprite.setTextureRect(IntRect{ 0, 0, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		//most likely setup text here as well
		m_text.setString("PLAY");
		break;

	case ButtonType::Exit:
		m_bgSprite.setTextureRect(IntRect{ 0, 0, 550, 150 }); //change based off position`
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("EXIT");
		break;

	case ButtonType::Tutorial:
		m_bgSprite.setTextureRect(IntRect{ 0, 0, 550, 150 }); //change based off position`
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("TUTORIAL");
		break;

	case ButtonType::Credits:
		m_bgSprite.setTextureRect(IntRect{ 0, 0, 550, 150 }); //change based off position`
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("CREDITS");
		break;
#pragma endregion
		
#pragma region Pause Button setup
	case ButtonType::Resume:
		m_bgSprite.setTextureRect(IntRect{ 0, 0, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("RESUME");
		break;

	case ButtonType::ToMenu:
		m_bgSprite.setTextureRect(IntRect{ 0, 0, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("TO MENU");
		break;

#pragma endregion

#pragma region Upgrade Button setup
	case ButtonType::UpgradeHealth:
		m_bgSprite.setTextureRect(IntRect{ 0, 150, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("INCREASE HEALTH");
		break;
	case ButtonType::UpgradeSpeed:
		m_bgSprite.setTextureRect(IntRect{ 0, 150, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("INCREASE SPEED");
		break;
	case ButtonType::UpgradeXP:
		m_bgSprite.setTextureRect(IntRect{ 0, 150, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("XP X2");
		break;
	case ButtonType::UpgradeArmor:
		m_bgSprite.setTextureRect(IntRect{ 0, 150, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("INCREASE ARMOR");
		break;
#pragma endregion

#pragma region GetGun Button setup
	case ButtonType::GetPistol:
		m_bgSprite.setTextureRect(IntRect{ 0, 150, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("Pistol");
		break;
	case ButtonType::GetRifle:
		m_bgSprite.setTextureRect(IntRect{ 0, 150, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("Assault\nRifle");
		break;

	case ButtonType::UpgradePistol:
		m_bgSprite.setTextureRect(IntRect{ 0, 150, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("PISTOL UPGRADE +");
		break;
	case ButtonType::UpgradeAR:
		m_bgSprite.setTextureRect(IntRect{ 0, 150, 550, 150 });
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(t_scale.x, t_scale.y);

		m_text.setString("RIFLE UPGRADE +");
		break;

#pragma endregion

	}



	//move back into each switch for accurate calculation
	m_text.setOrigin(m_text.getGlobalBounds().width / 2.0f, m_text.getGlobalBounds().height / 2.0f);
	m_text.setPosition(m_pos);
}

void Button::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_bgSprite);
	t_window.draw(m_text);
}

void Button::setPosition(Vector2f t_pos)
{
	m_pos = t_pos;
	m_bgSprite.setPosition(t_pos);
	m_text.setPosition(t_pos);
}


Vector2f Button::getPositon()
{
	return m_pos;
}

ButtonType Button::getType()
{
	return m_type;
}