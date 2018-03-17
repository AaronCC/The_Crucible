#ifndef GUI_H
#define GUI_H

#include "Game.h"

enum EType { BUTTON };

class GuiElement {

public:
	sf::RectangleShape outline;
	sf::Sprite sprite;
	sf::Vector2f size;
	sf::Vector2f pos;
	sf::Text text;
	std::string msg;
	std::string id;
	EType type;
	int eVariant;
	AnimationHandler animHandler;
	bool highlighted;

	void setPosition(sf::Vector2f position) { this->pos = position; }

	std::string activate() { return msg; }

	void draw(sf::RenderWindow& window, float dt);
	void update(const float dt);
	void onHover();

	GuiElement(sf::Texture& texture,
		const unsigned int height,
		const sf::Vector2f size,
		const std::vector<Animation>& animations,
		const EType type,
		const std::string& msg,
		const std::string& id,
		sf::Vector2f pos)
	{
		this->type = type;
		this->msg = msg;
		this->id = id;
		this->eVariant = 0;
		this->size = size;

		highlighted = false;
		outline.setSize({ size.x + 8,size.y + 8 });
		outline.setFillColor(sf::Color::White);
		outline.setOrigin(outline.getSize().x / 2, outline.getSize().y / 2);
		outline.setPosition(pos);

		animHandler.changeAnim(this->eVariant);
		this->sprite.setOrigin(sf::Vector2f(size.x / 2, size.y / 2));
		this->sprite.setPosition(pos);
		this->sprite.setTexture(texture);
		this->animHandler.frameSize = sf::IntRect(0, 0, size.x, size.y*height);
		for (auto animation : animations)
		{
			this->animHandler.addAnim(animation);
		}
		this->animHandler.update(0.0f);
	}

};

class GuiStyle {
public:
	sf::Color textCol;
	sf::Color textHighlightCol;
	sf::Font* font;
	bool centered;
	
	GuiStyle() {};

	GuiStyle(sf::Color textCol, sf::Color textHighlightCol, sf::Font* font, bool centered) {
		this->textCol = textCol;
		this->textHighlightCol = textHighlightCol;
		this->font = font;
		this->centered = centered;
	}
	~GuiStyle() {}
};

class Gui
{
private:
	Game * game;
public:
	GuiStyle style;
	bool centeredText;
	bool horizontal;

	std::vector<GuiElement> elements;
	int getElement(const sf::Vector2f mousePos);
	std::string getEMsg(const sf::Vector2f mousePos);
	void draw(sf::RenderWindow& window, float dt);
	void update(float dt);

	void highlight(int eId) {
		for (int i = 0; i < elements.size(); i++)
			elements[i].highlighted = false;
		if (eId > -1 && eId < elements.size())
			elements[eId].highlighted = true;
	}

	Gui() {}
	Gui(GuiStyle style, bool horizontal,
		std::vector<std::string> eTextures,
		std::vector<std::vector<Animation>> eAnims,
		std::vector<std::pair<std::string, std::string>> eMsgs,
		sf::Vector2f eSize,
		std::vector<sf::Vector2f> ePos,
		EType eType,
		int padding,
		Game* game) 
	{
		this->style = style;
		this->horizontal = horizontal;
		for (size_t e = 0; e < eMsgs.size(); ++e)
		{
			this->game = game;
			sf::Text text;
			text.setString(eMsgs[e].first);
			text.setFont(*style.font);
			text.setFillColor(style.textCol);
			text.setCharacterSize(eSize.y - padding);
			elements.push_back(GuiElement(this->game->texmgr.getRef(eTextures[e]),
				1, eSize, eAnims[e], eType,
				eMsgs[e].first,
				eMsgs[e].second,
				ePos[e]));
		}
	}
	~Gui();
};

#endif /* GUI_H */