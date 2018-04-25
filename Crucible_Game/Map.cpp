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
	for (auto t : actionText)
		window.draw(t);
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
	for (auto l : loot)
	{
		l->draw(this->game->window);
	}
	for (auto enemy : enemies)
	{
		if (!enemy->dead)
			enemy->draw(dt);
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
			expCache += 10 + (10 * enemy.lvl);
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
	if (mouseIndex.x < 0) mouseIndex.x = 0;
	if (mouseIndex.y < 0) mouseIndex.y = 0;
	if (mouseIndex.x > width - 1) mouseIndex.x = width - 1;
	if (mouseIndex.y > height - 1) mouseIndex.y = height - 1;
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
	std::vector<Loot*> newLoot = tLoot[playerPos.y *this->width + playerPos.x];
	if (newLoot[0] != nullptr)
	{
		actionText.push_back(sf::Text("[e] pickup", game->fonts["main_font"], tSize));
		actionText[0].setFillColor(sf::Color::White);
		actionText[0].setOutlineThickness(1);
		actionText[0].setPosition((playerPos.x * tileSize.x) - (TILE_SIZE / 2), (playerPos.y * tileSize.y) - (tSize * (3.5 + (1 * (newLoot.size() - 1)))));
		int i = 1;
		for (auto loot : newLoot)
		{
			std::string name = loot->getName();
			actionText.push_back(sf::Text(name, game->fonts["main_font"], tSize));
			actionText[i].setPosition((playerPos.x * tileSize.x) - (TILE_SIZE / 2), (playerPos.y * tileSize.y) - (tSize * (2.5 + (1 * (i - 1)))));
			switch (loot->getRarity())
			{
			case Item::Rarity::NORM:
				actionText[i].setFillColor(NORM_COLOR);
				break;
			case Item::Rarity::MAGIC:
				actionText[i].setFillColor(MAGIC_COLOR);
				break;
			case Item::Rarity::RARE:
				actionText[i].setFillColor(RARE_COLOR);
				break;
			case Item::Rarity::ULTRA:
				actionText[i].setFillColor(ULTRA_COLOR);
				break;
			}
			actionText[i].setOutlineThickness(1);
			i++;
		}
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
			this->game->queueMsg(enemy->name);
			for (auto eff : enemy->ability.getEffects())
			{
				effs.push_back(eff);
				float dmg = eff.damage.getDamage();
				if (eff.dur > 1)
					this->game->appendMsg(" dealt " + std::to_string((int)dmg) + " " + helper.damagenames[(int)eff.damage.type] + " damage [duration " + std::to_string(eff.dur) + "].");
				else
					this->game->appendMsg(" dealt " + std::to_string((int)dmg) + " " + helper.damagenames[(int)eff.damage.type] + " damage.");
			}
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
	for (auto pos : oldPosBuffer)
	{
		tEnemies[pos.y*this->width + pos.x] = nullptr;
	}
	for (auto enemy : moveBuffer)
	{
		tEnemies[enemy->tilePos.y*this->width + enemy->tilePos.x] = enemy;
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
	int sLoc = itemGenerator->getRand_0X(caves[maxCave].size() - 1);
	sf::Vector2i stairs = caves[maxCave][sLoc];
	cave.setStair(stairs.x, stairs.y);
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
				//this->tiles[y*this->width + x].tileVariant = 5;
				/*break;*/
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

	std::vector<sf::Vector2i> eLocs = caves[maxCave];
	eLocs.erase(eLocs.begin()); // player spawn

	int eCount = eLocs.size() / 50;

	genCaveEntities(eCount, eLocs, { {0,2} });
}

bool inProximity(std::vector<sf::Vector2i> locs, sf::Vector2i newLoc)
{
	for (auto loc : locs)
	{
		if (std::abs(loc.x - newLoc.x) < loc.y)
			return true;
	}
	return false;
}

void Map::genCaveEntities(int eCount, std::vector<sf::Vector2i> locs, std::vector<sf::Vector2i> oldLocs)
{
	if (eCount > 0 && locs.size() > 0)
	{
		int rSize = itemGenerator->getRand_0X(4) + 3;
		int loc = itemGenerator->getRand_0X(locs.size() - 1);
		do
		{
			loc = itemGenerator->getRand_0X(locs.size() - 1);
		} while (inProximity(oldLocs, { loc,rSize }));
		oldLocs.push_back({ loc,rSize });
		sf::Vector2i pos = locs[loc];
		locs.erase(locs.begin() + loc);
		eCount--;
		if (pos.x + rSize >= width)
			pos.x -= (pos.x + rSize + 1) - width;
		if (pos.y + rSize >= height)
			pos.y -= (pos.y + rSize + 1) - height;
		entities.push_back({ 'r',pos.x,pos.y,rSize,rSize });
		genCaveEntities(eCount, locs, oldLocs);
		return;
	}
	return;
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

	Dungeon::Entity pRoom = entities[itemGenerator->getRand_0X(entities.size() - 1)];
	this->spawnPos = { pRoom.x, pRoom.y };

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
				this->tiles[y*this->width + x].tileVariant = 6;
				//this->spawnPos = { x + 1, y };
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
	int roll, bossChance = 20;
	for (auto e : entities)
	{
		if (e.x == spawnPos.x && e.y == spawnPos.y)
			continue;
		switch (e.id)
		{
		case 'r':

			roll = itemGenerator->getRand_100();
			if (roll <= bossChance)
				do {} while (!spawnBossGroupInRoom(e));
			else
				do {} while (!spawnEnemyInRoom(e));
			break;
		default:
			break;
		}
	}
}

bool Map::spawnBossGroupInRoom(Dungeon::Entity e)
{
	float wMod = (itemGenerator->getRand_100() / 100.f), hMod = (itemGenerator->getRand_100() / 100.f);
	sf::Vector2i eSpawnStart = { e.x + (int)((e.w - 1)*wMod), e.y + (int)((e.h - 1)*hMod) };
	Tile* tile;
	tile = getTile(eSpawnStart.x, eSpawnStart.y);
	if (tile->occupied || !tile->passable)
		return false;

	int roll = itemGenerator->getRand_0X(ebases.size() - 1);
	EnemyBase ebase = ebases[roll];
	Ability* e_a = itemGenerator->makeEnemyAbility(level, Item::Rarity::RARE, ebase.melee, ebase.dtype);
	enemies.push_back(new Enemy(ebase.name, game, eSpawnStart, ebase.hp * 2, level + 1, e_a,
		"enemyattack1", true));

	tEnemies[eSpawnStart.y*this->width + eSpawnStart.x] = enemies[enemies.size() - 1];
	getTile(eSpawnStart.x, eSpawnStart.y)->occupied = true;

	int followerCount = 4 * (itemGenerator->getRand_100() / 100.f), ecount = 1;
	for (int i = 0; i < followerCount; i++)
	{
		Ability* a = itemGenerator->makeEnemyAbility(level, Item::Rarity::MAGIC, ebase.melee, ebase.dtype);

		if (ebase.group.first)
		{
			ecount = itemGenerator->getRand_0X(ebase.group.second) + 1;
		}

		for (int i = 0; i < ecount; i++)
		{
			do {
				wMod = (itemGenerator->getRand_100() / 100.f);
				hMod = (itemGenerator->getRand_100() / 100.f);
				eSpawnStart = { e.x + (int)((e.w - 1)*wMod), e.y + (int)((e.h - 1)*hMod) };
				tile = getTile(eSpawnStart.x, eSpawnStart.y);
			} while (tile->occupied || !tile->passable);

			enemies.push_back(new Enemy(ebase.name, game, eSpawnStart, ebase.hp, level, a,
				"enemyattack1", false));

			tEnemies[eSpawnStart.y*this->width + eSpawnStart.x] = enemies[enemies.size() - 1];
			getTile(eSpawnStart.x, eSpawnStart.y)->occupied = true;
		}
		return true;
	}
	return true;
}

bool Map::spawnEnemyInRoom(Dungeon::Entity e)
{
	float wMod = (itemGenerator->getRand_100() / 100.f), hMod = (itemGenerator->getRand_100() / 100.f);
	sf::Vector2i eSpawnStart = { e.x + (int)((e.w - 1)*wMod), e.y + (int)((e.h - 1)*hMod) };
	Tile* tile;
	tile = getTile(eSpawnStart.x, eSpawnStart.y);
	if (tile->occupied)
		return false;

	int ecount = 1;

	int roll = itemGenerator->getRand_0X(ebases.size() - 1);
	EnemyBase ebase = ebases[roll];
	Ability* a = itemGenerator->makeEnemyAbility(level, Item::Rarity::MAGIC, ebase.melee, ebase.dtype);

	if (ebase.group.first)
	{
		ecount = itemGenerator->getRand_0X(ebase.group.second) + 1;
	}

	for (int i = 0; i < ecount; i++)
	{
		do {
			wMod = (itemGenerator->getRand_100() / 100.f);
			hMod = (itemGenerator->getRand_100() / 100.f);
			eSpawnStart = { e.x + (int)((e.w - 1)*wMod), e.y + (int)((e.h - 1)*hMod) };
			tile = getTile(eSpawnStart.x, eSpawnStart.y);
		} while (tile->occupied || !tile->passable);

		enemies.push_back(new Enemy(ebase.name, game, eSpawnStart, ebase.hp, level, a,
			"enemyattack1", false));

		tEnemies[eSpawnStart.y*this->width + eSpawnStart.x] = enemies[enemies.size() - 1];
		getTile(eSpawnStart.x, eSpawnStart.y)->occupied = true;
	}
	return true;
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
	ebases.push_back(EnemyBase{ true,15,"ogre",AbEffect::DamageType::PHYS,{false,0} });
	ebases.push_back(EnemyBase{ false,10,"wraith",AbEffect::DamageType::FIRE,{false,0} });
	ebases.push_back(EnemyBase{ true,4,"rat",AbEffect::DamageType::POIS,{ true,3 } });
}
Map::~Map()
{
}
