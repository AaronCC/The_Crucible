#include "Game.h"
#include "GameState.h"
#include <iostream>
#include <fstream>
#include <sstream>

Game::Game()
{
	
	this->window.create(sf::VideoMode(windowSize.x, windowSize.y), "Game");
	this->window.setFramerateLimit(60);
	tileSize = sf::Vector2u(TILE_SIZE, TILE_SIZE);
	loadTextures();
	loadFonts();
	loadTiles();
}


Game::~Game()
{
}


void Game::pushState(GameState* state)
{
	this->states.push(state);
	return;
}

void Game::popState()
{
	delete this->states.top();
	this->states.pop();

	return;
}

void Game::changeState(GameState* state)
{
	if (!this->states.empty())
		popState();
	pushState(state);

	return;
}

GameState* Game::peekState()
{
	if (this->states.empty()) return nullptr;
	return this->states.top();
}

void Game::gameLoop()
{
	sf::Clock clock;

	while (this->window.isOpen())
	{
		sf::Time elapsed = clock.restart();
		float dt = elapsed.asSeconds();

		if (peekState() == nullptr) continue;
		peekState()->handleInput();
		peekState()->update(dt);
		this->window.clear(sf::Color::Black);
		peekState()->draw(dt);
		this->window.display();
	}
}

void Game::loadTiles()
{
	Animation staticAnim(0, 0, 1.0f);
	/*this->tileAtlas["grass0"] =
		Tile(this->tileSize, 1, texmgr.getRef("grass"),
		{ staticAnim },
			TileType::STATIC,
			TexMap("grass0", sf::Vector2u{ 0,0 }));
	this->tileAtlas["grass1"] =
		Tile(this->tileSize, 1, texmgr.getRef("grass"),
		{ staticAnim },
			TileType::STATIC,
			TexMap("grass1", sf::Vector2u{ 1,0 }));
	this->tileAtlas["grass2"] =
		Tile(this->tileSize, 1, texmgr.getRef("grass"),
		{ staticAnim },
			TileType::STATIC,
			TexMap("grass2", sf::Vector2u{ 2,0 }));*/

	this->tileAtlas["dungeon"] =
		Tile(this->tileSize, 1, texmgr.getRef("dungeon"), texmgr.getRef("fow"), texmgr.getRef("fow_dark"),
		{ 
			staticAnim, // Unused
		    staticAnim, // Floor
		    staticAnim, // Corridor
			staticAnim, // Wall
			staticAnim, // ClosedDoor
			staticAnim, // OpenDoor
			staticAnim, // Downstairs
		},
			TileType::STATIC);

	this->tileAtlas["can_select"] =
		Tile(this->tileSize, 1, texmgr.getRef("can_select"), texmgr.getRef("fow"), texmgr.getRef("fow_dark"),
			{
				staticAnim,
			},
			TileType::STATIC);
	this->tileAtlas["cant_select"] =
		Tile(this->tileSize, 1, texmgr.getRef("cant_select"), texmgr.getRef("fow"), texmgr.getRef("fow_dark"),
			{
				staticAnim,
			},
			TileType::STATIC);
	/*this->tileAtlas["dungeonFloor"] =
		Tile(this->tileSize, 1, texmgr.getRef("dungeon"),
		{ staticAnim },
			TileType::STATIC,
			TexMap("dungeonFloor", sf::Vector2u{ 1,0 }));*/

	Animation walkAnim(0,8,0.1);
	this->tileAtlas["walktest"] =
		Tile(sf::Vector2u(64, 64), 1, texmgr.getRef("walktest"), texmgr.getRef("fow"), texmgr.getRef("fow_dark"),
		{ walkAnim, walkAnim, walkAnim, walkAnim },
			TileType::ANIMATED);
}

void Game::loadTextures()
{
	std::string line;
	std::ifstream texFile("media/Textures.txt");
	if (texFile.is_open())
	{
		while (std::getline(texFile, line))
		{
			std::istringstream iss(line);
			std::vector<std::string> texPair((std::istream_iterator<std::string>(iss)),
				std::istream_iterator<std::string>());
			texmgr.loadTexture(texPair[0], texPair[1]);
		}
		texFile.close();
	}
}
void Game::loadSounds()
{
	std::string line;
	std::ifstream sndFile("media/Sounds.txt");
	if (sndFile.is_open())
	{
		while (std::getline(sndFile, line))
		{
			std::istringstream iss(line);
			std::vector<std::string> sndPair((std::istream_iterator<std::string>(iss)),
				std::istream_iterator<std::string>());
			sndmgr.loadSound(sndPair[0], sndPair[1]);
		}
		sndFile.close();
	}
}

void Game::loadMusic()
{
	std::string line;
	std::ifstream mscFile("media/Music.txt");
	if (mscFile.is_open())
	{
		while (std::getline(mscFile, line))
		{
			std::istringstream iss(line);
			std::vector<std::string> mscPair((std::istream_iterator<std::string>(iss)),
				std::istream_iterator<std::string>());
			sndmgr.loadMusic(mscPair[0], mscPair[1]);
		}
		mscFile.close();
	}
}


void Game::loadFonts()
{
	sf::Font font;
	if (!font.loadFromFile("media/font.ttf"))
	{
		//Error
	}
	this->fonts["main_font"] = font;

	return;
}
