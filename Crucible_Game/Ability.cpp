#include "Ability.h"

Ability::~Ability()
{
}

void Ability::activate(sf::Vector2i pPos, sf::Vector2i mPos)
{
	this->isAlive = true;
	this->durTimer = this->duration;
	this->animHandler.changeAnim(0);
	this->animHandler.reset();
}

std::vector<sf::Vector2i> Ability::getArea(sf::Vector2i origin, sf::Vector2i dir)
{
	std::vector<sf::Vector2i> area;
	switch (info.area)
	{
	case Area::TARGET:
		area.push_back(origin);
		break;
	case Area::LINE:
		area.push_back(origin);
		area.push_back(origin + dir);
		area.push_back(origin - dir);
		break;
	case Area::SQUARE:
		for (auto point : helper.getNeighbors(origin.x, origin.y, 1000, 1000, 1))
		{
			area.push_back({ point.first,point.second });
		}
		break;
	case Area::CIRC:
		area.push_back(origin);
		area.push_back(origin + sf::Vector2i{ 0,1 });
		area.push_back(origin + sf::Vector2i{ 0,-1 });
		area.push_back(origin + sf::Vector2i{ 1,0 });
		area.push_back(origin + sf::Vector2i{ -1,0 });
		break;
	}
	return area;
}

std::vector<sf::Vector2i> Ability::getActiveTiles(sf::Vector2i pPos, sf::Vector2i mPos)
{
	sf::Vector2i target;
	sf::Vector2f dir = { (float)pPos.x - (float)mPos.x, (float)pPos.y - (float)mPos.y };
	float mag = helper.magnitude(dir);
	if (mag < info.range)
		target = mPos;
	else
	{
		dir = helper.normalized(dir, mag) * (float)info.range;
		sf::Vector2i tileDir = { (int)std::ceil(std::abs(dir.x)), (int)std::ceil(std::abs(dir.y)) };
		if (dir.x < 0)
			target.x = pPos.x + tileDir.x;
		else if (dir.x >= 0)
			target.x = pPos.x - tileDir.x;
		if (dir.y < 0)
			target.y = pPos.y + tileDir.y;
		else if (dir.y >= 0)
			target.y = pPos.y - tileDir.y;
		if (target.x < 0)
			target.x = 0;
		if (target.y < 0)
			target.y = 0;
	}
	sf::Vector2i intDir;
	if (mPos.x < pPos.x)
	{
		if (mPos.y == pPos.y)
		{
			intDir = { -1, 0 };
		}
		else if (mPos.y < pPos.y)
		{
			intDir = { -1, -1 };
		}
		else
			intDir = { -1,1 };
	}
	else if(mPos.x > pPos.x)
	{
		if (mPos.y == pPos.y)
		{
			intDir = { 1, 0 };
		}
		else if (mPos.y < pPos.y)
		{
			intDir = { 1, -1 };
		}
		else
			intDir = { 1,1 };
	}
	else if (mPos.x == pPos.x)
	{
		if (mPos.y == pPos.y)
		{
			intDir = { 0, 0 };
		}
		else if (mPos.y < pPos.y)
		{
			intDir = { 0, -1 };
		}
		else
			intDir = { 0,1 };
	}
	area = getArea(target, intDir);
	drawPositions.clear();
	return area;
}
void Ability::pushPosition(sf::Vector2f pos)
{
	drawPositions.push_back(pos);
}
void Ability::update(float dt)
{
	durTimer -= dt;
	if (durTimer <= 0)
	{
		this->animHandler.reset();
		this->isAlive = false;
	}
}

void Ability::draw(float dt)
{
	this->animHandler.update(dt);
	this->sprite.setTextureRect(this->animHandler.bounds);
	for (auto pos : drawPositions)
	{
		this->sprite.setPosition(pos);
		this->game->window.draw(this->sprite);
	}
}