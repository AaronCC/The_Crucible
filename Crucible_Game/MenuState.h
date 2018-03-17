#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "GameState.h"
#include "Gui.h"
#include "TestState.h"
#include "ExploreState.h"

class MenuState : public GameState
{
public:
	MenuState(Game* game);
	~MenuState();	
	
	virtual void draw(const float dt);
	virtual void update(const float dt);
	virtual void handleInput();

private:
	Gui buttons;
	sf::Font testFont;
	sf::Text testText;
};

#endif /* MENU_STATE_H*/
