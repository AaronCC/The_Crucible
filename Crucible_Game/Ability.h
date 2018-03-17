#ifndef ABILITY_H
#define ABILITY_H

#include "Game.h"
#include "Helper.h"
#include "GenInfo.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Helper.h"
#include <random>
#include <vector>
#include <iostream>
#include <string>

class AbEffect {
public:
	enum DamageType {
		PHYS = 0,
		FIRE = 1,
		COLD = 2,
		LGHT = 3,
		POIS = 4
	};
	struct Damage {
		DamageType type;
		int min, max;
		Damage(DamageType type, int min, int max)
		{
			this->type = type;
			this->min = min;
			this->max = max;
		}
		Damage() {}
		int getDamage() {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dist(0, max - min);
			return dist(gen) + min;
		}
	};
	struct Effect {
		Helper::Stats stats;
		Damage damage;
		int dur;
		Effect(Helper::Stats stats, Damage damage, int dur)
		{
			this->stats = stats;
			this->damage = damage;
			this->dur = dur;
		}
		Effect() {}
	};
	Effect eff;

	virtual Effect getEffects() { return eff; }

	AbEffect(Effect eff)
	{
		this->eff = eff;
	}
	~AbEffect() {}
private:
};
class Ability
{
public:
	static const int numarea = 4;
	std::string areadesc[4] = { " targeted attack", " attack in a line", " attack in a square", " attack in a circle" };
	enum Area {
		TARGET,
		LINE,
		SQUARE,
		CIRC
	};
	enum AbPrmType {
		RANGED,
		MELEE
	};
	enum AbSecType {
		AREA,
		TARG
	};
	struct AbInfo {
		Area area;
		int range;
		AbInfo(int area, int range)
		{
			this->area = (Area)area;
			this->range = range;
		}
		AbInfo() {}
	};
	std::vector<AbEffect> effs;
	AbInfo info;
	std::vector<sf::Vector2f> drawPositions;
	std::vector<sf::Vector2i> area;

	std::vector<sf::Vector2i> getActiveTiles(sf::Vector2i pPos, sf::Vector2i mPos);

	void pushPosition(sf::Vector2f pos);

	sf::Sprite sprite;
	AnimationHandler animHandler;
	Game* game;

	sf::Vector2u size;

	float duration;
	float durTimer;
	bool isAlive;
	float cooldown;
	unsigned int slotNum;
	unsigned int tickCost;
	std::string name;
	std::string description;
	int speed;

	void activate(sf::Vector2i pPos, sf::Vector2i mPos);
	std::vector<sf::Vector2i> getArea(sf::Vector2i origin, sf::Vector2i dir);
	void update(float dt);
	void draw(float dt);

	void resolveCollision() {}

	void setInfo(AbInfo info) { this->info = info; }

	std::vector<AbEffect::Effect> getEffects() {
		std::vector<AbEffect::Effect> effEffs;
		for (auto eff : this->effs)
		{
			effEffs.push_back(eff.getEffects());
		}
		return effEffs;
	}
	void addEffect(AbEffect effect) {
		effs.push_back(effect);
	}

	Ability() {}

	std::vector<std::string> getBoostedString(Helper::Stats* stats)
	{
		std::vector<std::string> str;
		str.push_back(this->name);
		if (info.range == 1)
			str.push_back("Melee" + areadesc[(int)info.area]);
		else
			str.push_back("Ranged" + areadesc[(int)info.area]);
		str.push_back("Range: " + std::to_string(info.range));
		str.push_back("Cooldown: " + std::to_string(cooldown).substr(0,4));
		str.push_back("Cast Time: " + std::to_string((float)tickCost*stats->speed).substr(0, 4));
		for (auto e : effs)
		{
			AbEffect::Damage damage = e.getEffects().damage;
			AbEffect::Effect ef = e.getEffects();
			if (damage.min + damage.max != 0)
			{
				if (ef.dur <= 1)
					str.push_back(std::to_string(damage.min) + "-" + std::to_string(damage.max) + " " + helper.damagenames[(int)damage.type]);
				else
					str.push_back(std::to_string(damage.min * ef.dur) + "-" + std::to_string(damage.max * ef.dur) + " " + helper.damagenames[(int)damage.type] + " over " +
						std::to_string(ef.dur) + " ticks");
			}
		}
		return str;
	}

	Ability(const Ability& a)
	{
		this->isAlive = false;
		this->game = a.game;
		this->sprite = a.sprite;
		this->animHandler = a.animHandler;
		this->duration = a.duration;
		this->cooldown = a.cooldown;
		this->slotNum = a.slotNum;
		this->tickCost = a.tickCost;
		this->name = a.name;
		this->description = a.description;
		this->info = a.info;
		this->texName = a.texName;
		this->effs = a.effs;
	}
	std::string texName;
	Ability(Game* game,
		sf::Texture& texture,
		std::string texName,
		Animation animation,
		sf::Vector2u size,
		float cooldown,
		unsigned int tickCost,
		std::string name,
		std::string description)
	{
		this->name = name;
		this->texName = texName;
		this->tickCost = tickCost;
		this->slotNum = slotNum;
		this->cooldown = cooldown;
		this->size = size;
		this->isAlive = false;
		this->game = game;
		this->sprite.setTexture(texture);
		this->sprite.setOrigin(size.x / 2, size.y / 2);
		this->animHandler.changeAnim(0);
		this->animHandler.frameSize = sf::IntRect(0, 0, size.x, size.y);
		this->animHandler.addAnim(animation);
		this->duration = animation.duration * animation.getLength();
		this->description = description;
	}

	~Ability();
private:
	Helper helper;
};

#endif /* ABILITY_H */