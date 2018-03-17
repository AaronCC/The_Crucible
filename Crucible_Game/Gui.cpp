#include "Gui.h"

int Gui::getElement(const sf::Vector2f mousePos)
{
	/* If there are no entries then outside the menu */
	if (elements.size() == 0) return -1;
	//if (!this->visible) return -1;

	for (int i = 0; i < this->elements.size(); ++i)
	{
		/* Translate point to use the entry's local coordinates*/
		sf::Vector2f point = mousePos;
		point += this->elements[i].sprite.getOrigin();
		point -= this->elements[i].sprite.getPosition();

		if (point.x < 0 || point.x > this->elements[i].sprite.getTextureRect().width) continue;
		if (point.y < 0 || point.y > this->elements [i].sprite.getTextureRect().height) continue;
		return i;
	}

	return -1;
}

std::string Gui::getEMsg(const sf::Vector2f mousePos)
{
	/* If there are no entries then outside the menu */
	if (elements.size() == 0) return "";
	//if (!this->visible) return -1;

	for (int i = 0; i < this->elements.size(); ++i)
	{
		/* Translate point to use the entry's local coordinates*/
		sf::Vector2f point = mousePos;
		point += this->elements[i].sprite.getOrigin();
		point -= this->elements[i].sprite.getPosition();

		if (point.x < 0 || point.x > this->elements[i].sprite.getTextureRect().width) continue;
		if (point.y < 0 || point.y > this->elements[i].sprite.getTextureRect().height) continue;
		return elements[i].activate();
	}

	return "";
}

void Gui::draw(sf::RenderWindow & window, float dt)
{
	for (auto element : elements)
	{
		element.animHandler.changeAnim(element.eVariant);
		element.animHandler.update(dt);
 		element.sprite.setTextureRect(element.animHandler.bounds);
		element.draw(window, dt);
	}
}

void Gui::update(float dt)
{
	for (auto element : elements)
	{
		element.update(dt);
	}
}

Gui::~Gui()
{
}

void GuiElement::draw(sf::RenderWindow& window, float dt)
{
	this->animHandler.changeAnim(this->eVariant);

	this->animHandler.update(dt);

	this->sprite.setTextureRect(this->animHandler.bounds);

	if (highlighted)
		window.draw(this->outline);

	window.draw(this->sprite);

	return;
}

void GuiElement::update(const float dt)
{
}

void GuiElement::onHover()
{
}
