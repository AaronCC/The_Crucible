#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "GameState.h"
#include "Gui.h"
#include "TestState.h"
#include "ExploreState.h"
#include "EditorState.h"

class MenuState : public GameState
{
public:
	MenuState(Game* game);
	~MenuState();	
	
	virtual void draw(const float dt);
	virtual void update(const float dt);
	virtual void handleInput();
	bool loading = false;
private:
	Gui buttons;
	sf::Font testFont;
	sf::Text testText;
	sf::Text pathText;
	sf::Sprite background;
};

#endif /* MENU_STATE_H*/
