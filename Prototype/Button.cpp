#include "Button.h"

Button::Button(ButtonType t_type, Texture& t_texture, Font& t_font, Vector2f t_pos) :m_pos(t_pos), m_type(t_type)
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
		m_bgSprite.setScale(1.0f, 1.0f);

		//most likely setup text here as well
		m_text.setString("PLAY");
		break;

	case ButtonType::Exit:
		m_bgSprite.setTextureRect(IntRect{ 0, 0, 550, 150 }); //change based off position`
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(1.0f, 1.0f);

		m_text.setString("EXIT");
		break;

	case ButtonType::Tutorial:
		m_bgSprite.setTextureRect(IntRect{ 0, 0, 550, 150 }); //change based off position`
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(1.0f, 1.0f);

		m_text.setString("TUTORIAL");
		break;

	case ButtonType::Credits:
		m_bgSprite.setTextureRect(IntRect{ 0, 0, 550, 150 }); //change based off position`
		m_bgSprite.setOrigin(550.0f / 2.0f, 150.0f / 2.0f); //based off of texture rect size
		m_bgSprite.setScale(1.0f, 1.0f);

		m_text.setString("CREDITS");
		break;
#pragma endregion

	}

	m_text.setOrigin(m_text.getGlobalBounds().width / 2.0f, m_text.getGlobalBounds().height / 2.0f);
	m_text.setPosition(m_pos);
}

void Button::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_bgSprite);
	t_window.draw(m_text);
}

void Button::buttonPressed()
{

}

Vector2f Button::getPositon()
{
	return m_pos;
}

ButtonType Button::getType()
{
	return m_type;
}
