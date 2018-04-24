#ifndef MAP_H
#define MAP_H
#include "Game.h"
#include "Gui.h"
#include "Camera.h"
#include "Helper.h"
#include "Enemy.h"
#include "PathFinder.h"
#include "CaveGen.h"
#include "Loot.h"
#include "Dungeon.h"

class Map
{
public:
	std::vector<Enemy*> enemies;
	std::vector<Enemy*> aEnemies;
	std::vector<Tile> tiles;
	std::vector<Enemy*> tEnemies;
	std::vector<std::vector<Loot*>> tLoot;
	std::vector<Loot*> loot;
	std::vector<Tile>* getTiles() { return &tiles; }
	int width, height;
	sf::Vector2u tileSize;
	sf::Vector2i spawnPos;
	sf::Vector2f drawSize;
	std::vector<sf::Text> hoverText;
	std::vector<sf::Text> actionText;

	struct EnemyBase {
		bool melee;
		int hp;
		std::string name;
		std::pair<bool, int> group;
		AbEffect::DamageType dtype;
		EnemyBase(bool melee, int hp, std::string name, AbEffect::DamageType dtype, std::pair<bool,int> group) :
			melee(melee), hp(hp), name(name), dtype(dtype), group(group) {};
	};
	std::vector<EnemyBase> ebases;

	int level = 0;
	int expCache = 0;

	enum Action {
		PICKUP,
		NONE
	};
	Action action;
	sf::Vector2i oldMouseIndex;
	Tile background;
	Tile canSelect;
	Tile cantSelect;
	ItemGenerator* itemGenerator;
	std::vector<Dungeon::Entity> entities;

	//Tile player;
	Game* game;

	Camera* camera;

	bool activateObjsAtTile(std::vector<sf::Vector2i> pos);

	sf::Vector2i getSelectPos();
	sf::Vector2i mouseIndex;
	Helper helper;
	sf::Vector2i truncLineOfSight(sf::Vector2i from, sf::Vector2i to);

	bool hasLineOfSight(sf::Vector2i from, sf::Vector2i to);

	void drawL2(sf::RenderWindow & window, float dt);

	void drawL3(sf::RenderWindow & window, float dt);

	void draw(sf::RenderWindow & window, float dt);
	sf::Vector2i globalToTilePos(sf::Vector2f global);
	void update(float dt);
	void updateHoverText();
	void handleInput(sf::Event event);
	void eraseLoot(sf::Vector2i pos);
	void resolveAction(Player * player);
	void updateActionText(sf::Vector2i playerPos);

	std::vector<sf::Vector2i> entityOccToClear;

//	std::vector<Enemy*> getEnemiesAtPoint(sf::Vector2i point);

	std::vector<AbEffect::Effect> updateEntityAI(float tickCount, sf::Vector2i pPos, PathFinder* pf);

	void resolveEntityAI(float tickCount);

	void loadCave();

	void genCaveEntities(int eCount, std::vector<sf::Vector2i> locs, std::vector<sf::Vector2i> oldLocs);


	void Map::loadDungeon();

	void populateCave();

	void populateDungeon();

	bool spawnBossGroupInRoom(Dungeon::Entity e);

	bool spawnEnemyInRoom(Dungeon::Entity e);

	Enemy * getEnemyAt(int x, int y);

	Tile * getTile(int x, int y);

	Map(Game* game, Camera* camera);
	~Map();

private:
	Gui buttons;
	sf::Font testFont;
};

#endif /* MAP_H */