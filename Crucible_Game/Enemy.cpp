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
	sf::Vector2i diff = playerPos - tilePos;
	std::pair<int, int> point = { tilePos.x, tilePos.y };
	if (std::abs(diff.x) > moveRange || std::abs(diff.y) > moveRange)
	{
		wayPoints.clear();
		std::vector<std::pair<int, int>> path = pf->findPath(this->tilePos, playerPos);
		this->tickCount += newTicks;
		for (int i = path.size() - 1; i >= 0; i--)
		{
			if (tickCount >= speed)
			{
				point = path[i];
				wayPoints.push_back({ point.first,point.second });
				tickCount -= speed;
			}
			if (tickCount <= speed)
				break;
		}
	}
	return { point.first,point.second };
}

void Enemy::applyEff(AbEffect::Effect eff)
{
	eff.dur -= 1;
	dealDamage(eff.damage.getDamage());
	this->stats = this->stats - eff.stats;
	if (eff.dur > 0)
		effs.push_back(eff);
}

bool Enemy::update()
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
			return true;
		}
	}
	return false;
}

void Enemy::draw()
{
	if (dead)
		return;
	this->game->window.draw(this->sprite);
	if (hp < maxHp)
	{
		this->game->window.draw(this->hpBarBack);
		this->game->window.draw(this->hpBar);;
	}
	for (auto point : wayPoints)
	{
		this->waySprite.setPosition({ (float)point.x * TILE_SIZE,(float)point.y * TILE_SIZE });
		this->game->window.draw(waySprite);
	}
}

Enemy::~Enemy()
{
}