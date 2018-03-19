#include "Player.h"
//#include "PathFinder.h"
#define TILE_SIZE this->game->tileSize
#define RMB_SLOT 7
#include <cmath>

void Player::resolveLineOfSight(sf::Vector2i los)
{
	queuedAction = Action::ABILITY;
	queuedPoints.clear();
	for (auto tile : queuedAbility->getActiveTiles(tilePos, los))
		addQueuedPoint(tile);
	queuedCooldown = queuedAbility->cooldown;
	checkLineOfSight = true;
	this->tickCount = queuedAbility->tickCost * stats.speed;
}

void Player::resolveMeleeLineOfSight(sf::Vector2i los)
{
	queuedAction = Action::ABILITY;
	queuedPoints.clear();
	for (auto tile : queuedAbility->getActiveTiles(tilePos, los))
		addQueuedPoint(tile);
	queuedCooldown = queuedAbility->cooldown;
	this->tickCount = stats.speed;
}
void Player::resolveAbilityOnTile(sf::Vector2i pos)
{
	queuedAbility->pushPosition({ (float)pos.x*TILE_SIZE.x,(float)pos.y*TILE_SIZE.y });

}
void Player::lockActions()
{
	this->resolveActions = true;
	this->hud.gameMsgs.clear();
	keys[sf::Keyboard::Space] = true;
}
void Player::updatePlayerStats()
{
	bStats.reset();
	for (auto itm_text : inventory.eqItems)
	{
		Item* itm = itm_text.first.getItem();
		if (itm == nullptr)
			continue;
		Helper::Stats stats = itm->getStatBuffs();
		bStats = bStats + stats;
	}
	bStats = bStats + eStats;
	playerInfo.updateInfo(bStats);
}
void Player::handleInput()
{
	this->hud.handleInput();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && keys[sf::Keyboard::Space] == false)
	{
		lockActions();
	}
	else if (keys[sf::Keyboard::Space] == true)
	{
		this->resolveActions = false;
		keys[sf::Keyboard::Space] = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::I) && keys[sf::Keyboard::I] == false)
	{
		updateAbilityBar();
		updatePlayerStats();
		updateAbilities();
		this->playerInfo.setAbilities(this->inventory.getScrollAbilities(), &autoAttack,bStats);
		if (this->hud.showState == Hud::ShowState::SHOW_INV)
		{
			this->hud.showState = Hud::ShowState::SHOW_NONE;
		}
		else
			this->hud.showState = Hud::ShowState::SHOW_INV;
		keys[sf::Keyboard::I] = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::I))
	{
		keys[sf::Keyboard::I] = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tilde) && keys[sf::Keyboard::Tilde] == false)
	{
		updateAbilityBar();
		updatePlayerStats();
		updateAbilities();
		this->playerInfo.setAbilities(this->inventory.getScrollAbilities(), &autoAttack,bStats);
		if (hud.showState == Hud::ShowState::SHOW_MSG)
			hud.showState = Hud::ShowState::SHOW_NONE;
		else
			hud.showState = Hud::ShowState::SHOW_MSG;
		keys[sf::Keyboard::Tilde] = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Tilde))
	{
		keys[sf::Keyboard::Tilde] = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && keys[sf::Keyboard::C] == false)
	{
		updateAbilityBar();
		updatePlayerStats();
		updateAbilities();
		this->playerInfo.setAbilities(this->inventory.getScrollAbilities(), &autoAttack,bStats);
		if (this->hud.showState == Hud::ShowState::SHOW_INFO)
		{
			this->hud.showState = Hud::ShowState::SHOW_NONE;
		}
		else
			this->hud.showState = Hud::ShowState::SHOW_INFO;
		keys[sf::Keyboard::C] = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		keys[sf::Keyboard::C] = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
	{
		health -= 1;
		hud.updateHealth(health / (float)maxHealth);
		if (health <= 0)
		{
			health = maxHealth;
		}
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		if (hud.cooldowns[RMB_SLOT].timer <= 0)
		{
			queuedAbilitySlotNum = RMB_SLOT;
			queueAbility(RMB_SLOT);
		}
	}
	/*if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		if (hud.cooldowns[RMB_SLOT].timer <= 0)
		{
			queueAbility(7);
		}
	}*/
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1))
	{
		if (hud.cooldowns[0].timer <= 0)
		{
			queuedAbilitySlotNum = 0;
			queueAbility(0);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2))
	{
		if (hud.cooldowns[1].timer <= 0)
		{
			queuedAbilitySlotNum = 1;
			queueAbility(1);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3))
	{
		if (hud.cooldowns[2].timer <= 0)
		{
			queuedAbilitySlotNum = 2;
			queueAbility(2);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4))
	{
		if (hud.cooldowns[3].timer <= 0)
		{
			queuedAbilitySlotNum = 3;
			queueAbility(3);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5))
	{
		if (hud.cooldowns[4].timer <= 0)
		{
			queuedAbilitySlotNum = 4;
			queueAbility(4);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num6))
	{
		if (hud.cooldowns[5].timer <= 0)
		{
			queuedAbilitySlotNum = 5;
			queueAbility(5);
		}
	}
}

void Player::queueAutoAttack()
{
	queuedAbility = new Ability(autoAttack);
	queuedAbilitySlotNum = -1;
}

