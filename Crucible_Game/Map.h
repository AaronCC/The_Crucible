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

#define SHR_SPN_CHANCE 15

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

	struct Debris {
		bool cluster;
		bool wall;
		std::string texName;
		Debris() {}
		Debris(bool c, bool w, std::string t) :cluster(c), wall(w), texName(t) {}
	};
	std::vector<Debris> debris;

	struct EnemyBase {
		bool melee;
		int hp;
		float mult;
		std::string name, tx_name;
		std::pair<bool, int> group;
		AbEffect::DamageType dtype;
		EnemyBase(bool melee, int hp, std::string name, AbEffect::DamageType dtype, std::pair<bool,int> group, std::string tx_name, float mult) :
			melee(melee), hp(hp), name(name), dtype(dtype), group(group), tx_name(tx_name), mult(mult) {};
	};
	std::vector<EnemyBase> ebases;

	int level = 0;
	int expCache = 0;

	enum Action {
		PICKUP,
		SHRINE,
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

	void drawL2(sf::RenderWindow & window, float dt, sf::Vector2i pPos);

	void drawL3(sf::RenderWindow & window, float dt);

	void resizeMiniView(float windowH, float windowW)
	{
		miniMapView.setViewport(helper.resizeView(windowH, windowW, game->aspectRatio));
	}

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

	sf::RectangleShape youAreHere;
	sf::View miniMapView;
	bool showMiniMap;

	void Map::loadDungeon();

	void populateDungeon(bool debris);

	bool spawnShrine(Dungeon::Entity e);

	bool spawnBossGroupInRoom(Dungeon::Entity e);

	bool spawnDebrisInRoom(Dungeon::Entity e);

	bool spawnEnemyInRoom(Dungeon::Entity e);

	Enemy * getEnemyAt(int x, int y);

	Tile * getTile(int x, int y);

	void Map::parseEbases()
	{
		std::string line;
		std::ifstream texFile("media/Enemies.txt");

		if (texFile.is_open())
		{
			while (std::getline(texFile, line))
			{
				if (line[0] == ';')
					continue;
				std::string name = "", tx_name;
				bool melee, group;
				int hp, dtype, groupSize;
				float mult;
				std::istringstream iss(line);
				std::vector<std::string> results(std::istream_iterator<std::string>{iss},
					std::istream_iterator<std::string>());
				std::string word = results[0];
				name = word;
				int i = 0;
				while (word[word.size() - 1] != ';')
				{
					i++;
					word = results[i];
					name += " " + word;
				}
				name = name.substr(0, name.size() - 1);
				tx_name = results[1+i];
				std::stringstream ss(results[2+i]);
				ss >> melee;
				ss = std::stringstream(results[3+i]);
				ss >> hp;
				ss = std::stringstream(results[4+i]);
				ss >> dtype;
				ss = std::stringstream(results[5+i]);
				ss >> group;
				ss = std::stringstream(results[6+i]);
				ss >> groupSize;
				ss = std::stringstream(results[7+i]);
				ss >> mult;
				ebases.push_back(EnemyBase(melee, hp, name, (AbEffect::DamageType)dtype, { group,groupSize }, tx_name, mult));
			}
		}
	}

	Map(Game* game, Camera* camera);
	~Map();

private:
	Gui buttons;
	sf::Font testFont;
};

#endif /* MAP_H */