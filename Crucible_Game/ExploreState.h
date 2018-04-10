#ifndef EXPLORE_STATE_H
#define EXPLORE_STATE_H

#include "GameState.h"
#include "Player.h"
#include "Map.h"
#include "Camera.h"
#include "Helper.h"
#include "PathFinder.h"
#include <random>


class ExploreState : public GameState
{
public:
	ExploreState(Game* game);
	~ExploreState();

	Camera camera;
	Player player;
	int fps;
	int fTotal;
	float fTime;
	std::pair<bool,float> resolving;
	float rTimeTotal = 0;
	float rTime;

	sf::Vector2f oldMousePos;

	std::vector<sf::Vector2i> lightPoints;
	std::vector<Tile*> fowCache;
	sf::Vector2i playerOccToClear;
	std::vector<Ability*> activeAbilities;

	virtual void draw(const float dt);
	virtual void update(const float dt);
	virtual void handleInput();
	void getLightPoints(sf::Vector2i start, int radius);
	void resolveFoW();
	void resolveGameState(float ticks);

private:

	sf::Keyboard::Key oldMoveKey;
	bool old_mLeftState = false;
	bool old_wState = false;
	bool old_sState = false;
	bool old_aState = false;
	bool old_dState = false;
	sf::Font testFont;
	sf::Text testText;
	PathFinder pf;
	Map* map;
};

#endif /* EXPLORE_STATE_H */