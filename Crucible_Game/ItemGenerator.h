#ifndef ITEM_GENERATOR
#define ITEM_GENERATOR
#include "Item.h"
#include "Helper.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Game.h"

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
		texName(tn),iconName(in),description(desc),name(n),anim(anim),cd(cd),tc(tc){}
	};

	std::vector<AbBase> abBases;

	std::map<ST,std::vector<std::pair<int,BaseItem>>> bases;
	std::map<EfType, std::vector<std::pair<AbEffect::Effect,int>>> abAffixes;

	std::map<AF,std::vector<std::pair<AFV, int>>> suffixes;
	std::map<AF, std::vector<std::pair<AFV, int>>> prefixes;
	ItemGenerator() {}
	ItemGenerator(Game* game)
	{
		Helper helper;
		this->game = game;
		suffixes[AF::FIRE_FLT_DMG].push_back({ AFV(1, 2, "of ash",false,helper.damageColors[DMG::FIRE],true),0 });
		suffixes[AF::COLD_FLT_DMG].push_back({ AFV(1, 2, "of frost",false,helper.damageColors[DMG::COLD],true),0 });
		suffixes[AF::LGHT_FLT_DMG].push_back({ AFV(1, 2, "of sparking",false,helper.damageColors[DMG::LGHT],true),0 });
		suffixes[AF::POIS_FLT_DMG].push_back({ AFV(1, 2, "of sickness",false,helper.damageColors[DMG::POIS],true),0 });
		suffixes[AF::PHYS_FLT_DMG].push_back({ AFV(1, 2, "of striking",false,helper.damageColors[DMG::PHYS],true),0 });
		prefixes[AF::FIRE_PRC_DMG].push_back({ AFV(5, -1, "Hot",true,sf::Color::White,false),0 });
		prefixes[AF::COLD_PRC_DMG].push_back({ AFV(5, -1, "Cool",true,sf::Color::White,false),0 });
		prefixes[AF::LGHT_PRC_DMG].push_back({ AFV(5, -1, "Quick",true,sf::Color::White,false),0 });
		prefixes[AF::POIS_PRC_DMG].push_back({ AFV(5, -1, "Slick",true,sf::Color::White,false),0 });
		prefixes[AF::PHYS_PRC_L_DMG].push_back({ AFV(100, -1, "Heavy",true,sf::Color::White,false),0 });
		prefixes[AF::PHYS_PRC_G_DMG].push_back({ AFV(5, -1, "Heavy",true,sf::Color::White,false),0 });
		prefixes[AF::AGI].push_back({ AFV(1, -1, "Quick",true,sf::Color::White,false),0 });
		prefixes[AF::DEF].push_back({ AFV(1, -1, "Hard",true,sf::Color::White,false),0 });
		prefixes[AF::KNO].push_back({ AFV(1, -1, "Smart",true,sf::Color::White,false),0 });
		prefixes[AF::ATK].push_back({ AFV(1, -1, "Strong",true,sf::Color::White,false),0 });

		BaseItem* itm = new BaseItem(ST::MAH, "Elegant Sword");
		itm->addAff(AF::PHYS_FLT_DMG, { 1, 5, "Base", true, helper.damageColors[DMG::PHYS],true });
		bases[ST::MAH].push_back({ 0,*itm });

		abBases.push_back(AbBase("slash", "slash_icon", "Melee slash in an arc", "Slash",
			{ 0,3,0.1f }, 10, 1));

		abAffixes[INST].push_back({ AbEffect::Effect(Helper::Stats(), AbEffect::Damage(AbEffect::DamageType::PHYS, 1, 10), 1) ,0 });
		abAffixes[DOT].push_back({AbEffect::Effect(Helper::Stats(), AbEffect::Damage(AbEffect::DamageType::POIS, 1, 10), 5), 0});
	}

	Scroll * makeScroll(int aLvl, float mf);

	Item::Rarity getRarity(float mf);

	Item * makeItem(int aLvl, float mf);

};

#endif