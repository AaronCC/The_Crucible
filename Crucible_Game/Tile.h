#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <time.h>
#include "AnimationHandler.h"
#include <limits>

struct Node {
	enum set {
		Open, Closed, None
	};
	set s;
	bool inOpen() { return (s == set::Open); }
	bool inClosed() { return (s == set::Closed); }

	Node *parent;
	int x, y;
	int f, g, h;

	Node() {
		s = set::None;
		this->parent = nullptr;
		this->g = std::numeric_limits<int>::max();
	}
	Node(int x, int y) {
		this->x = x;
		this->y = y;
		s = set::None;
		this->parent = nullptr;
		this->g = std::numeric_limits<int>::max();
	};
	void P(Node* p) {
		this->parent = p;
	}
	int H(const Node n2)
	{
		int xdiff = std::abs(n2.x - this->x);
		int ydiff = std::abs(n2.y - this->y);
		this->h = (xdiff + ydiff) * 10;
		return this->h;
	}	
	int NewG(const Node n2)
	{
		if (this->x == n2.x || this->y == n2.y)
			return 10 + n2.g;
		else
			return 14 + n2.g;
	}
	int G(const Node n2)
	{
		if (this->x == n2.x || this->y == n2.y)
			this->g = 10 + n2.g;
		else
			this->g = 14 + n2.g;
		return this->g;
	}
	int F()
	{
		this->f = this->g + this->h;
		return this->f;
	}
	bool operator==(const Node n)
	{
		return (n.x == this->x && n.y == this->y);
	}
};

#define TILE_SIZE 32
enum class TileType { STATIC, ANIMATED };

struct TexMap {
	TexMap(std::string name, sf::Vector2u index) { this->name = name; this->index = index; }
	std::string name;
	sf::Vector2u index;
};

std::string tileTypeToStr(TileType type);

class Tile
{
public:

	AnimationHandler animHandler;

	sf::Vector2i position;
	sf::Sprite sprite;

	sf::Uint32 unitHeight;
	sf::Uint32 unitWidth;

	bool fow;
	bool dark_fow;

	Node node;

	TileType tileType;

	/* Tile variant, allowing for different looking versions of the
	* same tile */
	int tileVariant;

	bool passable;
	bool occupied;
	sf::Sprite fowSprite;
	sf::Sprite fowDarkSprite;

	/* Constructor */
	Tile() { }
	Tile(sf::Vector2u tileSize, const unsigned int height, sf::Texture& texture, sf::Texture& fow, sf::Texture& fow_dark,
		const std::vector<Animation>& animations,
		const TileType tileType)
	{
		this->fow = true;
		this->dark_fow = true;
		this->passable = true;
		sf::Vector2u size = texture.getSize();
		this->tileType = tileType;
		this->tileVariant = 0;
		unitHeight = tileSize.y / TILE_SIZE;
		unitWidth = tileSize.x / TILE_SIZE;
		this->animHandler.changeAnim(this->tileVariant);
		this->sprite.setOrigin(sf::Vector2f(tileSize.x / 2, tileSize.y / 2));
		this->sprite.setTexture(texture);
		this->fowSprite.setTexture(fow);
		this->fowSprite.setOrigin({ (float)tileSize.x / 2, (float)tileSize.y / 2 });
		this->fowDarkSprite.setTexture(fow_dark);
		this->fowDarkSprite.setOrigin({ (float)tileSize.x / 2, (float)tileSize.y / 2 });
		this->animHandler.frameSize = sf::IntRect(0, 0, tileSize.x, tileSize.y*height);
		for (auto animation : animations)
		{
			this->animHandler.addAnim(animation);
		}
		this->animHandler.update(0.0f);
		this->occupied = false;
	}
	void setPosition(sf::Vector2i position) {
		this->position = position;
		sf::Vector2f roundedPos = { std::round((float)position.x),std::round((float)position.y) };
		this->sprite.setPosition(roundedPos);
		this->fowSprite.setPosition(roundedPos);
		this->fowDarkSprite.setPosition(roundedPos);
	}
	void setPosition(sf::Vector2i position, int x, int y) {
		this->position = position;
		sf::Vector2f roundedPos = { std::round((float)position.x),std::round((float)position.y) };
		this->sprite.setPosition(roundedPos);
		this->fowSprite.setPosition(roundedPos);
		this->fowDarkSprite.setPosition(roundedPos);
		this->node.x = x;
		this->node.y = y;
		this->node.parent = nullptr;
	}

	void draw(sf::RenderWindow& window, float dt);
	void drawFoW(sf::RenderWindow& window, float dt);

	void reveal();

	void update();

};

#endif /* TILE_H */