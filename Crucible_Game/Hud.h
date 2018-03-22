#ifndef HUD_H
#define HUD_H
#include "Game.h"
#include "Item.h"
#include <queue>
#include <string>
#include <sstream>
#include <iterator>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "ItemGenerator.h"
#include "Consumable.h"

#define H_GLOBE "hGlobe"
#define H_POOL "hPool"
#define A_SLOT "aSlot"
#define CAN_SELECT "can_select"
#define CANT_SELECT "cant_select"
#define A_SLOT_COUNT 8

class InvSlot {
#define xOffset 192
#define spacing 40
#define invSlotW 182
#define invSlotH 32
#define infoWidth 296
#define tSize 12
#define charWidth 8
#define slotBorder 2

public:
	sf::Vector2f start{ 125, 110 };
	sf::RectangleShape infoBack;
	sf::RectangleShape slotBack;
	sf::Text slotTypeText;
	sf::Text seperatorText;
	std::vector<sf::Text> itemText;
	std::vector<sf::Text> nameInfoText;
	std::vector<sf::Text> buffInfoText;
	std::vector<sf::Text> baseInfoText;
	bool equipped = false;
	bool selected = false;
	float lineH = tSize + 4;
	int maxInfoChar = infoWidth / charWidth;
	int maxNameChar = invSlotW / charWidth;
	std::vector<std::string> wrapWord(int lineLength, std::string name)
	{
		std::vector<std::string> wrapped;
		int charcount = 0;
		std::vector<std::string> words = chopWord(name);
		std::string newLine = words[0];
		charcount += words[0].length();
		int linecount = 0;
		for (int i = 1; i < words.size(); i++)
		{
			charcount += words[i].length() + 1;
			if (charcount >= lineLength)
			{
				wrapped.push_back(newLine);
				charcount = words[i].length() + 1;;
				linecount++;
				newLine = "";
			}
			newLine += " " + words[i];
		}
		if (newLine != "")
		{
			wrapped.push_back(newLine);
		}
		return wrapped;
	}

	std::vector<std::string> chopWord(std::string word)
	{
		std::istringstream buffer(word);
		std::istream_iterator<std::string> beg(buffer), end;
		std::vector<std::string> words(beg, end);
		return words;
	}

	int getItemSlotType() {
		if (item == nullptr)
			return -1;
		return this->item->slotType;
	}

	Item* getItem() {
		return item;
	}

	void setItem(Item* item) {
		Helper helper;
		this->item = item;
		clearText();
		if (item->name.size() <= 0)
			return;
		int linecount = 0;

		slotTypeText = sf::Text(item->getTypeString(), game->fonts["main_font"], tSize);
		slotTypeText.setFillColor({ 102, 102, 255 });
		seperatorText = sf::Text("____", game->fonts["main_font"], tSize);
		seperatorText.setFillColor({ 102, 102, 255 });

		std::string pre = "", suf = "";
		if (item->rarity == Item::Rarity::MAGIC || item->rarity == Item::Rarity::NORM)
		{
			for (auto buff : item->buffs)
			{
				if (buff.second.getStr() != "")
					if (buff.second.pre)
						pre = buff.second.id;
					else
						suf = buff.second.id;
			}
		}
		else
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			int i = helper.numpreprefixes;
			std::uniform_int_distribution<> dist(0, i - 1);
			pre += helper.inamePrePrefixes[dist(gen)];
			dist.reset();
			dist = std::uniform_int_distribution<>(0, helper.inamePrefixes[item->slotType].size() - 1);
			pre += " " + helper.inamePrefixes[item->slotType][dist(gen)] + ",";
		}
		std::string newname = pre + " " + item->name + " " + suf;
		for (auto line : wrapWord(maxNameChar, newname))
		{
			itemText.push_back(sf::Text(line, game->fonts["main_font"], tSize));
			switch (item->rarity)
			{
			case Item::Rarity::NORM:
				itemText[linecount].setFillColor(NORM_COLOR);
				break;
			case Item::Rarity::MAGIC:
				itemText[linecount].setFillColor(MAGIC_COLOR);
				break;
			case Item::Rarity::RARE:
				itemText[linecount].setFillColor(RARE_COLOR);
				break;
			case Item::Rarity::ULTRA:
				itemText[linecount].setFillColor(ULTRA_COLOR);
				break;
			}
			linecount++;
		}

