#ifndef CAMERA_H
#define CAMERA_H

#include "Game.h"
#include "Player.h"
#include "Helper.h"

class Camera
{
public:
	sf::View view;
	
	Player* player;
	Game* game;

	Helper helper;

	sf::Vector2f acceleration;
	sf::Vector2f velocity;
	float speed;
	float maxSpeed;

	void update(float dt);

	void setView();

	void resizeView(float windowW, float windowH);
	Camera() {};
	Camera(Game* game, Player* player)
	{
		this->speed = 115.f;
		this->maxSpeed = 125.f;
		this->player = player;
		this->game = game; 
		this->view = { sf::Vector2f(this->game->windowSize.x / 2, this->game->windowSize.y / 2),
			sf::Vector2f(this->game->windowSize.x, this->game->windowSize.y) };
	}
	~Camera();
};

#endif /* CAMERA_H */