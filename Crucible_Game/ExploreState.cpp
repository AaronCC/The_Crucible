#include "ExploreState.h"
#include <queue>
#define PLAYER this->map->player

ExploreState::ExploreState(Game* game)
{
	this->game = game;
	testFont.loadFromFile("C:/Windows/Fonts/Arial.ttf");
	testText.setFont(testFont);
	testText.setPosition(0, 0);
	testText.setString("");
	initView();
	Animation walkAnim(0, 0, 0.1);
	camera = Camera(game, &player);
	map = new Map(game, &camera);
	//map->loadDungeon();
	map->loadCave();
	player = Player(game,
		sf::Vector2u(32, 32),
		this->game->texmgr.getRef("player"),
		{ walkAnim,walkAnim,walkAnim,walkAnim },
		map->spawnPos, &this->camera.view, &this->map->mouseIndex);
	pf = PathFinder(this->map->getTiles(), this->map->width, this->map->height);
	this->old_mLeftState = true;
	this->player.updateTilePos();
	map->getTile(player.tilePos.x, player.tilePos.y)->occupied = true;
	map->itemGenerator = &player.inventory.itemGenerator;
	map->populateDungeon(true);
	player.hud.updateDLevelText(map->level);
	resolveFoW();
	rTime = 0.01f;
	this->game->sndmgr.stopPlaying();
	this->themeName = "exploretheme";
	this->game->sndmgr.getMusicRef(this->themeName);
	this->camera.resizeView(this->game->window.getSize().x, this->game->window.getSize().y);
}

ExploreState::~ExploreState()
{
}

void ExploreState::draw(const float dt)
{
	this->camera.setView();
	this->map->draw(this->game->window, dt);
	if (fTime >= 1)
	{
		fTime = 0;
		fTotal = fps;
		fps = 0;
	}
	fTime += dt;
	fps++;
	testText.setString("FPS: " + std::to_string(fTotal));
	this->player.draw(dt);
	this->map->drawL2(this->game->window, dt, player.tilePos);
	this->player.drawHud(dt);
	//this->game->window.draw(testText);
}

void ExploreState::update(const float dt)
{
	this->game->sndmgr.clearSounds();
	bool shouldResolve = false;
	sf::Vector2f oldPos = player.position;
	this->player.update(dt);
	this->player.updateAnim(this->camera.view);
	std::vector<AbEffect::Effect> effs;
	if (resolving.first)
	{
		/*rTimeTotal += dt;
		if (rTimeTotal >= rTime)
		{*/
		std::queue<std::string> msgs;
		if (player.queuedAction == Player::Action::ABILITY)
		{
			std::vector<AbEffect::Effect> abEffs = player.queuedAbility->getBuffedEffects(&player.bStats);

			for (auto eff : abEffs)
			{
				if (eff.type == AbEffect::EffType::BUFF)
					player.applyEff(eff);
				else
					for (auto point : player.getQueuedPoints())
					{
						Enemy* enemy = map->getEnemyAt(point.x, point.y);
						if (enemy != nullptr)
							enemy->applyEff(eff);
					}
			}
			for (auto point : player.getQueuedPoints())
				if (map->getTile(point.x, point.y)->passable)
				{
					sf::Vector2i los = map->truncLineOfSight(player.tilePos, point);
					player.resolveAbilityOnTile(los);
				}
			player.resolveAbilityCDs(resolving.second);
			effs = map->updateEntityAI(player.tickCount, player.tilePos, &pf);
			for (auto eff : effs)
			{
				player.applyEff(eff);
			}
			map->resolveEntityAI(resolving.second);
			resolving = { false,0 };
			shouldResolve = true;
			player.clearQueuedPoints();
			msgs.push(player.activateQueuedAbility());
		}
		else if (player.queuedAction == Player::Action::MOVE)
		{
			if (player.moveNext())
			{
				resolveFoW();
				msgs.push("Moved");
			}
			else
			{
				map->getTile(player.tilePos.x, player.tilePos.y)->occupied = true;
				this->player.clearWayPoints();
				player.resolveAbilityCDs(resolving.second);
				effs = map->updateEntityAI(resolving.second, player.tilePos, &pf);
				for (auto eff : effs)
				{
					player.applyEff(eff);
				}
				map->resolveEntityAI(resolving.second);
				resolving = { false,0 };
				shouldResolve = true;
				player.quickMove = false;
			}
			//}
			//rTimeTotal = 0;
			player.queueHudMsg(msgs);
		}
	}
	if (player.resolveActions || effs.size() > 0)
		resolveGameState(player.tickCount);
	this->map->update(dt);
	if (map->expCache > 0)
	{
		player.gainExp(map->expCache);
		map->expCache = 0;
	}
	if (shouldResolve)
	{
		map->updateActionText(player.tilePos);
		map->updateHoverText();
		resolveFoW();
	}
	this->camera.update(dt);
}