		linecount = 0;
		for (auto line : wrapWord(maxInfoChar, newname))
		{
			nameInfoText.push_back(sf::Text(line, game->fonts["main_font"], tSize));
			switch (item->rarity)
			{
			case Item::Rarity::NORM:nameInfoText[linecount].setFillColor(NORM_COLOR);
				break;
			case Item::Rarity::MAGIC:nameInfoText[linecount].setFillColor(MAGIC_COLOR);
				break;
			case Item::Rarity::RARE:nameInfoText[linecount].setFillColor(RARE_COLOR);
				break;
			case Item::Rarity::ULTRA:nameInfoText[linecount].setFillColor(ULTRA_COLOR);
				break;
			}
			linecount++;
		}
		linecount = 0;
		for (auto buff : item->getBaseString())
		{
			for (auto line : wrapWord(maxInfoChar, buff.second))
			{
				baseInfoText.push_back(sf::Text(line, game->fonts["main_font"], tSize));
				baseInfoText[linecount].setFillColor(buff.first);
				linecount++;
			}
		}
		linecount = 0;
		for (auto buff : item->getBuffString())
		{
			for (auto line : wrapWord(maxInfoChar, buff.second))
			{
				buffInfoText.push_back(sf::Text(line, game->fonts["main_font"], tSize));
				buffInfoText[linecount].setFillColor(buff.first);
				linecount++;
			}
		}
	}

	void updatePos(sf::Vector2f newPos)
	{
		this->position = newPos;
		this->slotBack.setPosition(this->position);
	}
	void clearText() {
		itemText.clear();
		nameInfoText.clear();
		baseInfoText.clear();
		buffInfoText.clear();
	}
	void removeItem() {
		clearText();
		this->item = nullptr;
	}
	InvSlot() {}
	InvSlot(int index, bool eq, Game* game, int offset)
	{
		this->item = nullptr;
		this->game = game;
		this->equipped = eq;
		if (offset > 1)
			this->start.x += offset * xOffset;
		this->position = sf::Vector2f{
			start.x + (xOffset * (int)eq),
			start.y + ((spacing + ((int)eq*tSize)) * (index)) + ((int)eq*tSize)
		};
		infoBack.setSize({ infoWidth, lineH });
		infoBack.setFillColor(sf::Color::Black);
		infoBack.setOrigin({ 0,0 });
		infoBack.setPosition(this->position);
		infoBack.setOutlineThickness(slotBorder);
		infoBack.setOutlineColor({ 150, 150, 200 });

		slotBack.setSize({ invSlotW, invSlotH });
		slotBack.setFillColor({ 27, 31, 38 });
		slotBack.setOrigin({ 0,0 });
		slotBack.setPosition(this->position);
		slotBack.setOutlineThickness(slotBorder);
		slotBack.setOutlineColor(sf::Color::Black);
		sf::Vector2f padd{ 4, 2 };

	}

	~InvSlot() {}

	bool InvSlot::isHovering(const sf::Vector2f mousePos)
	{
		if (
			mousePos.x >= position.x &&
			mousePos.y >= position.y &&
			mousePos.x <= position.x + invSlotW + (2 * slotBorder) &&
			mousePos.y <= position.y + invSlotH + (2 * slotBorder)
			) return true;
		return false;
	}

	sf::Vector2f position;
private:
	Item * item;
	Game* game;
private:
};

class PlayerInfo {
public:
	Game * game;
	sf::Font font;
	std::vector<sf::Text> info;
	sf::Text title;
	std::vector<std::vector<sf::Text>> abilities;
	Helper helper;

	sf::Vector2f start = { 105, 105 };
	sf::Vector2f abStart = { 255, 115 };

