#include "Button.h"

Button::Button(ButtonType t_type, ResourceHolder<Texture, string>& t_holder, Font& t_font, Vector2f t_pos) {
	Texture& bgTexture = t_holder["mainMenuBG"];

	switch (t_type) 
	{
	case ButtonType::Menu:
		m_bgSprite.setTexture(bgTexture);
		m_bgSprite.setTextureRect(IntRect{ 0,0,150,80 });
		//m_bgSprite.setOrigin(); //based off of texture rect size
		m_bgSprite.setPosition(t_pos);
		m_bgSprite.setScale(10.0f, 10.0f);
		break;
		
	case ButtonType::Upgrade:
		m_bgSprite.setTexture(bgTexture);
		m_bgSprite.setTextureRect(IntRect{ 0,0,150,80 }); //change based off position`
		//m_bgSprite.setOrigin(); //based off of texture rect size
		m_bgSprite.setPosition(t_pos);
		m_bgSprite.setScale(10.0f, 10.0f);
		break;
	}
}