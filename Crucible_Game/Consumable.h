#ifndef CONSUMABLE_H
#define CONSUMABLE_H
#include "Game.h"
#include "Helper.h"

class Consumable
{
public:
	enum ConType {
		H_POT,
		S_POT
	};
	int ilvl;

	std::string name;
	sf::RectangleShape back;
	Game* game;

	struct ConEffect {
		int v1;
		ConType type;
		ConEffect():v1(-1) {}
		ConEffect(int v1, ConType type) : v1(v1), type(type) {

		}
	};
	ConEffect getEffect() { return effect; }
	sf::Text quantText;
	void setSpritePos(sf::Vector2f pos) {
		sprite.setPosition(pos);
		back.setPosition(pos);
		quantText.setPosition(pos);
	}
	sf::Sprite sprite;
	int quant;
	void add() { 
		quant++; 
		quantText.setString(std::to_string(quant));
	}
	bool use() {
		quant--;
		quantText.setString(std::to_string(quant));
		if (quant == 0)
			return true;
		return false;
	}
	void draw(sf::RenderWindow & window);
	Consumable(Game* game, std::string texName, ConEffect eff, std::string name, int ilvl)
		: effect(eff) {
		this->game = game;
		this->name = name;
		this->ilvl = ilvl;
		this->sprite.setTexture(this->game->texmgr.getRef(texName));
		back.setSize({ TILE_SIZE, TILE_SIZE });
		back.setOutlineThickness(1);
		back.setFillColor(sf::Color::Black);
		quant = 1;
		quantText = sf::Text("1", this->game->fonts["main_font"], 12);
		quantText.setFillColor(sf::Color::Black);
		quantText.setOutlineThickness(1);
		quantText.setOutlineColor(sf::Color::White);
	}
private:
	ConEffect effect;
};

#endif