#include "Game.h"
#include "Ability.h"
#include "GenInfo.h"
#include "Helper.h"

#ifndef ITEM_H
#define ITEM_H

#define AF Helper::Affix
#define AFV Helper::AffVal

class Item
{
public:


#define numst 8
	enum SlotType {
		HED = 0,
		BDY = 1,
		MAH = 2,
		OFH = 3,
		RNG = 4,
		AMU = 5,
		CLK = 6,
		BLT = 7,
		SCR = 8
	};

	struct BaseItem {
		SlotType t;
		bool twoh;
		char* id;
		int init_af;
		int init_v1, init_v2;
		std::map<AF, AFV> aff;
		void addAff(AF a, AFV av) {
			if (aff.size() == 0)
			{
				init_v1 = av.v1;
				init_v2 = av.v2;
				init_af = (int)a;
			}
			aff[a] = av;
		}
		BaseItem(SlotType t, char* id, bool twoh) :t(t), id(id), twoh(twoh) {
			init_v1 = -1;
			init_v2 = -1;
			init_af = 0;
		}
		BaseItem() :
			t(SlotType::AMU),
			id("") {}
	};

	enum Rarity {
		NORM = 0,
		MAGIC = 1,
		RARE = 3,
		ULTRA = 5
	};
	Rarity rarity;

	SlotType slotType;
	Helper helper;
#define NUM_ITEM_BUFFS 4

	std::vector<std::string> buffStrings{ "Attack","Defense","Agility","Knowledge" };
	std::vector<std::string> typeStrings{ "Head","Body","Main-hand","Off-hand","Ring","Amulet","Cloak","Belt","Scroll" };
	std::map<AF, AFV> buffs;

	bool twoHanded = false;

	std::vector<AbEffect::Damage> damage;

	std::string getTypeString()
	{
		std::string str = typeStrings[slotType];
		if (twoHanded)
			str = "2H " + str;
		return str;
	}

	virtual std::string getName()
	{
		return this->name;
	}
	void addDamage(AbEffect::DamageType type, int min, int max)
	{
		damage.push_back(AbEffect::Damage{ type,min,max });
	}
	void setDamage(std::vector<AbEffect::Damage> damage)
	{
		this->damage.clear();
		this->damage = damage;
	}
	std::vector<AbEffect> getEffectFromMAH() {
		std::vector<AbEffect> effs;
		for (auto dmg : damage)
			effs.push_back(AbEffect(AbEffect::Effect({}, dmg, 1, AbEffect::EffType::INST)));
		return effs;
	}
	std::vector<std::string> getDamageString()
	{
		std::map<AbEffect::DamageType, std::string> damageStrings;
		std::vector < std::string> strings;
		damageStrings[AbEffect::DamageType::FIRE] = "Fire";
		damageStrings[AbEffect::DamageType::COLD] = "Ice";
		damageStrings[AbEffect::DamageType::LGHT] = "Lightning";
		damageStrings[AbEffect::DamageType::PHYS] = "Physical";
		damageStrings[AbEffect::DamageType::POIS] = "Poison";
		for (auto dmg : damage)
			strings.push_back(std::to_string(dmg.min) + "-" + std::to_string(dmg.max) + " " + damageStrings[dmg.type]);
		return strings;
	}

	virtual std::vector<std::pair<sf::Color, std::string>> getBaseString()
	{
		std::vector<std::pair<sf::Color, std::string>> baseStr;
		for (auto af : base.aff)
		{
			std::string afvstr = af.second.getStr();
			if (afvstr != "")
				baseStr.push_back({ af.second.color, afvstr + " " + helper.buffnames[(int)af.first] });
		}
		return baseStr;
	}
	virtual std::vector<std::pair<sf::Color, std::string>> getBuffString()
	{
		std::vector<std::pair<sf::Color, std::string>> buffStr;
		for (auto af : buffs)
		{
			std::string afvstr = af.second.getStr();
			if (afvstr != "")
				buffStr.push_back({ af.second.color, afvstr + " " + helper.buffnames[(int)af.first] });
		}
		return buffStr;
	}
	virtual std::string getItemTexName() { return ""; }

	void updateBaseVals();