	PlayerInfo(Game* game, Helper::Stats stats)
	{
		this->game = game;
		updateInfo(stats);
	}
	void updateInfo(Helper::Stats stats)
	{
		std::string str, affstr;
		info.clear();
		for (int i = 0; i < helper.numbuffs; i++)
		{
			affstr = stats.buffs.at((Helper::Affix)i).getStr();
			if (affstr == "")
				affstr = "0";
			str = affstr + " " + helper.buffnames[i];
			info.push_back(sf::Text(str, this->game->fonts["main_font"], tSize));
			info[i].setPosition(start + sf::Vector2f{ 0, (float)(tSize + 2)*i });
		}
	}
	void setAbilities(std::vector<Ability*> abilities, Ability* default, Helper::Stats stats)
	{
		this->abilities.clear();
		int i = 0, j = 0;
		std::vector<std::string> abStr;
		sf::Vector2f pos = abStart;
		pos.x += 100;
		for (auto ab : abilities)
		{
			abStr = ab->getBoostedString(&stats);
			j = 0;
			for (auto str : abStr)
			{
				this->abilities.push_back(std::vector<sf::Text>());
				this->abilities[i].push_back(sf::Text(str, game->fonts["main_font"], 12));
				this->abilities[i][j].setPosition(pos);
				j++;
				pos.y += tSize + 2;
			}
			i++;
			pos.y += 2 * tSize;
		}
		abStr = default->getBoostedString(&stats);
		j = 0;
		for (auto str : abStr)
		{
			this->abilities.push_back(std::vector<sf::Text>());
			this->abilities[i].push_back(sf::Text(str, game->fonts["main_font"], 12));
			this->abilities[i][j].setPosition(pos);
			j++;
			pos.y += tSize;
		}
		this->title = sf::Text("Character Sheet", game->fonts["main_font"], tSize);
		this->title.setPosition(start.x, start.y - 30);
	}

	PlayerInfo() {}

	void draw();
};

class Inventory {
#define invHeight 500
#define invWidth 800
public:
	Game * game;
	sf::Font font;
	std::vector<InvSlot> itemSlots;
	std::vector<std::pair<InvSlot, sf::Text>> eqItems;
	std::vector<InvSlot> scrollSlots;
	std::vector<std::pair<InvSlot, sf::Text>> eqScrolls;
	sf::Text invTitle;
	std::pair<sf::RectangleShape, sf::Text> deleteButton;
	sf::Sprite itemTextBack;
	sf::Sprite itemInfoBack;
	sf::RectangleShape slider;
	ItemGenerator itemGenerator;
	sf::Vector2f start{ 100, 100 };
	bool hoveringEq = false;
	enum Hovering {
		ITM,
		EQI,
		SCR,
		EQS,
		NONE
	};
	Hovering hovType;
	Hovering selType;
	int scrollSelect = -1;
	bool lastSelected = false;
	int hovering;
	int selected;
	bool delHover = false;
	bool delSelect = false;
	std::map<std::string, Ability> abilityMap;

	std::vector<Ability*> getScrollAbilities();