void ExploreState::handleInput()
{
	sf::Event event;
	std::queue<std::string> msgs;
	bool act = false;
	moveQueued = false;
	while (this->game->window.pollEvent(event))
	{
		player.handleEvent(event);
		switch (event.type)
		{
		case sf::Event::GainedFocus:
			this->game->focus = true;
			break;
		case sf::Event::LostFocus:
			this->game->focus = false;
			break;
		case sf::Event::Closed:
			exit(0);
		case sf::Event::Resized:
		{
			this->camera.resizeView(event.size.width, event.size.height);
			this->map->resizeMiniView(event.size.width, event.size.height);
			break;
		}
		case sf::Event::KeyReleased:
		{

		}
		default: break;
		}
	}
	if (!this->game->focus)
		return;
	sf::Vector2f mousePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->view);
	bool hudHover = mousePos.y >= game->hudTop ? true : false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		player.hud.showState = Hud::ShowState::SHOW_NONE;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M) && old_mState == false)
	{
		map->showMiniMap = !map->showMiniMap;
		old_mState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::M))
		old_mState = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && old_pState == false)
	{
		this->game->sndmgr.mute();
		old_pState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::P))
		old_pState = false;

	if ((map->action == Map::Action::PICKUP && sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		|| (map->action == Map::Action::SHRINE && sf::Keyboard::isKeyPressed(sf::Keyboard::G)))
	{
		map->resolveAction(&player);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && map->hoverText.size() > 0)
	{
		map->updateHoverText();
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && old_mLeftState == false)
	{
		if (player.hud.levelCache > 0)
		{
			std::pair<int, int> stat = player.hud.checkLevelClick();
			if (stat.first != -1)
			{
				player.levelUp(stat);
				old_mLeftState = true;
				return;
			}
		}
		if (player.hud.conHover)
		{
			player.hud.equipCon();
		}
		if (player.hud.showState == Hud::ShowState::SHOW_INV && (player.inventory.showInfo || player.inventory.delHover))
		{
			player.inventory.select();
		}
		if (!player.engaged)
		{
			if (player.hud.showState == Hud::ShowState::SHOW_NONE && !hudHover && !player.hud.conHover)
			{
				std::vector<std::pair<int, int>> path = pf.findPath(this->player.tilePos, this->map->mouseIndex);
				this->player.clearWayPoints();
				player.quickMove = true;
				for (auto point : path)
				{
					this->player.addWayPoint(point);
				}
				this->player.queuedAction = Player::Action::MOVE;
			}
		}
		else
		{
			msgs.push("Cannot quick-move during combat. Use WSAD.");
		}
		old_mLeftState = true;
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
		old_mLeftState = false;
	std::pair<int, int> point;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && old_zState == false)
	{
		point = { player.tilePos.x - 1,player.tilePos.y - 1 };
		moveQueued = true;
		old_zState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		old_zState = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X) && old_xState == false)
	{
		point = { player.tilePos.x + 1,player.tilePos.y - 1 };
		moveQueued = true;
		old_xState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::X))
		old_xState = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && old_cState == false)
	{
		point = { player.tilePos.x - 1,player.tilePos.y + 1 };
		moveQueued = true;
		old_cState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		old_cState = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::V) && old_vState == false)
	{
		point = { player.tilePos.x + 1,player.tilePos.y + 1 };
		moveQueued = true;
		old_vState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::V))
		old_vState = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && old_wState == false)
	{
		point = { player.tilePos.x,player.tilePos.y - 1 };
		moveQueued = true;
		old_wState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		old_wState = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && old_sState == false)
	{
		point = { player.tilePos.x,player.tilePos.y + 1 };
		moveQueued = true;
		old_sState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		old_sState = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && old_aState == false)
	{
		point = { player.tilePos.x - 1,player.tilePos.y };
		moveQueued = true;
		old_aState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		old_aState = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && old_dState == false)
	{
		point = { player.tilePos.x + 1,player.tilePos.y };
		moveQueued = true;
		old_dState = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		old_dState = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
	{
		if (this->map->getTile(player.tilePos.x, player.tilePos.y)->tileVariant == 6)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dist(0, 1);
			bool cave = dist(gen);
			int aLvl = map->level;
			delete map;
			map = new Map(game, &camera);
			cave = true;
			if (cave)
			{
				map->loadCave();
			}
			else
			{
				map->loadDungeon();
			}
			playerOccToClear = { -1, -1 };
			pf = PathFinder(this->map->getTiles(), this->map->width, this->map->height);
			this->old_mLeftState = true;
			player.setPos(sf::Vector2f{ map->spawnPos.x*(float)TILE_SIZE,map->spawnPos.y*(float)TILE_SIZE });
			this->player.updateTilePos();
			map->getTile(player.tilePos.x, player.tilePos.y)->occupied = true;
			map->itemGenerator = &player.inventory.itemGenerator;
			map->level = aLvl + 1;
			map->populateDungeon(true);
			player.hud.updateDLevelText(map->level);
			fowCache.clear();
			resolveFoW();
		}
	}
	if (moveQueued)
	{
		if (point.first > map->width) point.first = map->width - 1;
		if (point.first < 0) point.first = 0;
		if (point.second > map->height) point.second = map->height - 1;
		if (point.second < 0) point.second = 0;

		Tile* tile = map->getTile(point.first, point.second);
		if (tile->passable)
		{
			act = true; if (map->getEnemyAt(point.first, point.second) != nullptr)
			{
				player.queueAutoAttack();
				player.resolveMeleeLineOfSight({ point.first,point.second });
			}
			else
			{
				if (playerOccToClear != sf::Vector2i{ -1, -1 })
				{
					map->getTile(playerOccToClear.x, playerOccToClear.y)->occupied = false;
					playerOccToClear = { -1, -1 };
				}
				this->player.clearWayPoints();
				this->player.addWayPoint(point);
				this->player.queuedAction = Player::Action::MOVE;
				map->getTile(player.tilePos.x, player.tilePos.y)->occupied = false;
				map->getTile(point.first, point.second)->occupied = true;
				playerOccToClear = { point.first, point.second };
			}
		}
	}
	this->player.handleInput();
	if (player.checkLineOfSight)
	{
		player.resolveLineOfSight(map->truncLineOfSight(player.tilePos, map->mouseIndex));
		player.checkLineOfSight = false;
	}

	oldMousePos = mousePos;

	sf::Vector2f center = view.getCenter();
	player.queueHudMsg(msgs);
	if (act)
		resolveGameState(player.tickCount);
}
void ExploreState::getLightPoints(sf::Vector2i start, int radius)
{
	if (start.x < 0 || start.y < 0 || start.x >= map->width || start.y >= map->height)
		return;
	Tile* tile = map->getTile(start.x, start.y);
	if (!tile->passable)
	{
		tile->reveal();
		fowCache.push_back(tile);
		lightPoints.push_back(start);
		return;
	}
	if (radius == 0)
		return;
	if (tile->fow)
	{
		tile->reveal();
		fowCache.push_back(tile);
		lightPoints.push_back(start);
	}
	getLightPoints({ start.x + 1, start.y }, radius - 1);
	getLightPoints({ start.x - 1, start.y }, radius - 1);
	getLightPoints({ start.x, start.y + 1 }, radius - 1);
	getLightPoints({ start.x, start.y - 1 }, radius - 1);
	return;
}
void ExploreState::resolveFoW()
{
	lightPoints.clear();
	for (auto tile : fowCache)
	{
		tile->fow = true;
	}
	fowCache.clear();
	getLightPoints(player.tilePos, player.lightRadius);
	/*for (int y = -player.lightRadius; y <= player.lightRadius; y++)
	{
		for (int x = -player.lightRadius; x <= player.lightRadius; x++)
		{
			sf::Vector2i pos = { x + player.tilePos.x, y + player.tilePos.y };
			if (pos.x < 0 || pos.y < 0 || pos.x > map->width - 1 || pos.y > map->height - 1)
				continue;
			Tile* tile = map->getTile(pos.x, pos.y);
			tile->reveal();
			points.push_back({ pos.x,pos.y });
			fowCache.push_back(tile);
		}
	}*/
	if (map->activateObjsAtTile(lightPoints))
	{
		if (!player.engaged)
			resolving.second = 0;
		player.engage();
		player.clearWayPoints();
	}
	else
		player.engaged = false;
}

void ExploreState::resolveGameState(float ticks)
{
	resolving = { true, ticks };
	if (player.queuedAction == Player::Action::MOVE)
		map->getTile(player.tilePos.x, player.tilePos.y)->occupied = false;
	std::vector<int> erase;
	for (int i = 0; i < player.effs.size(); i++)
	{
		AbEffect::Effect& eff = player.effs[i];
		eff.dur -= 1;
		if (eff.damage.min != -1 && eff.damage.max != -1)
		{
			float dmg = eff.damage.getDamage();
			if (eff.damage.type == AbEffect::DamageType::PHYS)
				dmg = player.applyAR(dmg);
			else
				dmg = player.applyRES(dmg, eff.damage.type);
			player.damage(dmg);
		}
		if (eff.dur < 1)
		{
			player.eStats = player.eStats - eff.stats;
			erase.push_back(i);
		}
	}
	int i = 0;
	for (auto e : erase)
	{
		e -= i;
		player.effs.erase(player.effs.begin() + e);
		i++;
	}
}

