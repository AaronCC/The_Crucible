#include "Map.h"
#include <time.h>
#include "Dungeon.h"

sf::Vector2i Map::hasLineOfSight(sf::Vector2i from, sf::Vector2i to)
{
	sf::Vector2i dir = to - from;
	sf::Vector2i atLocal;
	sf::Vector2i diff;
	sf::Vector2i nextLocal;
	sf::Vector2f ray = { dir.x * 32.f, dir.y * 32.f };
	sf::Vector2f at = { from.x * 32.f, from.y * 32.f };
	float mag = helper.magnitude(ray);
	sf::Vector2f rayI = helper.normalized(ray, mag) * 12.f;
	atLocal = globalToTilePos(at);
	while (atLocal != to)
	{
		while (globalToTilePos(at) == atLocal)
		{
			at += rayI;
		}
		nextLocal = globalToTilePos(at);
		diff += nextLocal - atLocal;
		if (std::abs(diff.x) >= std::abs(dir.x) && std::abs(diff.y) >= std::abs(dir.y) && getTile(to.x, to.y)->passable)
			return to;
		if (!getTile(nextLocal.x, nextLocal.y)->passable)
			return atLocal;
		atLocal = nextLocal;
	}
	return to;
}

void Map::draw(sf::RenderWindow & window, float dt)
{
	// Get the camera's position
	sf::Vector2f camPos = this->camera->view.getCenter();
	sf::Vector2f center = camPos / 32.f;
	// Calculate the start tile
	sf::Vector2f drawStart = { (int)center.x - (drawSize.x / 2), (int)center.y - (drawSize.y / 2) };
	// Clamp
	drawStart.x = drawStart.x < 0 ? 0 : drawStart.x;
	drawStart.y = drawStart.y < 0 ? 0 : drawStart.y;
	// Calc end tile
	sf::Vector2f drawEnd = { drawStart.x + drawSize.x, drawStart.y + drawSize.y };
	// Clamp
	drawEnd.x = drawEnd.x > width ? width : drawEnd.x;
	drawEnd.y = drawEnd.y > height ? height : drawEnd.y;

	for (int y = drawStart.y; y < drawEnd.y; ++y)
	{
		for (int x = drawStart.x; x < drawEnd.x; ++x)
		{
			this->tiles[y*this->width + x].draw(window, dt);
		}
	}

	for (auto enemy : enemies)
	{
		if (!enemy->dead)
			enemy->draw();
	}
	for (auto l : loot)
	{
		l->draw(this->game->window);
	}
	for (auto t : actionText)
		window.draw(t);
	for (int y = drawStart.y; y < drawEnd.y; ++y)
	{
		for (int x = drawStart.x; x < drawEnd.x; ++x)
		{
			this->tiles[y*this->width + x].drawFoW(window, dt);
		}
	}

	if (this->tiles[mouseIndex.y*this->width + mouseIndex.x].passable)
		canSelect.draw(window, dt);
	else
		cantSelect.draw(window, dt);

	for (auto text : hoverText)
	{
		window.draw(text);
	}
	return;
}

sf::Vector2i Map::globalToTilePos(sf::Vector2f global)
{
	sf::Vector2i local;
	local.x = ::ceil(global.x / 32);
	local.y = ::ceil(global.y / 32);
	if (global.x < 0)
		global.x = 0;
	else if (global.x > width)
		global.x = width - 1;
	if (global.y < 0)
		global.y = 0;
	return local;
}

