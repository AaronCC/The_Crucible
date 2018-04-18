#include "Loot.h"

Loot::LootType Loot::chooseType(int roll)
{
	if (roll <= 25)
		return NONE;
	else if (roll <= 50)
		return CONSUMABLE;
	else if (roll <= 80)
		return ITEM;
	else
		return SCROLL;
}

std::string Loot::getName()
{
	switch (type)
	{
	case ITEM:
		return item->name;
	case SCROLL:
		return scroll->name;
	case CONSUMABLE:
		return "CON_NAME";
		//return con->name;
	default:
		return "NULL_LOOT";
	}
}

Item::Rarity Loot::getRarity()
{
	switch (type)
	{
	case ITEM:
		return item->rarity;
	case SCROLL:
		return scroll->rarity;
	case CONSUMABLE:
		return Item::Rarity::NORM;
		//return con->name;
	default:
		return Item::Rarity::NORM;
	}
}

void Loot::draw(sf::RenderWindow & window)
{
	window.draw(sprite);
}

Loot::~Loot()
{
}
