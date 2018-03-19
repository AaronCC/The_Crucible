#ifndef ITEM_GENERATOR
#define ITEM_GENERATOR
#include "Item.h"
#include "Helper.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define BaseItem Item::BaseItem
#define ST Item::SlotType
#define DMG AbEffect::DamageType

class ItemGenerator
{
public:
	Game * game;
	static const int numEfType = 4;
	enum EfType {
		INST,
		DOT,
		BUFF,
		DEBUFF
	};
	struct AbBase {
		std::string texName;
		std::string iconName;
		std::string description;
		std::string name;
		Animation anim;
		int cd, tc;
		AbBase(std::string tn, std::string in, std::string desc, std::string n, Animation anim, int cd, int tc) :
			texName(tn), iconName(in), description(desc), name(n), anim(anim), cd(cd), tc(tc) {}
	};

	std::vector<AbBase> abBases;

	std::map<ST, std::vector<std::pair<int, BaseItem>>> bases;
	std::map<EfType, std::vector<std::pair<AbEffect::Effect, int>>> abAffixes;

	std::map<AF, std::vector<std::pair<AFV, int>>> suffixes;
	std::map<AF, std::vector<std::pair<AFV, int>>> prefixes;

	void addSuffix(AF af, AFV afv, int ilvl) {
		suffixes[af].push_back({ afv,0 });
	}
	void addPrefix(AF af, AFV afv, int ilvl) {
		prefixes[af].push_back({ afv,ilvl });
	}
	void addAbAffix(EfType type, AbEffect::Effect ef, int ilvl) {
		abAffixes[type].push_back({ ef ,ilvl });
	}

	bool parseSuffix(std::vector<std::string> data)
	{
		bool p = false;
		Helper helper;
		std::stringstream ss;
		if (data.size() < 6)
			return false;
		int a1 = 0, a2 = 0, i = 0, afInt = 0, lInt = 0, dtypeInt = 0;
		ss = std::stringstream(data[0]);
		ss >> a1;
		ss = std::stringstream(data[1]);
		ss >> a2;
		ss = std::stringstream(data[2]);
		ss >> i;
		ss = std::stringstream(data[3]);
		ss >> afInt;
		ss = std::stringstream(data[4]);
		ss >> lInt;
		ss = std::stringstream(data[5]);
		ss >> dtypeInt;
		AF af = (AF)afInt;
		bool l = (bool)lInt;
		DMG dtype = (DMG)dtypeInt;
		sf::Color c = helper.damageColors[dtype];
		std::string name = " of " + data[6];
		AFV afv = AFV(a1, a2, name, p, c, l);
		addSuffix(af, afv, i);
		return true;
	}
	bool parsePrefix(std::vector<std::string> data)
	{
		bool p = true;
		Helper helper;
		std::stringstream ss;
		if (data.size() < 6)
			return false;
		int a1 = 0, a2 = 0, i = 0, afInt = 0, lInt = 0, dtypeInt = 0;
		ss = std::stringstream(data[0]);
		ss >> a1;
		ss = std::stringstream(data[1]);
		ss >> a2;
		ss = std::stringstream(data[2]);
		ss >> i;
		ss = std::stringstream(data[3]);
		ss >> afInt;
		ss = std::stringstream(data[4]);
		ss >> lInt;
		ss = std::stringstream(data[5]);
		ss >> dtypeInt;
		AF af = (AF)afInt;
		bool l = (bool)lInt;
		DMG dtype = (DMG)dtypeInt;
		sf::Color c = helper.damageColors[dtype];
		std::string name = data[6];
		AFV afv = AFV(a1, a2, name, p, c, l);
		addPrefix(af, afv, i);
		return true;
	}
	bool parseBase(std::vector<std::string> data)
	{
		Helper helper;
		std::stringstream ss;
		int slot = 0;
		ss = std::stringstream(data[0]);
		ss >> slot;
		Item::SlotType type = (Item::SlotType)slot;
		int i = 2, a1 = 0, a2 = 0, dtypeInt = 0, afInt = 0, twohInt = 0, lInt;
		bool twoh;
		ss = std::stringstream(data[1]);
		ss >> twohInt;
		twoh = (bool)twohInt;
		std::string name = "";
		while (data[i][data[i].size() - 1] != ';')
		{
			name += data[i] + " ";
			i++;
		}
		name += data[i].substr(0,data[i].size()-1);
		ss = std::stringstream(data[i+1]);
		ss >> a1;
		ss = std::stringstream(data[i+2]);
		ss >> a2; 
		ss = std::stringstream(data[i + 3]);
		ss >> dtypeInt;		
		ss = std::stringstream(data[i + 4]);
		ss >> afInt;
		ss = std::stringstream(data[i + 5]);
		ss >> lInt;
		bool local = (bool)lInt;
		DMG dtype = (DMG)dtypeInt;
		sf::Color c = helper.damageColors[dtype];
		AFV afv = { a1, a2, "Base", true, c, local };
		AF af = (AF)afInt;
		BaseItem* bi = new BaseItem(type, name, twoh);
		bi->addAff(af, afv);
		bases[type].push_back({ 0,*bi });
		return true;
	}
	bool parseAbBase(std::string data, std::string name)
	{

		return false;
	}
	bool parseAbAff(std::string data, std::string name)
	{

		return false;
	}
	void loadAffixes() {
		std::string line;
		std::ifstream texFile("media/Affixes.txt");
		enum Parsing {
			SUF,
			PRE,
			BAS,
			AB_AF,
			AB_BAS,
			NA
		};
		Helper helper;
		Parsing parsing = Parsing::NA;
		if (texFile.is_open())
		{
			while (std::getline(texFile, line))
			{
				if (line[0] == ';')
					continue;
				std::istringstream iss(line);
				std::vector<std::string> results(std::istream_iterator<std::string>{iss},
					std::istream_iterator<std::string>());
				if (results.size() == 0)
					break;
				std::stringstream ss(results[0]);
				int type = 0;
				ss >> type;
				if (type == 0)
				{
					std::stringstream ss(results[4]);
					int afftype = 0;
					ss >> afftype;
					if (afftype <= helper.sRange.second)
						type = 0;
					else
						type = 1;
				}
				parsing = (Parsing)type;
				results.erase(results.begin());
				switch (parsing)
				{
				case Parsing::SUF:
					parseSuffix(results);
					std::cout << "\nadded suffix";
					break;
				case Parsing::PRE:
					parsePrefix(results);
					std::cout << "\nadded prefix";
					break;
				case Parsing::BAS:
					parseBase(results);
					std::cout << "\nadded base";
					break;
				case Parsing::AB_AF:
					parseAbAff(results[1], results[2]);
					std::cout << "\nadded ab affix";
					break;
				case Parsing::AB_BAS:
					parseAbBase(results[1], results[2]);
					std::cout << "\nadded ab base";
					break;
				default:
					break;
				}
			}
			texFile.close();
		}
	}

