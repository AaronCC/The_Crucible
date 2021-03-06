#ifndef ITEM_GENERATOR
#define ITEM_GENERATOR
#include "Item.h"
#include "Helper.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "Consumable.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <assert.h>
#include <Windows.h>
#include "dirent.h"

#define BaseItem Item::BaseItem
#define ST Item::SlotType
#define DMG AbEffect::DamageType
#define EffType AbEffect::EffType

class ItemGenerator
{
public:
	std::vector<std::string> filePaths;
	std::string dir_path = "media/data/";
	void getFilePaths() {
		filePaths.clear();
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir("media/data/")) != NULL) {
			/* print all the files and directories within directory */
			while ((ent = readdir(dir)) != NULL) {
				if (ent->d_name[0] != '.')
					filePaths.push_back(ent->d_name);
			}
			closedir(dir);
		}
		else {
			/* could not open directory */
			perror("");
		}
	}
	void writeFile(std::string file) {
		std::ofstream newFile(dir_path + file + ".txt");
		newFile.close();
	}

	Game * game;
	Helper helper;
	static const int numEfType = 4;
	static const int MAX_ILVL = 100;
	struct AbBase {
		std::string texName;
		std::string iconName;
		char description[255] = "NULL";
		char name[255] = "NULL";
		Animation anim;
		int cd, tc, area;
		float mult;
		AbBase(std::string tn, std::string in, char* desc, char* n, Animation anim, int cd, int tc, int area, float mult) :
			texName(tn), iconName(in), anim(anim), cd(cd), tc(tc), area(area), mult(mult)
		{
			memcpy(this->name, n, sizeof(char) * 255);
			memcpy(this->description, desc, sizeof(char) * 255);
		}
	};
	struct AfBase {
		//; type range offset aftype local dtype
		int range = 0;
		int offset = 0;
		int dtype;
		bool local;
		AfBase(int range, int offset, int dtype, bool local) :
			range(range), offset(offset), dtype(dtype), local(local) {
		}
		AfBase() {}
	};
	struct AbAfBase {
		//; type range offset aftype local dtype
		int range = 0;
		int offset = 0;
		int dtype;
		int dur;
		bool local;
		bool buff;
		Helper::Stats stats;
		AbAfBase(Helper::Stats stats,int range, int offset, int dtype, bool local, int dur, bool buff) :
			range(range), offset(offset), dtype(dtype), local(local), stats(stats), dur(dur), buff(buff) {
		}
		AbAfBase() {}
	};
	std::map<AF, AfBase> afBases;
	std::map<AF, AbAfBase> ab_afBases;
	int getRand_100() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 99);
		return dist(gen) + 1;
	}
	int getRand_0X(int x)
	{
		if (x < 0)
			return 0;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, x);
		return dist(gen);
	}

	std::vector<AbBase> abBases;
	std::vector<AbBase> e_abBases;

	std::map<ST, std::vector<std::pair<int, BaseItem>>> bases;
	std::map<EffType, std::vector<std::pair<AbEffect::Effect, int>>> abAffixes;

	std::map<AF, std::vector<std::pair<AFV, int>>> suffixes;
	std::map<AF, std::vector<std::pair<AFV, int>>> prefixes;

	std::map<AF, std::vector<std::string>> nameTable;

	void addSuffix(AF af, AFV afv, int ilvl) {
		suffixes[af].push_back({ afv,ilvl });
	}
	void addPrefix(AF af, AFV afv, int ilvl) {
		prefixes[af].push_back({ afv,ilvl });
	}
	void addAbAffix(EffType type, AbEffect::Effect ef, int ilvl) {
		abAffixes[type].push_back({ ef ,ilvl });
	}

	bool parseSuffix(std::vector<const char*> names, std::vector<std::string> data)
	{
		bool p = false;
		std::stringstream ss;
		std::vector<std::pair<AF, std::pair<AFV, int>>> afs;
		int range, off, af_I, local_I, dType_I, v1, v2, ilvl = 0;
		if (data.size() < 5)
			return false;
		ss = std::stringstream(data[0]);
		ss >> range;
		ss = std::stringstream(data[1]);
		ss >> off;
		ss = std::stringstream(data[2]);
		ss >> af_I;
		ss = std::stringstream(data[3]);
		ss >> local_I;
		ss = std::stringstream(data[4]);
		ss >> dType_I;
		AF af = (AF)af_I;
		afBases[af] = { range,off,dType_I,(bool)local_I };
		for (int i = 0; i < names.size(); i++)
		{
			nameTable[af].push_back(_strdup(names[i]));
		}
		bool l = (bool)local_I;
		DMG dtype = (DMG)dType_I;
		sf::Color c = helper.damageColors[dtype];
		while (ilvl < MAX_ILVL)
		{
			std::string name = names[getRand_0X(names.size() - 1)];
			v1 = off + (off*ilvl);
			v2 = v1 + range + (range*ilvl*0.5);
			if (v1 == v2)
				v2 = -1;
			AFV afv = AFV(v1, v2, name, p, c, l);
			afs.push_back({ af,{ afv,ilvl } });
			ilvl++;
		}
		for (auto af_afv : afs)
			addSuffix(af_afv.first, af_afv.second.first, af_afv.second.second);
		return true;
	}
	bool parsePrefix(std::vector<const char*> names, std::vector<std::string> data)
	{
		bool p = true;
		std::stringstream ss;
		std::vector<std::pair<AF, std::pair<AFV, int>>> afs;
		int range, off, af_I, local_I, dType_I, v1, v2, ilvl = 0;
		if (data.size() < 5)
			return false;
		ss = std::stringstream(data[0]);
		ss >> range;
		ss = std::stringstream(data[1]);
		ss >> off;
		ss = std::stringstream(data[2]);
		ss >> af_I;
		ss = std::stringstream(data[3]);
		ss >> local_I;
		ss = std::stringstream(data[4]);
		ss >> dType_I;
		AF af = (AF)af_I;
		afBases[af] = { range,off,dType_I,(bool)local_I };
		for (int i = 0; i < names.size(); i++)
		{
			nameTable[af].push_back(_strdup(names[i]));
		}
		bool l = (bool)local_I;
		DMG dtype = (DMG)dType_I;
		sf::Color c = helper.damageColors[dtype];
		while (ilvl < MAX_ILVL)
		{
			std::string name = names[getRand_0X(names.size() - 1)];
			v1 = off + (off*ilvl);
			v2 = v1 + range + (range*ilvl*0.5);
			if (v1 == v2)
				v2 = -1;
			AFV afv = AFV(v1, v2, name, p, c, l);
			afs.push_back({ af,{afv,ilvl} });
			ilvl++;
		}
		for (auto af_afv : afs)
			addPrefix(af_afv.first, af_afv.second.first, af_afv.second.second);
		return true;
	}
	bool parseBase(std::vector<std::string> data)
	{
		std::stringstream ss;
		int slot = 0;
		ss = std::stringstream(data[0]);
		ss >> slot;
		Item::SlotType type = (Item::SlotType)slot;
		int i = 2, a1 = 0, a2 = 0, dtypeInt = 0, afInt = 0, twohInt = 0, lInt, ilvl;
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
		name += data[i].substr(0, data[i].size() - 1);
		char* c_name = _strdup(name.c_str());
		ss = std::stringstream(data[i + 1]);
		ss >> a1;
		ss = std::stringstream(data[i + 2]);
		ss >> a2;
		ss = std::stringstream(data[i + 3]);
		ss >> dtypeInt;
		ss = std::stringstream(data[i + 4]);
		ss >> afInt;
		ss = std::stringstream(data[i + 5]);
		ss >> lInt;
		ss = std::stringstream(data[i + 6]);
		ss >> ilvl;
		bool local = (bool)lInt;
		DMG dtype = (DMG)dtypeInt;
		sf::Color c = helper.damageColors[dtype];
		AFV afv = { a1, a2, "Base", true, c, local };
		AF af = (AF)afInt;
		BaseItem* bi = new BaseItem(type, c_name, twoh);
		bi->addAff(af, afv);
		bases[type].push_back({ ilvl,*bi });
		return true;
	}
	bool parseEAbBase(std::vector<std::string> data)
	{
		int i = 2, cd, ct, area;
		float mult;
		unsigned int aStart, aEnd;
		std::string texname = data[0];
		std::string iconname = data[1];
		std::string desc = "";
		while (data[i][data[i].size() - 1] != ';')
		{
			desc += data[i] + " ";
			i++;
		}
		desc += data[i].substr(0, data[i].size() - 1);
		i++;
		std::string name;
		while (data[i][data[i].size() - 1] != ';')
		{
			name += data[i] + " ";
			i++;
		}
		name += data[i].substr(0, data[i].size() - 1);
		std::stringstream ss(data[i + 1]);
		ss >> aStart;
		ss = std::stringstream(data[i + 2]);
		ss >> aEnd;
		ss = std::stringstream(data[i + 3]);
		ss >> cd;
		ss = std::stringstream(data[i + 4]);
		ss >> ct;
		ss = std::stringstream(data[i + 5]);
		ss >> area;
		ss = std::stringstream(data[i + 6]);
		ss >> mult;
		char* c_desc = _strdup(desc.c_str());
		char* c_name = _strdup(name.c_str());
		e_abBases.push_back(AbBase(texname, iconname, c_desc, c_name,
			{ aStart,aEnd,0.1f }, cd, ct, area, mult));
		return false;
	}
	bool parseAbBase(std::vector<std::string> data)
	{
		int i = 2, cd, ct, area;
		float mult;
		unsigned int aStart, aEnd;
		std::string texname = data[0];
		std::string iconname = data[1];
		std::string desc = "";
		while (data[i][data[i].size() - 1] != ';')
		{
			desc += data[i] + " ";
			i++;
		}
		desc += data[i].substr(0, data[i].size() - 1);
		i++;
		std::string name;
		while (data[i][data[i].size() - 1] != ';')
		{
			name += data[i] + " ";
			i++;
		}
		name += data[i].substr(0, data[i].size() - 1);
		std::stringstream ss(data[i + 1]);
		ss >> aStart;
		ss = std::stringstream(data[i + 2]);
		ss >> aEnd;
		ss = std::stringstream(data[i + 3]);
		ss >> cd;
		ss = std::stringstream(data[i + 4]);
		ss >> ct;
		ss = std::stringstream(data[i + 5]);
		ss >> area;
		ss = std::stringstream(data[i + 6]);
		ss >> mult;
		char* c_desc = _strdup(desc.c_str());
		char* c_name = _strdup(name.c_str());
		abBases.push_back(AbBase(texname, iconname, c_desc, c_name,
			{ aStart,aEnd,0.1f }, cd, ct, area, mult));
		return false;
	}

	bool parseAbAff(std::vector<std::string> data)
	{
		bool hasStats;
		bool buff;
		int dmgtype, v1, v2, dur, ilvl = 0, b, i = 2, range, off, af;
		std::stringstream ss(data[0]);
		ss >> b;
		hasStats = (bool)b;
		Helper::Stats stats{};
		if (hasStats)
		{
			int afv;
			ss = std::stringstream(data[1]);
			ss >> b;
			buff = (bool)b;
			while (data[i + 1][data[i + 1].size() - 1] != ';')
			{
				ss = std::stringstream(data[i]);
				ss >> af;
				i++;
				ss = std::stringstream(data[i]);
				ss >> afv;
				stats.buffs[(AF)af] = AFV{ afv,-1,"",true,sf::Color::White,false };
			}
			ss = std::stringstream(data[i]);
			ss >> af;
			i++;
			ss = std::stringstream(data[i]);
			ss >> afv;
			stats.buffs[(AF)af] = AFV{ afv,-1,"",true,sf::Color::White,false };
			ss = std::stringstream(data[i + 1]);
			ss >> dmgtype;
		}
		else
		{
			i = 0;
			ss = std::stringstream(data[i + 1]);
			ss >> dmgtype;
			af = (int)helper.dTypeAffixTable[dmgtype];
		}
   		ss = std::stringstream(data[i + 2]);
		ss >> range;
		ss = std::stringstream(data[i + 3]);
		ss >> off;
		ss = std::stringstream(data[i + 4]);
		ss >> dur;
		ab_afBases[(AF)af] = { stats, range,off,dmgtype,false,dur,hasStats };
		while (ilvl <= MAX_ILVL)
		{
			v1 = off + (off*ilvl);
			v2 = v1 + range + (range*ilvl*0.5);
			if (v1 == v2)
				v2 = -1;
			if (!hasStats && dur == 1)
				abAffixes[EffType::INST].push_back({ AbEffect::Effect(Helper::Stats(), AbEffect::Damage((DMG)dmgtype, v1, v2), dur, EffType::INST) ,ilvl });
			else if (!hasStats)
				abAffixes[EffType::DOT].push_back({ AbEffect::Effect(Helper::Stats(), AbEffect::Damage((DMG)dmgtype, v1, v2), dur, EffType::DOT) ,ilvl });
			else if (hasStats && buff)
				abAffixes[EffType::BUFF].push_back({ AbEffect::Effect(stats, AbEffect::Damage((DMG)dmgtype, v1, v2), dur,EffType::BUFF) ,ilvl });
			else if (hasStats && !buff)
				abAffixes[EffType::DEBUFF].push_back({ AbEffect::Effect(stats, AbEffect::Damage((DMG)dmgtype, v1, v2), dur, EffType::DEBUFF) ,ilvl });
			ilvl += 2;
		}
		return false;
	}

	void exportAffixes(std::string fileName);

	void loadAffixes(std::string fileName) {

		nameTable.clear();
		afBases.clear();
		abBases.clear();
		e_abBases.clear();
		bases.clear();
		abAffixes.clear();
		suffixes.clear();
		prefixes.clear();

		std::string line;
		std::ifstream texFile(dir_path + fileName);
		enum Parsing {
			SUF,
			PRE,
			BAS,
			AB_AF,
			AB_BAS,
			E_AB_BAS,
			NAMES,
			NA
		};
		Parsing parsing = Parsing::NA;
		if (texFile.is_open())
		{
			std::vector<const char*> names{};
			while (std::getline(texFile, line))
			{
				std::istringstream iss(line);
				std::vector<std::string> results(std::istream_iterator<std::string>{iss},
					std::istream_iterator<std::string>());
				int type = 0;
				if (line[0] == ';')
					continue;
				if (results.size() == 0)
					break;
				std::stringstream ss(results[0]);
				ss >> type;
				if (type == (int)NAMES)
				{
					results.erase(results.begin());
					for (int i = 0; i < results.size(); i++)
					{
						names.push_back(_strdup(results[i].c_str()));
					}
					continue;
				}
				if (type == 0)
				{
					std::stringstream ss(results[3]);
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
					parseSuffix(names, results);
					names.clear();
					std::cout << "\nadded suffix";
					break;
				case Parsing::PRE:
					parsePrefix(names, results);
					names.clear();
					std::cout << "\nadded prefix";
					break;
				case Parsing::BAS:
					parseBase(results);
					std::cout << "\nadded base";
					break;
				case Parsing::AB_AF:
					parseAbAff(results);
					std::cout << "\nadded ab affix";
					break;
				case Parsing::AB_BAS:
					parseAbBase(results);
					std::cout << "\nadded ab base";
					break;
				case Parsing::E_AB_BAS:
					parseEAbBase(results);
					std::cout << "\nadded enemy ab base";
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
		this->game = game;

		afTypeExMap[AF::HEALTH] = { 2 };
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

		getFilePaths();
		loadAffixes(filePaths[0]);
		//BaseItem* itm = new BaseItem(ST::MAH, "Elegant Sword");
		/*itm->addAff(AF::PHYS_FLT_DMG, { 1, 5, "Base", true, helper.damageColors[DMG::PHYS],true });
		bases[ST::MAH].push_back({ 0,*itm });*/

		//abBases.push_back(AbBase("slash", "slash_icon", "Melee slash in an arc", "Slash",
		//	{ 0,3,0.1f }, 10, 1));

		//abAffixes[INST].push_back({ AbEffect::Effect(Helper::Stats(), AbEffect::Damage(AbEffect::DamageType::PHYS, 1, 10), 1) ,0 });
		//abAffixes[DOT].push_back({ AbEffect::Effect(Helper::Stats(), AbEffect::Damage(AbEffect::DamageType::POIS, 1, 10), 5), 0 });
	}

	Scroll * makeScroll(int aLvl, float mf);

	Ability * makeEnemyAbility(int aLvl, Item::Rarity rarity, bool melee, AbEffect::DamageType dtype, float eMult);

	bool canRollAf(Item::SlotType type, AF af);

	Consumable * makeConsumable(int aLvl);

	Item::Rarity getRarity(float mf);

	Item * makeItem(int aLvl, float mf);

	Item * makeItem(int aLvl, float mf, Item::SlotType type);

	Item * makeItem(int aLvl, Item::Rarity rarity, Item::SlotType sType);

};

#endif