	bool isHoveringDelete(sf::Vector2f mousePos)
	{
		sf::Vector2f position = deleteButton.first.getPosition();
		sf::Vector2f size = deleteButton.first.getSize();
		if (
			mousePos.x >= position.x &&
			mousePos.y >= position.y &&
			mousePos.x <= position.x + size.x + (2 * slotBorder) &&
			mousePos.y <= position.y + size.y + (2 * slotBorder)
			) return true;
		return false;
	}
	int firstEmptySlot(std::vector<InvSlot>& slots) {
		for (int i = 0; i < slots.size(); i++)
		{
			if (slots[i].getItem() == nullptr)
			{
				return i;
			}
		}
		return -1;
	}
	bool pickupItem(Item* item);
	bool eqToSlot(int eqIndex, std::vector<InvSlot>& slots, std::vector<std::pair<InvSlot, sf::Text>>& eqSlots)
	{
		if (eqSlots[eqIndex].first.getItem() == nullptr)
			return false;
		int i = firstEmptySlot(slots);
		if (i != -1)
		{
			slots[i].setItem(eqSlots[eqIndex].first.getItem());
			eqSlots[eqIndex].first.removeItem();
			return true;
		}
		return false;
	}
	bool hasOpenSlot()
	{
		for (int i = 0; i < itemSlots.size(); i++)
		{
			if (itemSlots[i].getItem() == nullptr)
			{
				return true;
			}
		}
		return false;
	}
	bool slotToEq(int index, int eqIndex, std::vector<InvSlot>& slots, std::vector<std::pair<InvSlot, sf::Text>>& eqSlots)
	{
		Item* item = slots[index].getItem();
		bool twoHEq = false;
		if (eqSlots[Item::SlotType::MAH].first.getItem() != nullptr)
			twoHEq = eqSlots[Item::SlotType::MAH].first.getItem()->twoHanded;
		if (slots[index].getItem() == nullptr)
			return false;
		if (item->twoHanded)
		{
			eqToSlot(Item::SlotType::OFH, slots, eqSlots);
		}
		else if (item->slotType == Item::SlotType::OFH &&
			twoHEq)
		{
			eqToSlot(Item::SlotType::MAH, slots, eqSlots);
		}
		eqSlots[eqIndex].first.setItem(item);
		slots[index].removeItem();
		return true;
	}
	Scroll* autoScroll;
	//void updateAutoScroll();

	bool showInfo = false;

