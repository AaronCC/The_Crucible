#ifndef LOOT_H
#define LOOT_H

#include "Game.h"
#include "Item.h"
#include "ItemGenerator.h"

class Loot
{
public:
	sf::Sprite sprite;

	ItemGenerator* itemGenerator;
	Item* item;
	sf::Vector2i pos;
	Loot() {}
	Loot(int mf, int ilvl, sf::Vector2i pos, Game* game, ItemGenerator* gen) {
		this->sprite.setTexture(game->texmgr.getRef("lootbag"));
		this->sprite.setPosition(pos.x * TILE_SIZE, pos.y * TILE_SIZE);
		this->sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
		this->itemGenerator = gen;
		this->pos = pos;
		this->item = itemGenerator->makeItem(ilvl, mf);
	}

	void draw(sf::RenderWindow & window);

	~Loot();
};

#endif