#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <stack>

#include "TextureManager.h"
#include "SoundManager.h"
#include "Tile.h"

#define TILE_SIZE 32

class GameState;

class Game
{
public: 

	const float aspectRatio = 800 / (float)600;
	const sf::Vector2u windowSize{ 800,600 };
	std::stack<GameState*> states;

	void pushState(GameState* state);
	void popState();
	void changeState(GameState* state);
	GameState* peekState();

	std::map<std::string, Tile> tileAtlas;
	//std::map<std::string, GuiStyle> stylesheets;
	std::map<std::string, sf::Font> fonts;
	sf::Vector2u tileSize;

	sf::RenderWindow window;
	TextureManager texmgr;
	SoundManager sndmgr;

	void gameLoop();

	Game();
	~Game();

private:

	void loadTiles();

	void loadTextures();

	void loadSounds();

	void loadMusic();

	void loadFonts();
};

#endif /* GAME_H */