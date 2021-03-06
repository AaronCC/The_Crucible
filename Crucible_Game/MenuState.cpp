#include "MenuState.h"


MenuState::MenuState(Game* game)
{
	this->game = game;
	pathText.setFont(this->game->fonts["main_font"]);
	pathText.setPosition({ (float)this->game->windowSize.x / 2 - ((game->defaultPath.size()+4)*7), (float)this->game->windowSize.y / 2 - 128 });
	pathText.setString("Mode: " + this->game->defaultPath);
	pathText.setOutlineThickness(1);
	pathText.setCharacterSize(24);
	background.setTexture(this->game->texmgr.getRef("mainmenu"));
	Animation btnAnim(0, 0, 1);
	buttons = Gui(
		GuiStyle(sf::Color::Black, sf::Color::White, &testFont, false),
		true,
		{ "beginbutton", "editorbutton" },
		{ { btnAnim }, {btnAnim} },
		{ std::pair<std::string,std::string>("btn1","btn1"), std::pair<std::string,std::string>("btn2","btn2") },
		{ 128, 64 },
		{ { (float)this->game->windowSize.x / 2, (float)this->game->windowSize.y / 2 - 32},
		{(float)this->game->windowSize.x / 2, (float)(this->game->windowSize.y / 2) + 64} },
		EType::BUTTON, 0, game);
	initView();
	this->themeName = "menutheme";
	this->game->sndmgr.getMusicRef(themeName);
}

MenuState::~MenuState()
{
}

void MenuState::draw(const float dt)
{
	this->game->window.draw(background);
	this->game->window.setView(this->view);
	//this->game->window.draw(testText);
	this->buttons.draw(this->game->window, dt);
	this->game->window.draw(pathText);
}

void MenuState::update(const float dt)
{
	this->buttons.update(dt);
	pathText.setPosition({ (float)this->game->windowSize.x / 2 - ((game->defaultPath.size() + 4) * 7), (float)this->game->windowSize.y / 2 - 128 });
	pathText.setString("Mode: " + this->game->defaultPath);
}

void MenuState::handleInput()
{
	sf::Event event;

	sf::Vector2f mousePos = this->game->window.mapPixelToCoords(sf::Mouse::getPosition(this->game->window), this->view);
	
	while (this->game->window.pollEvent(event))
	{
		if (loading)
			break;
		switch (event.type)
		{
		case sf::Event::Closed:
			exit(0);
		case sf::Event::Resized:
		{
			resizeView(event.size.width, event.size.height);
			break;
		}
		case sf::Event::MouseMoved:
		{
			this->buttons.highlight(this->buttons.getElement(mousePos));	
			break;
		}
		case sf::Event::MouseButtonPressed:
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				std::string msg = this->buttons.getEMsg(mousePos);
				if (msg == "btn1")
				{
					loading = true;
					this->game->pushState(new ExploreState(game));
				}
				else if (msg == "btn2")
				{
					loading = true;
					this->game->pushState(new EditorState(game));
				}
			}
			break;
		}
		case sf::Event::KeyReleased:
		{

			break;
		}
		default: break;
		}
	}
	loading = false;
}
