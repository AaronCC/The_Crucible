#ifndef TEST_STATE_H
#define TEST_STATE_H

#include "GameState.h"
#include "Player.h"
#include "Map.h"
#include "Camera.h"
#include "PathFinder.h"

class TestState : public GameState
{

public:
	TestState(Game* game);
	~TestState();

	Camera camera;

	Player player;
	int fps;
	int fTotal;
	float fTime;
	virtual void draw(const float dt);
	virtual void update(const float dt);
	virtual void handleInput();

private:
	bool old_mLeftState = false;
	sf::Font testFont;
	sf::Text testText;
	PathFinder pf;
	Map* map;
};

#endif /* TEST_STATE_H */