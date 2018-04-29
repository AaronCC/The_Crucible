#pragma once
#include "GameState.h"
#include "imgui.h"
#include "imgui-sfml.h"
#include "Helper.h"
#include "ItemGenerator.h"

class EditorState : public GameState
{
private:
	sf::Clock deltaClock;

	int old_mode;
	int old_abase_at = -1;
	int old_file;
	ItemGenerator iGen;
	Helper helper;
	std::vector<std::string> v_names;
	enum EType {
		I_AFFIX,
		I_BASE,
		A_AFFIX,
		A_BASE,
		AE_BASE,
		ENEMY
	};
	EType editing;
public:
	EditorState(Game* game) {
		this->game = game;
		ImGui::SFML::Init(game->window);
		this->game->window.resetGLStates();
		iGen = ItemGenerator(game);
	}
	~EditorState() { ImGui::SFML::Shutdown(); }
	virtual void draw(const float dt);
	virtual void update(const float dt);
	virtual void handleInput();
};