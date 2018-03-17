#include <SFML/Graphics.hpp>

#include "Game.h"
#include "TestState.h"
#include "MenuState.h"
	
int main()
{
	Game game;

	//game.pushState(new TestState(&game));
	game.pushState(new MenuState(&game));
	game.gameLoop();

	return 0;
}