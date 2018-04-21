#include "Map.h"
#include <time.h>
sf::Vector2i Map::truncLineOfSight(sf::Vector2i from, sf::Vector2i to)
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
bool Map::hasLineOfSight(sf::Vector2i from, sf::Vector2i to)
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
			return true;
		if (!getTile(nextLocal.x, nextLocal.y)->passable)
			return false;
		atLocal = nextLocal;
	}
	return false;
}
void Map::drawL2(sf::RenderWindow & window, float dt)
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
	for (auto t : actionText)
		window.draw(t);
	for (auto enemy : enemies)
	{
		if (!enemy->dead)
			enemy->drawAbility(dt);
	}
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
}
void Map::drawL3(sf::RenderWindow & window, float dt)
{
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
			enemy->draw(dt);
	}
	for (auto l : loot)
	{
		l->draw(this->game->window);
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

		if (enemy.update(dt))
		{
			getTile(enemy.tilePos.x, enemy.tilePos.y)->occupied = false;
			oldPosBuffer.push_back(enemy.tilePos);
			Loot* newloot = new Loot(enemy.mf, enemy.lvl, enemy.tilePos, this->game, this->itemGenerator);
			if (newloot->type != Loot::LootType::NONE)
			{
				if (tLoot[enemy.tilePos.y * this->width + enemy.tilePos.x][0] == nullptr)
					tLoot[enemy.tilePos.y * this->width + enemy.tilePos.x][0] = newloot;
				else
					tLoot[enemy.tilePos.y * this->width + enemy.tilePos.x].push_back(newloot);
				loot.push_back(newloot);
			}
			enemy.tilePos = { 0,0 };
		}
	}
	for (auto pos : oldPosBuffer)
	{
		tEnemies[pos.y*this->width + pos.x] = nullptr;
	}
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
		if (enemy != nullptr && enemy->active)
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
	else if ((loot = tLoot[mouseIndex.y*this->width + mouseIndex.x][0]) != nullptr)
	{
		std::string name = loot->getName();
		hoverText.clear();
		hoverText.push_back(sf::Text(name, game->fonts["main_font"], tSize));
		hoverText[0].setPosition((mouseIndex.x * tileSize.x) - (TILE_SIZE / 2), (mouseIndex.y * tileSize.y) - (tSize * 2.5));
		switch (loot->getRarity())
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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			std::vector<std::pair<sf::Color, std::string>> info = loot->getBuffString();
			int count = 0;
			for (auto i : info)
			{
				sf::Text text{ i.second, game->fonts["main_font"], tSize };
				text.setFillColor(i.first);
				text.setOutlineThickness(1);
				text.setPosition((mouseIndex.x * tileSize.x) + (TILE_SIZE / 2), (mouseIndex.y * tileSize.y) - (TILE_SIZE / 2) + (count*tSize));
				hoverText.push_back(text);
				count++;
			}
		}
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
	Loot* newLoot = tLoot[pos.y*this->width + pos.x][0];
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
		if (tLoot[pos.y * this->width + pos.x].size() > 1)
			tLoot[pos.y * this->width + pos.x].erase(tLoot[pos.y * this->width + pos.x].begin());
		else
			tLoot[pos.y * this->width + pos.x][0] = nullptr;
	}
}
void Map::resolveAction(Player* player)
{
	Item * itm = nullptr;
	Scroll * scr = nullptr;
	Consumable * con = nullptr;
	Loot * newLoot;
	switch (action)
	{
	case Action::PICKUP:
		newLoot = tLoot[player->tilePos.y *this->width + player->tilePos.x][0];
		if (newLoot != nullptr)
		{
			switch (newLoot->type)
			{
			case Loot::LootType::ITEM:
				itm = newLoot->item;
				break;
			case Loot::LootType::SCROLL:
				scr = newLoot->scroll;
				break;
			case Loot::LootType::CONSUMABLE:
				con = newLoot->con;
				break;
			default:
				break;
			}
			if (itm != nullptr) {
				if (player->pickup_ITM(itm))
				{
					eraseLoot(player->tilePos);
					updateActionText(player->tilePos);
				}
			}
			else if (scr != nullptr)
			{
				if (player->pickup_SCR(scr))
				{
					eraseLoot(player->tilePos);
					updateActionText(player->tilePos);
				}
			}
			else if (con != nullptr)
			{
				if (player->pickup_CON(con))
				{
					eraseLoot(player->tilePos);
					updateActionText(player->tilePos);
				}
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
	Loot* newLoot = tLoot[playerPos.y *this->width + playerPos.x][0];
	if (newLoot != nullptr)
	{
		actionText.push_back(sf::Text("[e] pickup", game->fonts["main_font"], tSize));
		actionText[0].setFillColor(sf::Color::White);
		actionText[0].setOutlineThickness(1);
		actionText[0].setPosition((playerPos.x * tileSize.x) - (TILE_SIZE / 2), (playerPos.y * tileSize.y) - (tSize * 3.5));
		std::string name = newLoot->getName();
		actionText.push_back(sf::Text(name, game->fonts["main_font"], tSize));
		actionText[1].setPosition((playerPos.x * tileSize.x) - (TILE_SIZE / 2), (playerPos.y * tileSize.y) - (tSize * 2.5));
		switch (newLoot->getRarity())
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
	else if (getTile(playerPos.x, playerPos.y)->tileVariant == 5)
	{
		actionText.push_back(sf::Text("[f] go up", game->fonts["main_font"], tSize));
		actionText[0].setFillColor(sf::Color::White);
		actionText[0].setOutlineThickness(1);
		actionText[0].setPosition((playerPos.x * tileSize.x) - (TILE_SIZE / 2), (playerPos.y * tileSize.y) - (tSize * 3.5));
	}
	else if (getTile(playerPos.x, playerPos.y)->tileVariant == 6)
	{
		actionText.push_back(sf::Text("[f] go down", game->fonts["main_font"], tSize));
		actionText[0].setFillColor(sf::Color::White);
		actionText[0].setOutlineThickness(1);
		actionText[0].setPosition((playerPos.x * tileSize.x) - (TILE_SIZE / 2), (playerPos.y * tileSize.y) - (tSize * 3.5));
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

std::vector<AbEffect::Effect> Map::updateEntityAI(float _tickCount, sf::Vector2i pPos, PathFinder* pf)
{
	std::vector<AbEffect::Effect> effs;
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
		enemy->losToPlayer = hasLineOfSight(enemy->tilePos, pPos);
		sf::Vector2i end = enemy->updateAI(_tickCount, pPos, pf);
		if (enemy->queuedAbility != nullptr)
		{
			for (auto eff : enemy->ability.getEffects())
				effs.push_back(eff);
		}
		getTile(end.x, end.y)->occupied = true;
		if (end != enemy->tilePos)
		{
			entityOccToClear.push_back(end);
		}
	}
	return effs;
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
				// If the enemy moved
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
void Map::loadCave()
{
	width = 50;
	height = 50;
	tEnemies.clear();
	tLoot.clear();
	tEnemies.reserve(width*height);
	tLoot.reserve(width*height);
	for (int i = 0; i < width*height; i++)
	{
		tEnemies.push_back(nullptr);
		tLoot.push_back({ nullptr });
	}
	int* map = new int[width*height];
	CaveGen cave(map, width, height, 3, 4, 4);
	std::map<int, std::vector<sf::Vector2i>> caves = cave.flood();
	int max = 0, maxCave = -1;
	for (auto c : caves)
	{
		if (c.second.size() > max)
		{
			max = c.second.size();
			maxCave = c.first;
		}
	}
	this->spawnPos = caves[maxCave][0];
	//cave.floodConnect(caves);
	//cave.print();
	tiles.reserve(width * height);
	sf::Vector2i pos = { 0,0 };
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; x++)
		{
			char t = cave.getTile(x, y);
			this->tiles.push_back(game->tileAtlas.at("cave"));
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
				this->tiles[y*this->width + x].tileVariant = 0;
				break;
				/*this->tiles[y*this->width + x].tileVariant = 5;
				break;*/
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
}
void Map::loadDungeon()
{
	//loadCave();
	//return;
	width = 79;
	height = 24;
	tEnemies.clear();
	tLoot.clear();
	tEnemies.reserve(width*height);
	tLoot.reserve(width*height);
	for (int i = 0; i < width*height; i++)
	{
		tEnemies.push_back(nullptr);
		tLoot.push_back({ nullptr });
	}
	Dungeon dungeon(width, height);
	dungeon.generate(70);
	dungeon.print();
	entities.clear();
	entities = dungeon.getEntities();
	tiles.reserve(width * height);
	sf::Vector2i pos = { 0,0 };
	this->spawnPos = { width / 2, (height / 2) + 1 };
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; x++)
		{
			char t = dungeon.getTile(x, y);
			//char t = cave.getTile(x, y);
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
				this->tiles[y*this->width + x].tileVariant = 0;
				break;
				/*this->tiles[y*this->width + x].tileVariant = 5;
				break;*/
			}
			case Dungeon::Tile::DownStairs:
			{
				spawnPos = { x + 1, y };
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

}

void Map::populateDungeon()
{
	int roll;
	Ability* ogreAbility = itemGenerator->makeEnemyAbility(level, Item::Rarity::MAGIC, true);
	Ability* wraithAbility = itemGenerator->makeEnemyAbility(level, Item::Rarity::MAGIC, false);
	for (auto e : entities)
	{
		switch (e.id)
		{
		case 'd':
			roll = itemGenerator->getRand_100();
			if (roll <= 50)
			{
				enemies.push_back(new Enemy("ogre", game, { e.x,e.y }, 10, level, ogreAbility,
					"enemyattack1"));
			}
			else
				enemies.push_back(new Enemy("wraith", game, { e.x,e.y }, 10, level, wraithAbility,
					"enemyattack2"));
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