	Inventory() {}
	Inventory(Game* game) {
		this->game = game;
		this->itemGenerator = ItemGenerator(game);
		this->font = this->game->fonts["main_font"];
		itemTextBack.setTexture(this->game->texmgr.getRef("item_text_back"));
		itemInfoBack.setTexture(this->game->texmgr.getRef("item_info_back"));
		itemTextBack.setOrigin(0, 0);
		itemInfoBack.setOrigin(0, 0);
		invTitle = sf::Text("Inventory", game->fonts["main_font"], tSize);
		invTitle.setPosition({ start.x,  start.y - 30 });
		invTitle.setFillColor(sf::Color::White);
		hovType = Hovering::NONE;
		/* TEMP */
#pragma region ITEMS

		for (int i = 0; i < 25; i++)
		{
			itemSlots.push_back(InvSlot(i, false, this->game, 0));
		}
		//item = new Item("Helm of the All-Seeing God",
		//	{ 0,0,0,12 },
		//	Item::SlotType::HED);
		//itemSlots[0].setItem(item);

		//item = new Item("Ring of Power",
		//	{ 1,1,1,1 },
		//	Item::SlotType::RNG);
		//itemSlots[1].setItem(item);
		//item = new Item("Giant Fang",
		//	{ 20,0,-2,0 },
		//	Item::SlotType::MAH);
		//item->setDamage(AbEffect::DamageType::POIS, 1, 100);
		//itemSlots[2].setItem(item);
		//itemSlots[2].getItem()->twoHanded = true;
		//item = new Item("Sharpened Icicle",
		//	{ 10,0,2,0 },
		//	Item::SlotType::MAH);
		//item->setDamage(AbEffect::DamageType::ICE, 5, 25);
		//itemSlots[3].setItem(item);
		//item = new Item("Cardboard Shield",
		//	{ 0,2,0,-10 },
		//	Item::SlotType::OFH);
		//itemSlots[4].setItem(item);
		for (int i = 0; i < 5; i++)
		{
			Item* item = itemGenerator.makeItem(1, 500);
			itemSlots[i].setItem(item);
		}
		std::vector<std::string> eqNames = { "Head","Body","Main-hand","Off-hand","Ring","Amulet","Cloak","Belt" };
		for (int i = 0; i < 8; i++)
		{
			eqItems.push_back({ InvSlot(i,true,this->game,1),
				sf::Text(eqNames[i], game->fonts["main_font"], tSize) });
			eqItems[i].second.setPosition(eqItems[i].first.position - sf::Vector2f(0, 4 + tSize));
			eqItems[i].second.setFillColor({ 255, 220, 125 });
		}
#pragma endregion
#pragma region SCROLLS
		for (int i = 0; i < 25; i++)
		{
			scrollSlots.push_back(InvSlot(i, false, this->game, 2));
		}
		/*
		abilityMap["slash"] = Ability(this->game, game->texmgr.getRef("slash"), "slash_icon",
			{ 0,3,0.1f }, { 32,32 }, 10, 1, "slash",
			"Melee slash in an arc");
		abilityMap["slash"].setInfo(Ability::AbInfo(Ability::AbPrmType::MELEE, Ability::AbSecType::TARG, 1, 2));
		abilityMap["move"] = Ability(this->game, game->texmgr.getRef("move_icon"), "move_icon",
			{ 0,3,0.1f }, { 32,32 },  10, 2, "Move",
			"Move to target location");
		Ability* slash = new Ability(abilityMap["slash"]);
		scroll = new Scroll("Slash",
			Item::SlotType::SCR,
			slash,
			Item::Rarity::NORM);
		AbEffect eff = AbEffect(AbEffect::Effect({ 0,0,0,0 },
			AbEffect::Damage(
				AbEffect::DamageType::COLD, 1, 10),
			1));
		AbEffect eff2 = AbEffect(AbEffect::Effect({ 0,0,0,0 },
			AbEffect::Damage(
				AbEffect::DamageType::PHYS, 1, 1),
			5));
		slash->addEffect(eff);
		slash->addEffect(eff2);*/
		for (int i = 0; i < 25; i++)
			scrollSlots[i].setItem(itemGenerator.makeScroll(1, 500));
		std::vector<std::string> scEqNames = { "1","2","3","4","5","6","LMB","RMB" };
		for (int i = 0; i < A_SLOT_COUNT; i++)
		{
			eqScrolls.push_back({ InvSlot(i,true,this->game,2),
				sf::Text(scEqNames[i], game->fonts["main_font"], tSize) });
			eqScrolls[i].second.setPosition(eqScrolls[i].first.position - sf::Vector2f(0, 4 + tSize));
			eqScrolls[i].second.setFillColor({ 255, 220, 125 });
		}
		//eqScrolls[6].first.setItem(scroll);
		//autoSlot.first = InvSlot(A_SLOT_COUNT, true, this->game, 2);
		//autoSlot.second = sf::Text("Default Attack", game->fonts["main_font"], tSize);
		//autoSlot.second.setFillColor({ 255, 220, 125 });

#pragma endregion
		/* END */
		slider.setSize({ 10, (invHeight + 10) / ((float)itemSlots.size() - maxScHeight + 2) });
		slider.setFillColor(sf::Color::White);
		slider.setOrigin({ 0,0 });
		slider.setPosition(sf::Vector2f{ 10,10 }+start);
		deleteButton.first.setSize({ invSlotW / 2,invSlotH });
		deleteButton.first.setFillColor(sf::Color::Black);
		deleteButton.first.setOrigin({ 0,0 });
		deleteButton.first.setPosition(sf::Vector2f{ 25 + xOffset,10 + invHeight - (1.5*spacing) }+start);
		deleteButton.first.setOutlineThickness(2.f);
		deleteButton.first.setOutlineColor(sf::Color::Black);
		deleteButton.second = sf::Text("Destroy", game->fonts["main_font"], tSize);
		deleteButton.second.setPosition(deleteButton.first.getPosition() + (deleteButton.first.getSize() / 4.2f));
		deleteButton.second.setFillColor({ 255, 90, 50 });
	}

	~Inventory() {}

