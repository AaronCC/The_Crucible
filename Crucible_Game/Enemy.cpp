#include "Enemy.h"

std::pair<sf::Vector2i, sf::Vector2i> Enemy::resolveTick(float newTicks)
{
	sf::Vector2i oldPos, newPos;
	std::vector<int> erase;
	for (int i = 0; i < effs.size(); i++)
	{
		AbEffect::Effect& eff = effs[i];
		eff.dur -= 1;
		dealDamage(eff.damage.getDamage());
		if (eff.dur < 1)
		{
			this->stats = this->stats + eff.stats;
			erase.push_back(i);
		}
	}
	int i = 0;
	for (auto e : erase)
	{
		e -= i;
		effs.erase(effs.begin() + e);
		i++;
	}
	oldPos = this->tilePos;
	if (wayPoints.size() > 0)
	{
		move(wayPoints[0]);
		wayPoints.erase(wayPoints.begin());
	}
	newPos = this->tilePos;
	return { oldPos,newPos };
}

void Enemy::move(sf::Vector2i pos)
{
	this->tilePos = pos;
	this->pos = { (float)pos.x*TILE_SIZE,(float)pos.y*TILE_SIZE };
	this->sprite.setPosition(this->pos);
}

void Enemy::deactivate()
{
	this->active = false;
}

void Enemy::dealDamage(int dmg)
{
	qDmg += dmg;
}

sf::Vector2i Enemy::updateAI(float newTicks, sf::Vector2i playerPos, PathFinder* pf)
{
	wayPoints.clear();
	queuedAbility = nullptr;
	sf::Vector2i diff = playerPos - tilePos;
	std::pair<int, int> point = { tilePos.x, tilePos.y };
	this->tickCount += newTicks;
	if ((std::abs(diff.x) <= ability.info.range || std::abs(diff.y) <= ability.info.range) 
		&& losToPlayer && tickCount >= ability.tickCost)
	{
		tickCount -= ability.tickCost;
		queuedAbility = new Ability(ability);
		queuedAbility->pushPosition({ playerPos.x * (float)TILE_SIZE,playerPos.y * (float)TILE_SIZE });
		queuedAbility->activate({ 0,0 }, { 0,0 });
		this->game->sndmgr.playSound(queuedAbility->soundeffect);
		abilities.push_back(queuedAbility);
	}
	else if ((!losToPlayer && std::abs(diff.x) > moveRange || std::abs(diff.y) > moveRange) ||
		(std::abs(diff.x) > moveRange + ability.info.range || std::abs(diff.y) > moveRange + ability.info.range))
	{
		std::vector<std::pair<int, int>> path = pf->findPath(this->tilePos, playerPos);
		for (int i = path.size() - 1; i >= 0; i--)
		{
			point = path[i];
			if (sf::Vector2i{ point.first, point.second } == playerPos)
				continue;
			if (tickCount >= speed)
			{
				wayPoints.push_back({ point.first,point.second });
				tickCount -= speed;
			}
			if (tickCount <= speed)
				break;
		}
	}
	/*else
		tickCount -= newTicks;*/
	if (wayPoints.size() > 0)
		return wayPoints[wayPoints.size() - 1];
	else return { 0,0 };
}

void Enemy::applyEff(AbEffect::Effect eff)
{
	eff.dur -= 1;
	dealDamage(eff.damage.getDamage());
	this->stats = this->stats - eff.stats;
	if (eff.dur > 0)
		effs.push_back(eff);
}

bool Enemy::update(float dt)
{
	if (dead)
		return false;
	this->tilePos.x = std::ceil((pos.x) / (float)TILE_SIZE);
	this->tilePos.y = std::ceil((pos.y) / (float)TILE_SIZE);
	sf::Vector2f halfTSize = { TILE_SIZE / 2, TILE_SIZE / 2 };
	sprite.setPosition(pos);
	float hHeight = hpBar.getSize().y;
	hpBar.setPosition(pos - halfTSize - sf::Vector2f{ 0,hHeight });
	hpBarBack.setPosition(pos - halfTSize - sf::Vector2f{ 0,hHeight });
	if (qDmg != 0)
	{
		this->hp -= qDmg;
		qDmg = 0;
		float p = hp / (float)maxHp;
		this->hpBar.setSize({ p*TILE_SIZE,4 });
		if (hp <= 0)
		{
			/*this->tilePos = { -1,-1 };
			this->pos = { (float)tilePos.x*TILE_SIZE, (float)tilePos.y*TILE_SIZE };*/
			this->deactivate();
			this->dead = true;
			this->game->sndmgr.playSound("death1");
			return true;
		}
	}
	int i = 0;
	std::vector<int> rmCache;
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
	return false;
}

void Enemy::draw(float dt)
{
	if (dead)
		return;
	this->game->window.draw(this->sprite);
	for (auto point : wayPoints)
	{
		this->waySprite.setPosition({ (float)point.x * TILE_SIZE,(float)point.y * TILE_SIZE });
		this->game->window.draw(waySprite);
	}
}
void Enemy::drawAbility(float dt)
{
	for (auto ab : abilities)
		ab->draw(dt);
	if (hp < maxHp)
	{
		this->game->window.draw(this->hpBarBack);
		this->game->window.draw(this->hpBar);;
	}
}
Enemy::~Enemy()
{
}