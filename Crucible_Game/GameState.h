#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "Game.h"

class GameState
{
public:

	Game* game;

	sf::View view;

	virtual void draw(const float dt) = 0;
	virtual void update(const float dt) = 0;
	virtual void handleInput() = 0;

	void initView() {
		view = { sf::Vector2f(this->game->windowSize.x / 2, this->game->windowSize.y / 2),
			sf::Vector2f(this->game->windowSize.x, this->game->windowSize.y) };
	}

	void updateView(sf::Vector2f center)
	{
		view.setCenter(center);
	}

	void GameState::resizeView(float windowW, float windowH)
	{
		float windowRatio = windowW / (float)windowH;
		float viewRatio =  this->game->aspectRatio;
		float sizeX = 1;
		float sizeY = 1;
		float posX = 0;
		float posY = 0;

		bool horizontalSpacing = true;
		if (windowRatio < viewRatio)
			horizontalSpacing = false;

		if (horizontalSpacing) {
			sizeX = viewRatio / windowRatio;
			posX = (1 - sizeX) / 2.f;
		}
		else {
			sizeY = windowRatio / viewRatio;
			posY = (1 - sizeY) / 2.f;
		}

		view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
	}

};

#endif /*GAME_STATE_H*/