	void select() {
		if (delHover)
		{
			if (delSelect)
			{
				if (lastSelected)
				{
					if (selType == Hovering::EQI)
					{
						eqItems[selected].first.removeItem();
						eqItems[selected].first.slotBack.setOutlineColor(sf::Color::Black);
					}
					else
					{
						eqScrolls[selected].first.removeItem();
						eqScrolls[selected].first.slotBack.setOutlineColor(sf::Color::Black);
					}
				}
				else
				{
					if (selType == Hovering::ITM)
					{
						itemSlots[selected].removeItem();
						itemSlots[selected].slotBack.setOutlineColor(sf::Color::Black);
					}
					else
					{
						scrollSlots[selected].removeItem();
						scrollSlots[selected].slotBack.setOutlineColor(sf::Color::Black);
					}
				}
				deleteButton.first.setOutlineColor(sf::Color::Black);
				selected = -1;
				delSelect = false;
				return;
			}
			if (selected >= 0)
			{
				delSelect = true;
				deleteButton.first.setOutlineColor({ 255,220,125 });
			}
		}
		else {
			deleteButton.first.setOutlineColor(sf::Color::Black);
			for (int i = 0; i < eqItems.size(); i++)
				eqItems[i].first.slotBack.setOutlineColor(sf::Color::Black);
			for (int i = 0; i < itemSlots.size(); i++)
				itemSlots[i].slotBack.setOutlineColor(sf::Color::Black);
			for (int i = 0; i < eqItems.size(); i++)
				eqScrolls[i].first.slotBack.setOutlineColor(sf::Color::Black);
			for (int i = 0; i < scrollSlots.size(); i++)
				scrollSlots[i].slotBack.setOutlineColor(sf::Color::Black);
			switch (hovType)
			{
			case Hovering::EQI:
			{
				if (selected == hovering && lastSelected)
				{
					int slotType = eqItems[hovering].first.getItemSlotType();
					eqItems[hovering].first.selected = false;
					eqItems[hovering].first.slotBack.setOutlineColor(sf::Color::Black);
					int selectedSlotType = eqItems[hovering].first.getItemSlotType();
					if (selectedSlotType != -1)
					{
						eqToSlot(selectedSlotType, itemSlots, eqItems);
					}
					selected = -1;
					scrollSelect = -1;
					lastSelected = true;
					selType = Hovering::NONE;
					return;
				}
				selected = hovering;
				lastSelected = true;
				eqItems[hovering].first.selected = true;
				eqItems[hovering].first.slotBack.setOutlineColor({ 255, 220, 125 });
				scrollSelect = -1;
				selType = hovType;
				return;
			}
			case Hovering::ITM:
			{
				if (selected == hovering && !lastSelected)
				{
					int slotType = itemSlots[selected].getItemSlotType();
					itemSlots[selected].selected = false;
					itemSlots[selected].slotBack.setOutlineColor(sf::Color::Black);
					if (slotType != -1)
					{
						eqToSlot(slotType, itemSlots, eqItems);
						slotToEq(selected, slotType, itemSlots, eqItems);
					}
					selected = -1;
					scrollSelect = -1;
					lastSelected = false;
					selType = Hovering::NONE;
					return;
				}
				selected = hovering;
				lastSelected = false;
				itemSlots[selected].selected = true;
				itemSlots[selected].slotBack.setOutlineColor({ 255, 220, 125 });
				scrollSelect = -1;
				selType = hovType;
				break;
			}
			case Hovering::EQS:
			{
				if (selected == hovering && lastSelected)
				{
					int slotType = eqScrolls[hovering].first.getItemSlotType();
					eqScrolls[hovering].first.selected = false;
					eqScrolls[hovering].first.slotBack.setOutlineColor(sf::Color::Black);
					int selectedSlotType = eqScrolls[hovering].first.getItemSlotType();
					if (selectedSlotType != -1)
					{
						eqToSlot(hovering, scrollSlots, eqScrolls);
					}
					selected = -1;
					lastSelected = true;
					selType = Hovering::NONE;
					return;
				}
				selected = hovering;
				selType = hovType;
				if (scrollSelect != -1)
				{
					eqToSlot(selected, scrollSlots, eqScrolls);
					slotToEq(scrollSelect, selected, scrollSlots, eqScrolls);
				}
				else
				{
					lastSelected = true;
					eqScrolls[hovering].first.selected = true;
					eqScrolls[hovering].first.slotBack.setOutlineColor({ 255, 220, 125 });
				}
				scrollSelect = -1;
				return;
			}
			case Hovering::SCR:
			{
				if (selected == hovering && !lastSelected)
				{
					int slotType = scrollSlots[selected].getItemSlotType();
					scrollSlots[selected].selected = false;
					scrollSlots[selected].slotBack.setOutlineColor(sf::Color::Black);
					if (slotType != -1)
					{
						/*eqToSlot(slotType, scrollSlots, eqScrolls);
						slotToEq(selected, slotType, scrollSlots, eqScrolls);*/
					}
					selected = -1;
					lastSelected = false;
					selType = Hovering::NONE;
					return;
				}
				selected = hovering;
				scrollSelect = selected;
				lastSelected = false;
				scrollSlots[selected].selected = true;
				scrollSlots[selected].slotBack.setOutlineColor({ 255, 220, 125 });
				selType = hovType;
				break;
			}
			default:
				scrollSelect = -1;
				break;
			}
			delSelect = false;
		}
	}

