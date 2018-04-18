#ifndef LOOT_H
#define LOOT_H

#include "Game.h"
#include "Item.h"
#include "Consumable.h"
#include "ItemGenerator.h"

class Loot
{
public:
	sf::Sprite sprite;

	enum LootType {
		ITEM,
		SCROLL,
		CONSUMABLE,
		NONE
	};
	LootType type;

	ItemGenerator* itemGenerator;
	Item* item;
	Scroll* scroll;
	Consumable* con;
	sf::Vector2i pos;

	LootType chooseType(int roll);
	std::string getName();

	std::vector<std::pair<sf::Color,std::string>> getBuffString()
	{
		switch (type)
		{
		case ITEM:
			return item->getBuffString();
		case SCROLL:
			return scroll->getBuffString();
		case CONSUMABLE:
			return { { sf::Color::White, "CON_STRING" } };
		default:
			return { { sf::Color::Red, "ERROR_GETBUFFSTR" } };
		}
	}
	Item::Rarity getRarity();
	Loot() {}
	Loot(int mf, int ilvl, sf::Vector2i pos, Game* game, ItemGenerator* gen) {
		this->sprite.setTexture(game->texmgr.getRef("lootbag"));
		this->sprite.setPosition(pos.x * TILE_SIZE, pos.y * TILE_SIZE);
		this->sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
		this->itemGenerator = gen;
		this->pos = pos;
		type = chooseType(itemGenerator->getRand_100());
		switch (type)
		{
		case ITEM:
			this->item = itemGenerator->makeItem(ilvl, mf);
			break;
		case SCROLL:
			this->scroll = itemGenerator->makeScroll(ilvl, mf);
			break;
		case CONSUMABLE:
			break;
		case NONE:
			break;
		default:
			break;
		}
	}

	void draw(sf::RenderWindow & window);

	~Loot();
};

#endif