void Map::update(float dt)
{
	sf::Vector2i selectPos = getSelectPos();
	std::vector<sf::Vector2i> oldPosBuffer;
	for (int i = 0; i < enemies.size(); i++)
	{
		Enemy& enemy = *enemies[i];

		if (enemy.update())
		{
			getTile(enemy.tilePos.x, enemy.tilePos.y)->occupied = false;
			oldPosBuffer.push_back(enemy.tilePos);
			Loot* newloot = new Loot(enemy.mf, enemy.lvl, enemy.tilePos, this->game, this->itemGenerator);
			tLoot[enemy.tilePos.y * this->width + enemy.tilePos.x] = newloot;
			loot.push_back(newloot);
		}
	}
	for (auto pos : oldPosBuffer)
	{
		tEnemies[pos.y*this->width + pos.x] = nullptr;
	}
	Loot* loot;
	if (oldMouseIndex != mouseIndex)
		updateHoverText();
	oldMouseIndex = mouseIndex;
	canSelect.setPosition(selectPos);
	cantSelect.setPosition(selectPos);
}
void Map::updateHoverText()
{
	Loot* loot;
	if (getTile(mouseIndex.x, mouseIndex.y)->occupied)
	{
		Enemy* enemy = tEnemies[mouseIndex.y*this->width + mouseIndex.x];
		if (enemy != nullptr)
		{
			std::string name = enemy->name;
			std::string hp = std::to_string(enemy->hp);
			std::string maxHp = std::to_string(enemy->maxHp);
			hoverText.clear();
			hoverText.push_back(sf::Text(name, game->fonts["main_font"], tSize));
			hoverText.push_back(sf::Text(hp + "/" + maxHp, game->fonts["main_font"], tSize));
			hoverText[0].setPosition((mouseIndex.x * tileSize.x) - (TILE_SIZE / 2), (mouseIndex.y * tileSize.y) - (tSize * 3.5));
			hoverText[0].setFillColor({ 255, 0, 0 });
			hoverText[1].setPosition((mouseIndex.x * tileSize.x) - (TILE_SIZE / 2), (mouseIndex.y * tileSize.y) - (tSize * 2.5f));
			hoverText[1].setFillColor({ 255, 0, 0 });
			hoverText[0].setOutlineThickness(1);
			hoverText[1].setOutlineThickness(1);
		}
	}
	else if ((loot = tLoot[mouseIndex.y*this->width + mouseIndex.x]) != nullptr)
	{
		std::string name = loot->item->name;
		hoverText.clear();
		hoverText.push_back(sf::Text(name, game->fonts["main_font"], tSize));
		hoverText[0].setPosition((mouseIndex.x * tileSize.x) - (TILE_SIZE / 2), (mouseIndex.y * tileSize.y) - (tSize * 2.5));
		switch (loot->item->rarity)
		{
		case Item::Rarity::NORM:
			hoverText[0].setFillColor(NORM_COLOR);
			break;
		case Item::Rarity::MAGIC:
			hoverText[0].setFillColor(MAGIC_COLOR);
			break;
		case Item::Rarity::RARE:
			hoverText[0].setFillColor(RARE_COLOR);
			break;
		case Item::Rarity::ULTRA:
			hoverText[0].setFillColor(ULTRA_COLOR);
			break;
		}
		hoverText[0].setOutlineThickness(1);
	}
	else
		hoverText.clear();
}
bool Map::activateObjsAtTile(std::vector<sf::Vector2i> pos)
{
	bool hit = false;
	int hc = 0;
	aEnemies.clear();
	for (int i = 0; i < enemies.size(); i++)
	{
		Enemy& enemy = *enemies[i];
		enemy.deactivate();
		for (auto point : pos)
			if (enemy.tilePos == point && !enemy.dead)
			{
				aEnemies.push_back(&enemy);
				enemy.active = true;
				hc++;
			}
		if (!enemy.active)
			enemy.wayPoints.clear();
	}

	if (hc > 0)
		hit = true;
	return hit;
}
sf::Vector2i Map::getSelectPos()
{
	sf::Vector2f camPos = this->camera->view.getCenter();
	sf::Vector2f camSize = this->camera->view.getSize();
	sf::Vector2f mousePos =
		this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->camera->view);
	mousePos -= {16, 16};
	this->mouseIndex.x = std::ceil(mousePos.x / 32);
	this->mouseIndex.y = std::ceil(mousePos.y / 32);
	if (mouseIndex.x < 0)
		mouseIndex.x = 0;
	else if (mouseIndex.x > width)
		mouseIndex.x = width - 1;
	if (mouseIndex.y < 0)
		mouseIndex.y = 0;
	else if (mouseIndex.y >= height)
		mouseIndex.y = height - 1;
	return { mouseIndex.x * 32, mouseIndex.y * 32 };
}