	void scrollItems(int delta)
	{
		slotScIndex -= delta;
		if (slotScIndex < 0) {
			slotScIndex = 0;
		}
		else if (slotScIndex >= itemSlots.size()) {
			slotScIndex = itemSlots.size() - 1;
		}
		else if (slotScIndex >= itemSlots.size() - maxScHeight) {
			slotScIndex = itemSlots.size() - maxScHeight;
		}
		slider.setPosition(sf::Vector2f{ 10,10 + (slotScIndex*(float)slider.getSize().y) }+start);
	}

	void draw();
	void update(sf::Vector2f mousePos);
private:
	int slotScIndex = 0;
	unsigned int maxScHeight = invHeight / spacing;
};

class Hud
{
public:
	std::map<std::string, sf::Sprite> elements;
	Game* game;

	sf::RectangleShape conBack;
	std::vector<Consumable> consumables;
	bool conHover = false;
	int conAt;

	sf::Text slotText[A_SLOT_COUNT];
	sf::Font font;

	sf::Vector2f oldMousePos;

	sf::Text msgTitle;

	std::vector<sf::Text> itemSlotNames;

	sf::Vector2f slotStart;
	float slotW = 32.f;

	std::vector<sf::Text> gameMsgs;
	enum ShowState {
		SHOW_MSG,
		SHOW_INV,
		SHOW_INFO,
		SHOW_NONE
	};
	ShowState showState;

	sf::RectangleShape msgBack;

	struct Cooldown {
		float totalTime;
		float timer;
		bool active;
		sf::IntRect bounds;
	};

	std::vector<Cooldown> cooldowns;

	std::map<sf::Keyboard::Key, bool> keys;

	std::map<sf::Keyboard::Key, std::pair<sf::Sprite, bool>> aSlotSprites;
	std::pair<sf::Sprite, bool> lmbSprite;
	std::pair<sf::Sprite, bool> rmbSprite;
	sf::Vector2f msgStart;
	sf::Text conText;
	sf::Vector2f conPos;

	sf::Sprite cdSprite;

	void setSlotSprites(std::vector<std::pair<sf::Keyboard::Key, std::string>> slotSpriteIDs,
		std::string lmbID, std::string rmbID);

	void queueMsg(std::string msg);

	void equipCon();

	void setCooldown(int index, float timer) {
		if (index >= A_SLOT_COUNT)
			return;
		cooldowns[index].active = true;
		cooldowns[index].timer = timer;
		cooldowns[index].totalTime = timer;
		cooldowns[index].bounds = sf::IntRect(slotStart.x + (index * slotW), slotStart.y, slotW, slotW);
	}