	Item(std::string name, std::map<AF, AFV> buffs, BaseItem base, SlotType type, Rarity rarity)
	{
		this->name = name;
		this->slotType = type;
		this->buffs = buffs;
		this->base = base;
		this->rarity = rarity;
		this->twoHanded = base.twoh;
		updateBaseVals();
		std::string pre = "", suf = "";
		if (rarity == Item::Rarity::MAGIC || rarity == Item::Rarity::NORM)
		{
			for (auto buff : buffs)
			{
				if (buff.second.getStr() != "")
					if (buff.second.pre)
						pre = buff.second.id + " ";
					else
						suf = " of " + buff.second.id;
			}
			this->name = pre + name + suf;
		}
		else
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			int i = helper.numpreprefixes;
			std::uniform_int_distribution<> dist(0, i - 1);
			pre += helper.inamePrePrefixes[dist(gen)];
			dist.reset();
			dist = std::uniform_int_distribution<>(0, helper.inamePrefixes[slotType].size() - 1);
			pre += " " + helper.inamePrefixes[slotType][dist(gen)] + ", ";
			this->name = pre + suf + name;
		}
	}
	~Item();

	Helper::Stats getStatBuffs();

	std::string name;
	BaseItem base;

	std::vector<std::string> propertyText;

	virtual Ability* getAbility() {
		return nullptr;
	}

	Ability * ability;
};
class Scroll : public Item {
public:
	Scroll(std::string name, SlotType type, Ability* ability, Rarity rarity)
		: Item(name, {}, BaseItem(), type, rarity) {
		this->ability = ability;
		this->twoHanded = false;
	}
	std::string getDesc() {
		return ability->description;
	}

	std::vector<std::pair<sf::Color, std::string>> getBuffString()
	{
		std::map<AbEffect::DamageType, std::string> damageStrings;
		damageStrings[AbEffect::DamageType::FIRE] = "Fire";
		damageStrings[AbEffect::DamageType::COLD] = "Ice";
		damageStrings[AbEffect::DamageType::LGHT] = "Lightning";
		damageStrings[AbEffect::DamageType::PHYS] = "Physical";
		damageStrings[AbEffect::DamageType::POIS] = "Poison";
		std::map<AbEffect::DamageType, sf::Color> damageColors;
		damageColors[AbEffect::DamageType::FIRE] = sf::Color({ 255, 74, 61 });
		damageColors[AbEffect::DamageType::COLD] = sf::Color({ 109, 165, 255 });
		damageColors[AbEffect::DamageType::LGHT] = sf::Color({ 248, 255, 61 });
		damageColors[AbEffect::DamageType::PHYS] = sf::Color({ 226, 165, 86 });
		damageColors[AbEffect::DamageType::POIS] = sf::Color({ 131, 211, 69 });

		std::vector<std::pair<sf::Color, std::string>> buffStr;
		buffStr.push_back({ sf::Color::White, getDesc() });
		/*	if (ability->info.range == 0)
				buffStr.push_back({ sf::Color::White, "Buff" });
			else if (ability->info.range == 1)
				buffStr.push_back({ sf::Color::White, "Melee" });
			else
				buffStr.push_back({ sf::Color::White, "Ranged" });*/
		std::vector<AbEffect::Effect> effs = ability->getEffects();
		//float cd = 
		buffStr.push_back({ sf::Color::White, "Range: " + std::to_string(ability->info.range) });
		buffStr.push_back({ sf::Color::White, "Cooldown: " + std::to_string(ability->cooldown).substr(0,4) });
		buffStr.push_back({ sf::Color::White, "Cast Time: " + std::to_string((float)ability->tickCost).substr(0, 4) });
		for (auto eff : effs)
		{
			std::string min = helper.cutTrailing0s(std::to_string(eff.damage.min * eff.dur));
			std::string max = helper.cutTrailing0s(std::to_string(eff.damage.max * eff.dur));
			std::string type = damageStrings[eff.damage.type];
			if (eff.type != AbEffect::EffType::BUFF && eff.type != AbEffect::EffType::DEBUFF)
			{
				if (eff.dur == 1)
				{
					if (min != max)
						buffStr.push_back({ damageColors[eff.damage.type], min + "-" + max + " " + type });
					else
						buffStr.push_back({ damageColors[eff.damage.type], min + " " + type });
				}
				else
				{
					if (min != max)
						buffStr.push_back({ damageColors[eff.damage.type], min + "-" + max + " " + type + " over " + helper.cutTrailing0s(std::to_string(eff.dur)) + " ticks" });
					else
						buffStr.push_back({ damageColors[eff.damage.type], min + " " + type + " over " + helper.cutTrailing0s(std::to_string(eff.dur)) + " ticks" });
				}
			}
			Helper::Stats stats = eff.stats;
			for (int i = 0; i < Helper::numbuffs; i++)
			{
				int v1 = stats.buffs[(Helper::Affix)i].v1;
				if (v1 != -1)
				{
					if (v1 > 0)
						buffStr.push_back({ sf::Color::White,"Buffs " + helper.buffnames[i] + " by " + std::to_string(v1) + " for " + helper.cutTrailing0s(std::to_string(eff.dur)) + " ticks" });
					else
						buffStr.push_back({ sf::Color::White,"Debuffs " + helper.buffnames[i] + " by " + std::to_string(v1) + " for " + helper.cutTrailing0s(std::to_string(eff.dur)) + " ticks" });
				}
			}
		}


		return buffStr;
	}

	virtual Ability* getAbility() { return this->ability; }

	virtual std::string getItemTexName() {
		return ability->texName;
	}

	virtual std::string getName() {
		return this->ability->name;
	}
};
#endif /* ITEM_H */