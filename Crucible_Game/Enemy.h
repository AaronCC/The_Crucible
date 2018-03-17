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

	bool active, dead;
	int qDmg;
	int hp;
	int maxHp;
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

	std::pair<sf::Vector2i, sf::Vector2i> resolveTick(float tickCount);

	void move(sf::Vector2i pos);

	void deactivate();

	Enemy(std::string spriteName, Game* game, sf::Vector2i pos, int hp)
	{
		this->game = game;
		this->hp = hp;
		this->maxHp = hp;
		this->tilePos = pos;
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
		ability = Ability(this->game, game->texmgr.getRef("slash"), "slash_icon",
			{ 0,3,0.1f }, { 32,32 }, 10, 2, "slash",
			"Melee slash in an arc");
		ability.setInfo(Ability::AbInfo(0, 1));
	}

	void dealDamage(int dmg);

	sf::Vector2i updateAI(float tickCount, sf::Vector2i playerPos, PathFinder* pf);

	void applyEff(AbEffect::Effect eff);

	bool update();
	void draw();

	~Enemy();
};

#endif