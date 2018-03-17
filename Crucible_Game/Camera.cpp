#include "Camera.h"

void Camera::update(float dt)
{
	//sf::Vector2f target = this->player->position;
	//target.x = std::round(target.x);
	//target.y = std::round(target.y);
	//sf::Vector2f center = this->view.getCenter();

	//if (center == target)
	//	return;

	//sf::Vector2f dir = target - center;

	//float mag = helper.magnitude(dir);
	//dir = helper.normalized(dir, mag);

	//if (mag > helper.magnitude(velocity))
	//{
	//	acceleration = dir * speed;
	//	velocity += acceleration;
	//	helper.clamp(velocity, maxSpeed);
	//	this->view.move(velocity * dt);
	//}
	//else
	//{
	//	if (mag < helper.magnitude((dir * speed) * dt))
	//		this->view.setCenter(target);
	//	velocity *= 0.2f;
	//	this->view.move(velocity * dt);
	//}
	this->view.setCenter({ std::round(player->position.x),std::round(player->position.y) });
}

void Camera::setView()
{
	this->game->window.setView(this->view);
}

void Camera::resizeView(float windowW, float windowH)
{
	float windowRatio = windowW / (float)windowH;
	float viewRatio = this->game->aspectRatio;
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

Camera::~Camera()
{
}
