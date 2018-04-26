#include <SFML/Graphics.hpp>

#include "AnimationHandler.h"
#include "Tile.h"

std::pair<bool, Helper::Stats> Tile::getShrineEffect()
{
	bool hp = shrine == HP_SHRINE ? true : false;
	if (!hp)
	{

	}

	return { hp, {} };
}

void Tile::draw(sf::RenderWindow& window, float dt)
{
	/* Change the sprite to reflect the tile variant */
	if (tileType == TileType::STATIC)
		this->animHandler.changeAnim(this->tileVariant);

	/* Update the animation */
	this->animHandler.update(dt);

	/* Update the sprite */
	this->sprite.setTextureRect(this->animHandler.bounds);

	/* Draw the tile */
	window.draw(this->sprite);

	if (shrine != NO_SHRINE)
		window.draw(this->shrineSprite);

	return;
}

void Tile::drawFoW(sf::RenderWindow & window, float dt)
{
	if (dark_fow)
		window.draw(this->fowDarkSprite);
	else if (fow)
		window.draw(this->fowSprite);
}

void Tile::reveal()
{
	this->fow = false;
	this->dark_fow = false;
}

void Tile::update()
{
	return;
}

std::string tileTypeToStr(TileType type)
{
	switch (type)
	{
	default:
		break;
	}
	return "";
}