void Map::handleInput(sf::Event event)
{
	switch (event.type)
	{
	case sf::Event::MouseMoved:

		break;
	default:
		break;
	}
}
void Map::eraseLoot(sf::Vector2i pos)
{
	Loot* newLoot = tLoot[pos.y*this->width + pos.x];
	if (newLoot != nullptr)
	{
		for (int i = 0; i < loot.size(); i++)
		{
			if (loot[i]->pos == pos)
			{
				loot.erase(loot.begin() + i);
				break;
			}
		}
		tLoot[pos.y * this->width + pos.x] = nullptr;
	}
}
void Map::resolveAction(Player* player)
{
	Item * itm;
	Loot * newLoot;
	switch (action)
	{
	case Action::PICKUP:
		newLoot = tLoot[player->tilePos.y *this->width + player->tilePos.x];
		if (newLoot != nullptr)
		{
			itm = newLoot->item;
			if (itm != nullptr)
				if (player->pickup(itm))
				{
					eraseLoot(player->tilePos);
					updateActionText(player->tilePos);
				}
		}
		break;
	default:
		break;
	}
}
void Map::updateActionText(sf::Vector2i playerPos)
{
	actionText.clear();
	this->action = Action::NONE;
	Loot* newLoot = tLoot[playerPos.y *this->width + playerPos.x];
	if (newLoot != nullptr)
	{
		actionText.push_back(sf::Text("e: pickup", game->fonts["main_font"], tSize));
		actionText[0].setFillColor(sf::Color::White);
		actionText[0].setOutlineThickness(1);
		actionText[0].setPosition((playerPos.x * tileSize.x) - (TILE_SIZE / 2), (playerPos.y * tileSize.y) - (tSize * 3.5));
		std::string name = newLoot->item->name;
		actionText.push_back(sf::Text(name, game->fonts["main_font"], tSize));
		actionText[1].setPosition((playerPos.x * tileSize.x) - (TILE_SIZE / 2), (playerPos.y * tileSize.y) - (tSize * 2.5));
		switch (newLoot->item->rarity)
		{
		case Item::Rarity::NORM:
			actionText[1].setFillColor(NORM_COLOR);
			break;
		case Item::Rarity::MAGIC:
			actionText[1].setFillColor(MAGIC_COLOR);
			break;
		case Item::Rarity::RARE:
			actionText[1].setFillColor(RARE_COLOR);
			break;
		case Item::Rarity::ULTRA:
			actionText[1].setFillColor(ULTRA_COLOR);
			break;
		}
		actionText[1].setOutlineThickness(1);
		this->action = Action::PICKUP;
	}
}

//std::vector<Enemy*> Map::getEnemiesAtPoint(sf::Vector2i point)
//{
//	std::vector<Enemy*> aEnemies;
//	for (int i = 0; i < enemies.size(); i++)
//	{
//		if (enemies[i].tilePos == point)
//			aEnemies.push_back(&enemies[i]);
//	}
//	return aEnemies;
//}

void Map::updateEntityAI(float tickCount, sf::Vector2i pPos, PathFinder* pf)
{
	for (auto point : entityOccToClear)
	{
		getTile(point.x, point.y)->occupied = false;
	}
	entityOccToClear.clear();
	for (int i = 0; i < aEnemies.size(); i++)
	{
		Enemy* enemy = aEnemies[i];
		if (enemy->dead)
			continue;
		sf::Vector2i end = enemy->updateAI(tickCount, pPos, pf);
		getTile(end.x, end.y)->occupied = true;
		if (end != enemy->tilePos)
		{
			entityOccToClear.push_back(end);
		}
	}
}