void Player::queueAbility(int slotIndex)
{
	Item* itm = inventory.eqScrolls[slotIndex].first.getItem();
	if (itm == nullptr)
		return;
	std::string abStr = itm->getName();
	if (abStr != "")
	{
		queuedAbility = new Ability(*itm->getAbility());
		checkLineOfSight = true;
	}
}
void Player::handleEvent(sf::Event event)
{
	switch (event.type)
	{
	case sf::Event::Resized:
		hudView.setViewport(helper.resizeView(event.size.width, event.size.height, game->aspectRatio));
		break;
	case sf::Event::MouseLeft:
		break;
	case sf::Event::MouseWheelMoved:
		if (this->hud.showState == Hud::ShowState::SHOW_INV)
		{
			this->inventory.scrollItems(event.mouseWheel.delta);
		}
		break;
	default:
		break;
	}
}

void Player::draw(float dt)
{
	/* Update the animation */
	this->animHandler.update(dt);

	/* Update the sprite */
	this->sprite.setTextureRect(this->animHandler.bounds);

	/* Draw the tile */
	this->game->window.draw(this->sprite);

	for (auto ability : abilities)
	{
		ability->draw(dt);
	}
	if (queuedAction != Action::MOVE || quickMove)
		for (auto point : queuedPoints)
		{
			queueSprite.setPosition(point.x * TILE_SIZE.x, point.y * TILE_SIZE.y);
			this->game->window.draw(queueSprite);
		}
	this->game->window.setView(hudView);
	this->hud.draw(dt);
	if (this->hud.showState == Hud::ShowState::SHOW_INV)
	{
		this->inventory.draw();
	}
	else if (this->hud.showState == Hud::ShowState::SHOW_INFO)
	{
		this->playerInfo.draw();
	}
	return;
}

void Player::updateAnim(sf::View view)
{
	//sf::Vector2f mousePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), view);
	//std::string oldAnim = currentAnim;

	//float xdiff = mousePos.x - position.x;
	//float ydiff = mousePos.y - position.y;
	//sf::Vector2f normalDir = helper.normalized({ xdiff,ydiff },
	//	helper.magnitude({ xdiff,ydiff }));
	//double angle = std::acos(helper.dotProduct({ 0,1 }, normalDir))*(180.0 / 3.141592653589793238463);

	//if (xdiff > 0)
	//	angle += 2 * (180 - angle);

	//this->sprite.setRotation(angle);

	this->animHandler.changeAnim(anims[currentAnim]);
	oldWalkState = walkState;
	//walkState = WalkState::NONE;
}

bool Player::moveNext()
{
	if (wayPoints.size() > 0)
	{
		setPos({ wayPoints.top().x * 32.f, wayPoints.top().y * 32.f });
		wayPoints.pop();
		updateTilePos();
		return true;
	}
	return false;
}

void Player::queueHudMsg(std::queue<std::string> msgs)
{
	while (msgs.size() > 0)
	{
		hud.queueMsg(msgs.front());
		msgs.pop();
	}
}

void Player::update(float dt)
{
	float fX, fY;

	updateAbilities(dt);

	updateTilePos();

	hud.update(dt);

	if (this->hud.showState == Hud::ShowState::SHOW_INV)
		inventory.update(this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), hudView));
}

void Player::updateTilePos()
{
	this->tilePos.x = std::ceil((position.x) / 32);
	this->tilePos.y = std::ceil((position.y) / 32);
	if (tilePos.x < 0)
		tilePos.x = 0;
	if (tilePos.y < 0)
		tilePos.y = 0;
}

void Player::updateAbilities(float dt)
{
	int i = 0;
	std::vector<int> rmCache;
	/*if (rmbCooldown > 0)
		rmbCooldown -= dt;
	if (lmbCooldown > 0)
		lmbCooldown -= dt;
	for (auto cd : keyCooldowns)
	{
		if (cd.second > 0)
			cd.second -= dt;
	}*/
	for (auto ability : abilities)
	{
		ability->update(dt);
		if (!ability->isAlive)
			rmCache.push_back(i);
		i++;
	}
	for (int rm = 0; rm < rmCache.size(); rm++)
	{
		abilities.erase(abilities.begin() + rmCache[rm]);
		if (rm < rmCache.size() - 1)
			rmCache[rm + 1]--;
	}
}
void Player::updateAbilities()
{
	Item* itm = inventory.eqItems[2].first.getItem();
	autoAttack.effs.clear();
	if (itm != nullptr)
		for (auto eff : itm->getEffectFromMAH())
			autoAttack.addEffect(eff);
}
void Player::applyEff(AbEffect::Effect eff)
{
	eff.dur -= 1;
	this->eStats = this->eStats + eff.stats;
	if (eff.dur > 0)
		effs.push_back(eff);
}
void Player::updateAbilityBar()
{
	std::string lmb, rmb = "";
	std::vector<std::pair<sf::Keyboard::Key, std::string>> slotIDs;
	sf::Keyboard::Key keys[6] = {
		sf::Keyboard::Key::Num1,
		sf::Keyboard::Key::Num2,
		sf::Keyboard::Key::Num3,
		sf::Keyboard::Key::Num4,
		sf::Keyboard::Key::Num5,
		sf::Keyboard::Key::Num6
	};
	Item* itm;
	for (int i = 0; i < inventory.eqScrolls.size() - 2; i++)
	{
		itm = inventory.eqScrolls[i].first.getItem();
		if (itm != nullptr)
		{
			slotIDs.push_back({ keys[i], itm->getItemTexName() });
		}
		else
		{
			slotIDs.push_back({ keys[i], "" });
		}
	}
	itm = inventory.eqScrolls[6].first.getItem();
	if (itm != nullptr)
		lmb = itm->getItemTexName();
	itm = inventory.eqScrolls[7].first.getItem();
	if (itm != nullptr)
		rmb = itm->getItemTexName();
	hud.setSlotSprites(slotIDs, lmb, rmb);
}

Player::~Player()
{
}
