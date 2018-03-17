#include "Helper.h"

float Helper::dotProduct(sf::Vector2f v1, sf::Vector2f v2)
{
	return (v1.x * v2.x + v1.y * v2.y);
}

std::vector<std::pair<int, int>> Helper::getNeighbors(int x, int y, int width, int height, int area)
{
	std::vector<std::pair<int, int>> neighbors;
	neighbors.reserve(8);
	for (int yn = y - area; yn <= y + area; yn++)
	{
		if (yn == height || yn < 0)
			continue;
		for (int xn = x - area; xn <= x + area; xn++)
		{
			if (xn == width || xn < 0)
				continue;
			neighbors.push_back({ xn,yn });
		}
	}
	return neighbors;
}
std::vector<std::pair<int, int>> Helper::getNeighbors(int x, int y, int width, int height)
{
	std::vector<std::pair<int, int>> neighbors;
	neighbors.reserve(8);

	for (int yn = y - 1; yn <= y + 1; yn++)
	{
		if (yn == height || yn < 0)
			continue;
		for (int xn = x - 1; xn <= x + 1; xn++)
		{
			if (xn == width || xn < 0 || (xn == x && yn == y))
				continue;
			neighbors.push_back({ xn,yn });
		}
	}

	return neighbors;
}

sf::FloatRect Helper::resizeView(float windowW, float windowH, float aspectRatio)
{
	float windowRatio = windowW / (float)windowH;
	float viewRatio = aspectRatio;
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

	return sf::FloatRect(posX, posY, sizeX, sizeY);
}

float Helper::magnitude(sf::Vector2f vec)
{
	return std::sqrt(std::pow(vec.x, 2) + std::pow(vec.y, 2));
}

sf::Vector2f Helper::normalized(sf::Vector2f vec, float mag)
{
	return { vec.x / mag, vec.y / mag };
}

sf::Vector2f Helper::clamp(sf::Vector2f vec, float max)
{
	float mag = magnitude(vec);
	if (mag > max)
	{
		sf::Vector2f norm = normalized(vec, mag);
		return norm * max;
	}
	else
		return vec;
}

Helper::Helper()
{
}


Helper::~Helper()
{
}