void Map::resolveEntityAI(float tickCount)
{
	for (auto point : entityOccToClear)
	{
		getTile(point.x, point.y)->occupied = false;
	}
	entityOccToClear.clear();
	std::vector<Enemy*> moveBuffer;
	std::vector<sf::Vector2i> oldPosBuffer;
	for (int i = 0; i < enemies.size(); i++)
	{
		Enemy* enemy = enemies[i];
		if (enemy->active)
			for (int t = 0; t < tickCount; t++)
			{
				std::pair<sf::Vector2i, sf::Vector2i> occ_tiles;
				occ_tiles = enemy->resolveTick(tickCount);
				if (occ_tiles.first != occ_tiles.second)
				{
					getTile(occ_tiles.first.x, occ_tiles.first.y)->occupied = false;
					moveBuffer.push_back(enemy);
					oldPosBuffer.push_back(occ_tiles.first);
				}
				getTile(occ_tiles.second.x, occ_tiles.second.y)->occupied = true;
			}
		else if (enemy->dead)
		{
			oldPosBuffer.push_back(enemy->tilePos);
		}
	}
	int i = 0;
	for (auto enemy : moveBuffer)
	{
		tEnemies[enemy->tilePos.y*this->width + enemy->tilePos.x] = enemy;
		for (auto pos : oldPosBuffer)
		{
			if (enemy->tilePos == pos)
			{
				oldPosBuffer.erase(oldPosBuffer.begin() + i);
				break;
			}
			i++;
		}
	}
	for (auto pos : oldPosBuffer)
	{
		tEnemies[pos.y*this->width + pos.x] = nullptr;
	}
}

void Map::loadMap()
{
	width = 79;
	height = 24;
	tEnemies.clear();
	tEnemies.reserve(width*height);
	for (int i = 0; i < width*height; i++)
	{
		tEnemies.push_back(nullptr);
		tLoot.push_back(nullptr);
	}
	Dungeon dungeon(width, height);
	dungeon.generate(70);
	dungeon.print();
	std::vector<Dungeon::Entity> entities = dungeon.getEntities();
	tiles.reserve(width * height);
	sf::Vector2i pos = { 0,0 };
	this->spawnPos = { width / 2, (height / 2) + 1 };
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; x++)
		{
			char t = dungeon.getTile(x, y);
			this->tiles.push_back(game->tileAtlas.at("dungeon"));
			switch (t)
			{
			case Dungeon::Tile::Unused:
			{
				this->tiles[y*this->width + x].tileVariant = 0;
				break;
			}
			case Dungeon::Tile::Floor:
			{
				this->tiles[y*this->width + x].tileVariant = 1;
				this->tiles[y*this->width + x].passable = false;
				break;
			}
			case Dungeon::Tile::Wall:
			{
				this->tiles[y*this->width + x].tileVariant = 2;
				this->tiles[y*this->width + x].passable = false;
				break;
			}
			case Dungeon::Tile::ClosedDoor:
			{
				this->tiles[y*this->width + x].tileVariant = 3;
				break;
			}
			case Dungeon::Tile::OpenDoor:
			{
				this->tiles[y*this->width + x].tileVariant = 4;
				break;
			}
			case Dungeon::Tile::UpStairs:
			{
				this->tiles[y*this->width + x].tileVariant = 5;
				break;
			}
			case Dungeon::Tile::DownStairs:
			{
				this->tiles[y*this->width + x].tileVariant = 6;
				break;
			}
			default:
				break;
			}

			this->tiles[y*this->width + x].setPosition(pos, x, y);
			this->tiles[y*this->width + x].sprite.setPosition((sf::Vector2f)pos);
			pos.x += tileSize.x;
		}
		pos.x -= tileSize.x * width;
		pos.y += tileSize.y;
	}
	for (auto e : entities)
	{
		switch (e.id)
		{
		case 'd':
			enemies.push_back(new Enemy("ogre", game, { e.x,e.y }, 10));
			tEnemies[e.y*this->width + e.x] = enemies[enemies.size() - 1];
			getTile(e.x, e.y)->occupied = true;
			break;
		default:
			break;
		}
	}
}

Enemy* Map::getEnemyAt(int x, int y)
{
	return this->tEnemies[y*this->width + x];
}

Tile* Map::getTile(int x, int y)
{
	return &this->tiles[y*this->width + x];
}

Map::Map(Game* game, Camera* camera)
{
	this->game = game;
	this->camera = camera;
	this->tiles = std::vector<Tile>();
	this->tileSize = this->game->tileSize;
	this->canSelect = this->game->tileAtlas.at("can_select");
	this->cantSelect = this->game->tileAtlas.at("cant_select");
	this->canSelect.reveal();
	this->cantSelect.reveal();
	this->drawSize = { (float)this->game->windowSize.x / tileSize.x, (float)this->game->windowSize.y / tileSize.y };
}
Map::~Map()
{
}
