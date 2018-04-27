#ifndef ENEMY_H
#define ENEMY_H

#include "AnimationHandler.h"
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Hud.h"
#include "Helper.h"
#include "Ability.h"
#include <stack>
#include "PathFinder.h"

class Enemy
{
public:
	Game * game;

	Helper::Stats stats;

	sf::Sprite crown;
	bool elite;

	bool active, dead;
	int qDmg;
	int hp;
	int maxHp;
	int lvl;
	int mf;
	Ability* queuedAbility = nullptr;
	sf::Sprite sprite;
	sf::Vector2i tilePos;
	sf::Vector2f pos;
	sf::RectangleShape hpBar;
	sf::RectangleShape hpBarBack;
	sf::Sprite waySprite;
	int moveRange;
	float tickCount = 0;
	float queuedTicks = 0;

	Ability ability;
	float speed;

	std::vector<AbEffect::Effect> effs;

	std::vector<sf::Vector2i> wayPoints;

	std::vector<Ability*> abilities;

	bool losToPlayer;

	std::pair<sf::Vector2i, sf::Vector2i> resolveTick(float tickCount);

	void move(sf::Vector2i pos);

	void deactivate();

	std::string name;

	Enemy(std::string spriteName, std::string name, Game* game, sf::Vector2i pos, int hp, int lvl, Ability* attack, std::string atkSound, bool elite)
	{
		this->game = game;
		this->elite = elite;
		this->tilePos = pos;
		this->mf = 0;
		this->lvl = lvl;
		this->maxHp = hp * (1 + (lvl*0.5));
		this->hp = maxHp;
		this->pos = { (float)pos.x*TILE_SIZE,(float)pos.y*TILE_SIZE };
		this->sprite.setTexture(this->game->texmgr.getRef(spriteName));
		this->sprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
		this->sprite.setPosition(this->pos);
		hpBar.setOrigin(0, 0);
		hpBar.setFillColor(sf::Color::Green);
		hpBar.setSize({ TILE_SIZE, 4 });
		hpBarBack.setOrigin(0, 0);
		hpBarBack.setFillColor(sf::Color::Red);
		hpBarBack.setSize({ TILE_SIZE, 4 });
		this->active = false;
		this->dead = false;
		this->waySprite.setTexture(game->texmgr.getRef("cant_select"));
		this->waySprite.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
		this->moveRange = 1;
		this->qDmg = 0;
		this->speed = 1;
		this->name = name;
		losToPlayer = false;
		ability = Ability(*attack);
		/*ability = Ability(this->game, game->texmgr.getRef("poison"), "poison_icon",
			{ 0,3,0.1f }, { 32,32 }, 0, 2, "Default Attack",
			"Melee slash in an arc");*/
		ability.setSound(atkSound);
		/* ability.addEffect(AbEffect(AbEffect::Effect({}, { AbEffect::DamageType::POIS,2,10 }, 1, AbEffect::EffType::INST)));
		ability.setInfo(Ability::AbInfo(0, 1));*/
		if (elite)
		{
			this->mf += 500;
			this->name = "Elite " + this->name;
			crown.setTexture(this->game->texmgr.getRef("elite_crown"));
			crown.setOrigin(TILE_SIZE / 2, TILE_SIZE / 2);
			this->crown.setPosition(this->pos);
		}
	}

	void dealDamage(int dmg);

	sf::Vector2i updateAI(float tickCount, sf::Vector2i playerPos, PathFinder* pf);

	void applyEff(AbEffect::Effect eff);

	bool update(float dt);
	void draw(float dt);

	void drawAbility(float dt);

	~Enemy();
};

#endif