	std::map<AF, std::vector<int>> afTypeExMap;

	ItemGenerator() {}
	ItemGenerator(Game* game)
	{
		Helper helper;
		this->game = game;
		loadAffixes();

		afTypeExMap[AF::FIRE_FLT_DMG] = { 0,1,3,4,5,6,7,8 };
		afTypeExMap[AF::COLD_FLT_DMG] = { 0,1,3,4,5,6,7,8 };
		afTypeExMap[AF::LGHT_FLT_DMG] = { 0,1,3,4,5,6,7,8 };
		afTypeExMap[AF::POIS_FLT_DMG] = { 0,1,3,4,5,6,7,8 };
		afTypeExMap[AF::PHYS_FLT_DMG] = { 0,1,3,4,5,6,7,8 };
		afTypeExMap[AF::FIRE_RES] = { 2 };
		afTypeExMap[AF::COLD_RES] = { 2 };
		afTypeExMap[AF::POIS_RES] = { 2 };
		afTypeExMap[AF::LGHT_RES] = { 2 };
		afTypeExMap[AF::PHYS_PRC_L_DMG] = { 0,1,3,4,5,6,7,8 };
		afTypeExMap[AF::ARM_RAT] = { 2,4,5 };

		//BaseItem* itm = new BaseItem(ST::MAH, "Elegant Sword");
		/*itm->addAff(AF::PHYS_FLT_DMG, { 1, 5, "Base", true, helper.damageColors[DMG::PHYS],true });
		bases[ST::MAH].push_back({ 0,*itm });*/

		abBases.push_back(AbBase("slash", "slash_icon", "Melee slash in an arc", "Slash",
			{ 0,3,0.1f }, 10, 1));

		abAffixes[INST].push_back({ AbEffect::Effect(Helper::Stats(), AbEffect::Damage(AbEffect::DamageType::PHYS, 1, 10), 1) ,0 });
		abAffixes[DOT].push_back({ AbEffect::Effect(Helper::Stats(), AbEffect::Damage(AbEffect::DamageType::POIS, 1, 10), 5), 0 });
	}

	Scroll * makeScroll(int aLvl, float mf);

	bool canRollAf(Item::SlotType type, AF af);

	Item::Rarity getRarity(float mf);

	Item * makeItem(int aLvl, float mf);

};

#endif