	Hud() {};
	Hud(Game * game, std::vector<std::string> eData)
	{
		this->showState = ShowState::SHOW_NONE;
		//font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
		for (int i = 0; i < A_SLOT_COUNT; i++)
		{
			slotText[i].setFont(game->fonts["main_font"]);
			if (i < A_SLOT_COUNT - 2)
				slotText[i].setString(std::to_string(i + 1));
			slotText[i].setOutlineThickness(1);
			slotText[1].setFillColor(sf::Color::White);
		}
		slotText[A_SLOT_COUNT - 2].setString("LMB");
		slotText[A_SLOT_COUNT - 1].setString("RMB");

		unsigned int eCount = 0;
		this->game = game;
		/* Health */
		elements[H_GLOBE].setTexture(game->texmgr.getRef(eData[eCount]));
		elements[H_GLOBE].setPosition(15.f, game->windowSize.y - 15.f);
		elements[H_GLOBE].setOrigin(0, elements[H_GLOBE].getTexture()->getSize().y);
		eCount++;

		elements[H_POOL].setTexture(game->texmgr.getRef(eData[eCount]));
		elements[H_POOL].setPosition(15.f, game->windowSize.y - 15.f);
		elements[H_POOL].setOrigin(0, elements[H_POOL].getTexture()->getSize().y);
		eCount++;

		slotStart = { 95.f, game->windowSize.y - 15.f };
		for (int i = 0; i < A_SLOT_COUNT; i++)
		{
			std::string slotID = A_SLOT + std::to_string(i);
			sf::Vector2f slotPos = { slotStart.x + (slotW * i), slotStart.y };
			elements[slotID].setTexture(game->texmgr.getRef(eData[eCount]));
			elements[slotID].setPosition(slotPos);
			elements[slotID].setOrigin(0, elements[slotID].getTexture()->getSize().y);
			slotText[i].setPosition(slotPos.x + (slotW / 2) - (slotText[i].getString().getSize() * 4.f), slotPos.y - slotW - tSize-2);
			slotText[i].setCharacterSize(tSize);
		}
		eCount++;

		cooldowns.reserve(A_SLOT_COUNT);

		for (int cd = 0; cd < A_SLOT_COUNT; cd++)
		{
			cooldowns.push_back(Cooldown());
			setCooldown(cd, 0);
		}

		cdSprite.setTexture(this->game->texmgr.getRef("cooldown_icon"));
		cdSprite.setOrigin(0, slotW);
		msgBack.setSize({ 800.f,500.f });
		sf::Vector2u diff = this->game->windowSize - sf::Vector2u{ 800, 600 };
		msgBack.setOrigin(0, 0);
		msgBack.setPosition((float)(diff.x / 2), (float)(diff.y / 2));
		msgBack.setFillColor(sf::Color::Black);
		msgBack.setOutlineThickness(4);
		msgBack.setOutlineColor(sf::Color::White);
		keys[sf::Keyboard::Tilde] = false;
		msgStart = sf::Vector2f(this->msgBack.getPosition() + sf::Vector2f{ 6,480 });
		msgTitle = sf::Text("Game Messages", game->fonts["main_font"], tSize);
		msgTitle.setPosition({ msgBack.getPosition().x, msgBack.getPosition().y - 30 });
		msgTitle.setFillColor(sf::Color::White);
		conText = sf::Text("E", game->fonts["main_font"], tSize);
		conText.setOutlineThickness(1);
		conPos = { slotStart + sf::Vector2f(slotW * 9,-slotW) };
		conBack.setSize({ TILE_SIZE, TILE_SIZE });
		conBack.setPosition(conPos);
		conBack.setOutlineThickness(1);
		conBack.setFillColor(sf::Color::Black);
		conText.setPosition(conPos - sf::Vector2f{ 0,tSize+2 });
		consumables.push_back({ game,"hpot",{ 0,Consumable::ConType::H_POT } });
		consumables.push_back({ game,"ppot",{ 0,Consumable::ConType::H_POT } });
		consumables[0].setSpritePos(conPos);
		conAt = 0;
	}

	bool hoverCon(sf::Vector2f mPos);
	void showCons();

	void draw(float dt);
	void updateCD(unsigned int slot, float ticks);
	void checkHover(sf::View view);
	void update(float dt);

	void updateHealth(float dmg);

	bool useConsumable();

	void handleInput();

	~Hud();
};

#endif /* HUD_H */