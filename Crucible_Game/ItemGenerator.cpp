#include "ItemGenerator.h"

Item::Rarity ItemGenerator::getRarity(float mf)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	const float MAX_MF = 1000.f;
	int dropRate[4] = { 50, 35, 10, 5 };
	int rarityWeights[4]{ -50, 25, 15, 10 };

	for (int i = 0; i < 4; i++)
	{
		float prc = mf / MAX_MF;
		int diff = rarityWeights[i] * prc;
		dropRate[i] += diff;
	}

	std::uniform_int_distribution<> dist(0, 99);
	Item::Rarity rarity = Item::Rarity::NORM;
	int roll = dist(gen) + 1;
	Item::Rarity rarities[4] = { Item::Rarity::NORM, Item::Rarity::MAGIC, Item::Rarity::RARE, Item::Rarity::ULTRA };
	int dropchance = 0;
	for (int i = 0; i < 4; i++)
	{
		dropchance += dropRate[i];
		if (roll <= dropchance)
		{
			rarity = rarities[i];
			break;
		}
	}
	return rarity;
}
void ItemGenerator::exportAffixes(std::string fileName)
{
	std::string out;
	std::ofstream newFile(dir_path + fileName, std::ofstream::trunc);

	for (auto base : afBases)
	{
		std::string names = "6 ";
		for (auto name : nameTable[base.first])
		{
			names += name + " ";
		}
		names += "\n";
		AfBase afb = base.second;

		int local = afb.local ? 1 : 0;
		out = "0 " +
			std::to_string(afb.range) + " " +
			std::to_string(afb.offset) + " " +
			std::to_string((int)base.first) + " " +
			std::to_string((int)afb.local) + " " +
			std::to_string(local) + "\n";
		newFile << names;
		newFile << out;
	}
	out = "";
	for (int slot_type = 0; slot_type < Item::SlotType::SCR; slot_type++)
	{
		for (auto base : bases[(Item::SlotType)slot_type])
		{
			int ilvl = base.first;
			BaseItem bi = base.second;
			int dType = helper.isDamage[(AF)bi.init_af].second;
			int isDamage = (int)helper.isDamage[(AF)bi.init_af].first;
			out = "2 " +
				std::to_string(slot_type) + " " +
				std::to_string(bi.twoh) + " " +
				bi.id + "; " +
				std::to_string(bi.init_v1) + " " +
				std::to_string(bi.init_v2) + " " +
				std::to_string(dType) + " " +
				std::to_string(bi.init_af) + " " +
				std::to_string(isDamage) + " " +
				std::to_string(ilvl) + "\n";
			newFile << out;
		}
	}
	out = "";
	for (auto base : ab_afBases)
	{
		int hasStats = helper.isDamage[base.first].first ? 0 : 1;
		std::map<AF,AFV> afBuffs = base.second.stats.getActiveBuffs();
		out = "3 " + std::to_string(hasStats);
		if (hasStats == 1)
		{
			out += " 1 ";
			for (auto afBuff : afBuffs)
			{
				out += std::to_string((int)afBuff.first) + " " +
					std::to_string(afBuff.second.v1) + "; ";
			}
			out += "5 0 0 10\n";
		}
		else
		{
			out += " " + 
				std::to_string(base.second.dtype) + " " +
				std::to_string(base.second.range) + " " +
				std::to_string(base.second.offset) + " " +
				std::to_string(base.second.dur) + "\n";
		}
		newFile << out;
	}
	out = "";
	for (auto ab : abBases)
	{
		out = "4 " +
			ab.texName + " " +
			ab.iconName + " " +
			ab.description + "; " +
			ab.name + "; " +
			std::to_string(ab.anim.startFrame) + " " +
			std::to_string(ab.anim.endFrame) + " " +
			std::to_string(ab.cd) + " " +
			std::to_string(ab.tc) + " " +
			std::to_string(ab.area) + " " +
			std::to_string(ab.mult) + "\n";
		newFile << out;
	}
	out = "";
	for (auto ab : e_abBases)
	{
		out = "5 " +
			ab.texName + " " +
			ab.iconName + " " +
			ab.description + "; " +
			ab.name + "; " +
			std::to_string(ab.anim.startFrame) + " " +
			std::to_string(ab.anim.endFrame) + " " +
			std::to_string(ab.cd) + " " +
			std::to_string(ab.tc) + " " +
			std::to_string(ab.area) + " " +
			std::to_string(ab.mult) + "\n";
		newFile << out;
	}
	/*nameTable.clear();
	afBases.clear();
	abBases.clear();
	e_abBases.clear();
	bases.clear();
	abAffixes.clear();*/
	newFile.close();
}
Scroll* ItemGenerator::makeScroll(int aLvl, float mf)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	Item::Rarity rarity = getRarity(mf);
	bool justbuff = true;
	if (rarity == Item::Rarity::NORM)
		rarity = Item::Rarity::MAGIC;
	std::uniform_int_distribution<> dist(0, (int)rarity - 1);
	Scroll* scroll;
	int numaff = dist(gen) + 1;
	dist.reset();
	dist = std::uniform_int_distribution<>(0, abBases.size() - 1);
	AbBase abBase = abBases[dist(gen)];
	Ability* ab = new Ability(game, game->texmgr.getRef(abBase.texName), abBase.iconName,
		abBase.anim, { TILE_SIZE,TILE_SIZE }, abBase.cd, abBase.tc,
		abBase.name, abBase.description);
	std::vector<AbEffect> effs;
	std::map<EffType, std::vector<AbEffect>> pools;
	std::vector<EffType> choices;
	for (int i = 0; i < numEfType; i++)
	{
		for (auto eff : abAffixes[(EffType)i])
		{
			if (eff.second <= aLvl)
				pools[(EffType)i].push_back(eff.first);
		}
		if (pools[(EffType)i].size() > 0)
			choices.push_back((EffType)i);
	}
	if (choices.size() > 0)
		for (int i = 0; i < numaff; i++)
		{
			dist.reset();
			dist = std::uniform_int_distribution<>(0, choices.size() - 1);
			int choice = dist(gen);
			EffType efType = choices[choice];
			int poolsize = pools[efType].size();
			dist.reset();
			dist = std::uniform_int_distribution<>(0, poolsize - 1);
			int poolat = dist(gen);
			AbEffect eff = pools[efType][poolat];
			if (efType != EffType::BUFF)
			{
				eff.eff.damage.min *= abBase.mult;
				eff.eff.damage.max *= abBase.mult;
				justbuff = false;
			}
			effs.push_back(eff);
			pools[efType].erase(pools[efType].begin() + poolat);
			if (pools[efType].size() == 0)
				choices.erase(choices.begin() + choice);
			if (choices.size() <= 0)
				break;
		}
	Ability::Area area;
	int range;
	if (justbuff)
	{
		ab->description = "Buff";
		area = Ability::Area::TARGET;
		range = 0;
	}
	else if (abBase.area != -1 && abBase.area < Ability::numarea)
	{
		area = (Ability::Area)abBase.area;
		dist.reset();
		dist = std::uniform_int_distribution<>(0, 3);
		range = dist(gen) + 1;
	}
	else
	{
		dist.reset();
		dist = std::uniform_int_distribution<>(0, Ability::numarea - 1);
		area = (Ability::Area)dist(gen);
		dist.reset();
		dist = std::uniform_int_distribution<>(0, 3);
		range = dist(gen) + 1;
	}
	Ability::AbInfo info{ area,range };
	ab->setInfo(info);
	for (auto eff : effs)
		ab->addEffect(eff);
	scroll = new Scroll(ab->name, Item::SlotType::SCR, ab, rarity);
	return scroll;
}/*
Ability * ItemGenerator::makeEnemyAbility(int aLvl, Item::Rarity rarity, bool melee, float eMult)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	if (rarity == Item::Rarity::NORM)
		rarity = Item::Rarity::MAGIC;
	std::uniform_int_distribution<> dist(0, (int)rarity - 1);
	Scroll* scroll;
	int numaff = dist(gen) + 1;
	dist.reset();
	dist = std::uniform_int_distribution<>(0, abBases.size() - 1);
	AbBase abBase = abBases[dist(gen)];
	dist.reset();
	dist = std::uniform_int_distribution<>(0, 2);
	int tcPenalty = dist(gen) + 1;
	Ability* ab = new Ability(game, game->texmgr.getRef(abBase.texName), abBase.iconName,
		abBase.anim, { TILE_SIZE,TILE_SIZE }, abBase.cd, abBase.tc,
		abBase.name, abBase.description);
	std::vector<AbEffect> effs;
	std::map<EffType, std::vector<AbEffect>> pools;
	std::vector<EffType> choices;
	for (int i = 0; i < numEfType; i++)
	{
		if ((EffType)i == EffType::BUFF)
			continue;
		for (auto eff : abAffixes[(EffType)i])
		{
			if (eff.second <= aLvl)
				pools[(EffType)i].push_back(eff.first);
		}
		if (pools[(EffType)i].size() > 0)
			choices.push_back((EffType)i);
	}
	if (choices.size() > 0)
		for (int i = 0; i < numaff; i++)
		{
			dist.reset();
			dist = std::uniform_int_distribution<>(0, choices.size() - 1);
			int choice = dist(gen);
			EffType efType = choices[choice];
			int poolsize = pools[efType].size();
			dist.reset();
			dist = std::uniform_int_distribution<>(0, poolsize - 1);
			int poolat = dist(gen);
			AbEffect eff = pools[efType][poolat];
			effs.push_back(eff);
			pools[efType].erase(pools[efType].begin() + poolat);
			if (pools[efType].size() == 0)
				choices.erase(choices.begin() + choice);
			if (choices.size() <= 0)
				break;
		}
	Ability::Area area;
	int range;
	dist.reset();
	dist = std::uniform_int_distribution<>(0, Ability::numarea - 1);
	area = Ability::Area::TARGET;
	if (melee)
		range = 1;
	else {
		dist.reset();
		dist = std::uniform_int_distribution<>(0, 2);
		range = dist(gen) + 2;
	}
	Ability::AbInfo info{ area,range };
	ab->setInfo(info);
	for (auto eff : effs)
		ab->addEffect(eff);
	return ab;
}*/
Ability * ItemGenerator::makeEnemyAbility(int aLvl, Item::Rarity rarity, bool melee, AbEffect::DamageType dtype, float eMult)
{
	bool has_dType = false;
	std::random_device rd;
	std::mt19937 gen(rd());
	if (rarity == Item::Rarity::NORM)
		rarity = Item::Rarity::MAGIC;
	std::uniform_int_distribution<> dist(0, (int)rarity - 1);
	Scroll* scroll;
	int numaff = dist(gen) + 1;
	dist.reset();
	dist = std::uniform_int_distribution<>(0, e_abBases.size() - 1);
	AbBase abBase = e_abBases[dist(gen)];
	Ability* ab = new Ability(game, game->texmgr.getRef(abBase.texName), abBase.iconName,
		abBase.anim, { TILE_SIZE,TILE_SIZE }, abBase.cd, abBase.tc,
		abBase.name, abBase.description);
	std::vector<AbEffect> effs;
	std::map<EffType, std::vector<AbEffect>> pools;
	std::vector<EffType> choices;
	float multiplier = ((abBase.mult + eMult) / 2.f) + (aLvl * 0.2f);
	for (int i = 0; i < numEfType; i++)
	{
		if ((EffType)i == EffType::BUFF)
			continue;
		for (auto eff : abAffixes[(EffType)i])
		{
			if (eff.second <= aLvl)
			{
				if (eff.first.damage.type == dtype && !has_dType)
				{
					numaff--;
					has_dType = true;
					eff.first.damage.min *= multiplier;
					if (eff.first.damage.min < 1) eff.first.damage.min = 1;
					eff.first.damage.max *= multiplier;
					if (eff.first.damage.max < 1) eff.first.damage.max = 1;
					effs.push_back(eff.first);
				}
				else
					pools[(EffType)i].push_back(eff.first);
			}
		}
		if (pools[(EffType)i].size() > 0)
			choices.push_back((EffType)i);
	}
	if (choices.size() > 0)
		for (int i = 0; i < numaff; i++)
		{
			dist.reset();
			dist = std::uniform_int_distribution<>(0, choices.size() - 1);
			int choice = dist(gen);
			EffType efType = choices[choice];
			int poolsize = pools[efType].size();
			dist.reset();
			dist = std::uniform_int_distribution<>(0, poolsize - 1);
			int poolat = dist(gen);
			AbEffect eff = pools[efType][poolat];
			eff.eff.damage.min *= multiplier;
			if (eff.eff.damage.min < 1) eff.eff.damage.min = 1;
			eff.eff.damage.max *= multiplier;
			if (eff.eff.damage.max < 1) eff.eff.damage.max = 1;
			effs.push_back(eff);
			pools[efType].erase(pools[efType].begin() + poolat);
			if (pools[efType].size() == 0)
				choices.erase(choices.begin() + choice);
			if (choices.size() <= 0)
				break;
		}
	Ability::Area area;
	int range;
	dist.reset();
	dist = std::uniform_int_distribution<>(0, Ability::numarea - 1);
	area = Ability::Area::TARGET;
	if (melee)
		range = 1;
	else {
		dist.reset();
		dist = std::uniform_int_distribution<>(0, 2);
		range = dist(gen) + 2;
	}
	Ability::AbInfo info{ area,range };
	ab->setInfo(info);
	for (auto eff : effs)
		ab->addEffect(eff);
	return ab;
}
bool ItemGenerator::canRollAf(Item::SlotType type, AF af)
{
	for (auto st : afTypeExMap[af])
	{
		if (type == st)
			return false;
	}
	return true;
}

Consumable * ItemGenerator::makeConsumable(int aLvl)
{
	int roll = getRand_100();
	std::string tiers[5] = { "Meager", "Lesser", "Medium", "Greater", "Mega" };
	if (aLvl >= 5)
		aLvl = 4;
	return new Consumable{ game,"hpot",{ 10+(10 * aLvl),Consumable::ConType::H_POT },tiers[aLvl] + " Health Potion",aLvl };
}

Item * ItemGenerator::makeItem(int aLvl, float mf)
{
	Helper helper;
	int offset = 0;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, numst - 1);

	int st = dist(gen);
	ST type = (ST)st;
	std::vector<BaseItem> basePool;

	for (auto base : bases[type])
	{
		if (base.first <= aLvl)
			basePool.push_back(base.second);
	}
	if (basePool.size() < 1)
		return nullptr;

	dist.reset();
	dist = std::uniform_int_distribution<>(0, basePool.size() - 1);
	int rb = dist(gen);
	BaseItem bi = basePool[rb];

	Item::Rarity rarity = getRarity(mf);

	int numAffixes = (int)rarity;
	std::pair<int, int> afRange = helper.pRange;
	int numP = 0, numS = 0, rndNumAffixes = 0;
	do
	{
		dist.reset();
		dist = std::uniform_int_distribution<>(0, numAffixes);
		numP = dist(gen);
		numS = dist(gen);
	} while (numP < (1 + numAffixes - 1) && numS < (1 + numAffixes - 1));
	std::map<AF, AFV> itmAfvs;
	rndNumAffixes = numP;

	std::map<AF, std::vector<std::pair<AFV, int>>> sufpools;
	std::map<AF, std::vector<std::pair<AFV, int>>> prepools;
	std::vector<AF> sufchoices;
	std::vector<AF> prechoices;

	for (int i = 0; i <= helper.sRange.second; i++)
	{
		for (auto eff : suffixes[(AF)i])
		{
			if (eff.second <= aLvl && canRollAf(bi.t, (AF)i))
				sufpools[(AF)i].push_back({ eff.first,eff.second });
		}
		if (sufpools[(AF)i].size() > 0)
			sufchoices.push_back((AF)i);
	}
	for (int i = helper.pRange.first; i <= helper.pRange.second; i++)
	{
		for (auto eff : prefixes[(AF)i])
		{
			if (eff.second <= aLvl && canRollAf(bi.t, (AF)i))
				prepools[(AF)i].push_back({ eff.first,eff.second });
		}
		if (prepools[(AF)i].size() > 0)
			prechoices.push_back((AF)i);
	}

	std::vector<AF> choices = prechoices;
	std::map<AF, std::vector<std::pair<AFV, int>>> affixes = prepools;
	for (int c = 0; c < 2; c++)
	{
		for (int i = 0; i < rndNumAffixes; i++)
		{
			dist.reset();
			dist = std::uniform_int_distribution<>(0, choices.size() - 1);
			int choice = dist(gen);
			AF af = (AF)choices[choice];
			dist.reset();
			dist = std::uniform_int_distribution<>(0, affixes[af].size() - 1);
			int rndAfv = dist(gen);
			itmAfvs[af] = affixes[af][rndAfv].first;
			affixes[af].erase(affixes[af].begin() + rndAfv);
			if (affixes[af].size() == 0)
				choices.erase(choices.begin() + choice);
			if (choices.size() <= 0)
				break;
		}
		affixes = sufpools;
		choices = sufchoices;
		afRange = helper.sRange;
		rndNumAffixes = numS;
	}
	Item* itm = new Item{ bi.id,itmAfvs,bi,bi.t,rarity };
	return itm;
}

Item * ItemGenerator::makeItem(int aLvl, float mf, Item::SlotType sType)
{
	Helper helper;
	int offset = 0;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist;
	std::vector<BaseItem> basePool;
	for (auto base : bases[sType])
	{
		if (base.first <= aLvl)
			basePool.push_back(base.second);
	}
	if (basePool.size() < 1)
		return nullptr;

	dist.reset();
	dist = std::uniform_int_distribution<>(0, basePool.size() - 1);
	int rb = dist(gen);
	BaseItem bi = basePool[rb];

	Item::Rarity rarity = getRarity(mf);

	int numAffixes = (int)rarity;
	std::pair<int, int> afRange = helper.pRange;
	int numP = 0, numS = 0, rndNumAffixes = 0;
	do
	{
		dist.reset();
		dist = std::uniform_int_distribution<>(0, numAffixes);
		numP = dist(gen);
		numS = dist(gen);
	} while (numP < (1 + numAffixes - 1) && numS < (1 + numAffixes - 1));
	std::map<AF, AFV> itmAfvs;
	rndNumAffixes = numP;

	std::map<AF, std::vector<std::pair<AFV, int>>> sufpools;
	std::map<AF, std::vector<std::pair<AFV, int>>> prepools;
	std::vector<AF> sufchoices;
	std::vector<AF> prechoices;

	for (int i = 0; i <= helper.sRange.second; i++)
	{
		for (auto eff : suffixes[(AF)i])
		{
			if (eff.second <= aLvl && canRollAf(bi.t, (AF)i))
				sufpools[(AF)i].push_back({ eff.first,eff.second });
		}
		if (sufpools[(AF)i].size() > 0)
			sufchoices.push_back((AF)i);
	}
	for (int i = helper.pRange.first; i <= helper.pRange.second; i++)
	{
		for (auto eff : prefixes[(AF)i])
		{
			if (eff.second <= aLvl && canRollAf(bi.t, (AF)i))
				prepools[(AF)i].push_back({ eff.first,eff.second });
		}
		if (prepools[(AF)i].size() > 0)
			prechoices.push_back((AF)i);
	}

	std::vector<AF> choices = prechoices;
	std::map<AF, std::vector<std::pair<AFV, int>>> affixes = prepools;
	for (int c = 0; c < 2; c++)
	{
		for (int i = 0; i < rndNumAffixes; i++)
		{
			dist.reset();
			dist = std::uniform_int_distribution<>(0, choices.size() - 1);
			int choice = dist(gen);
			AF af = (AF)choices[choice];
			dist.reset();
			dist = std::uniform_int_distribution<>(0, affixes[af].size() - 1);
			int rndAfv = dist(gen);
			itmAfvs[af] = affixes[af][rndAfv].first;
			affixes[af].erase(affixes[af].begin() + rndAfv);
			if (affixes[af].size() == 0)
				choices.erase(choices.begin() + choice);
			if (choices.size() <= 0)
				break;
		}
		affixes = sufpools;
		choices = sufchoices;
		afRange = helper.sRange;
		rndNumAffixes = numS;
	}
	Item* itm = new Item{ bi.id,itmAfvs,bi,bi.t,rarity };
	return itm;
}

Item * ItemGenerator::makeItem(int aLvl, Item::Rarity rarity, Item::SlotType sType)
{
	Helper helper;
	int offset = 0;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist;
	std::vector<BaseItem> basePool;
	for (auto base : bases[sType])
	{
		if (base.first <= aLvl)
			basePool.push_back(base.second);
	}
	if (basePool.size() < 1)
		return nullptr;

	dist.reset();
	dist = std::uniform_int_distribution<>(0, basePool.size() - 1);
	int rb = dist(gen);
	BaseItem bi = basePool[rb];

	int numAffixes = (int)rarity;
	std::pair<int, int> afRange = helper.pRange;
	int numP = 0, numS = 0, rndNumAffixes = 0;
	do
	{
		dist.reset();
		dist = std::uniform_int_distribution<>(0, numAffixes);
		numP = dist(gen);
		numS = dist(gen);
	} while (numP < (1 + numAffixes - 1) && numS < (1 + numAffixes - 1));
	std::map<AF, AFV> itmAfvs;
	rndNumAffixes = numP;

	std::map<AF, std::vector<std::pair<AFV, int>>> sufpools;
	std::map<AF, std::vector<std::pair<AFV, int>>> prepools;
	std::vector<AF> sufchoices;
	std::vector<AF> prechoices;

	for (int i = 0; i <= helper.sRange.second; i++)
	{
		for (auto eff : suffixes[(AF)i])
		{
			if (eff.second <= aLvl && canRollAf(bi.t, (AF)i))
				sufpools[(AF)i].push_back({ eff.first,eff.second });
		}
		if (sufpools[(AF)i].size() > 0)
			sufchoices.push_back((AF)i);
	}
	for (int i = helper.pRange.first; i <= helper.pRange.second; i++)
	{
		for (auto eff : prefixes[(AF)i])
		{
			if (eff.second <= aLvl && canRollAf(bi.t, (AF)i))
				prepools[(AF)i].push_back({ eff.first,eff.second });
		}
		if (prepools[(AF)i].size() > 0)
			prechoices.push_back((AF)i);
	}

	std::vector<AF> choices = prechoices;
	std::map<AF, std::vector<std::pair<AFV, int>>> affixes = prepools;
	for (int c = 0; c < 2; c++)
	{
		for (int i = 0; i < rndNumAffixes; i++)
		{
			dist.reset();
			dist = std::uniform_int_distribution<>(0, choices.size() - 1);
			int choice = dist(gen);
			AF af = (AF)choices[choice];
			dist.reset();
			dist = std::uniform_int_distribution<>(0, affixes[af].size() - 1);
			int rndAfv = dist(gen);
			itmAfvs[af] = affixes[af][rndAfv].first;
			affixes[af].erase(affixes[af].begin() + rndAfv);
			if (affixes[af].size() == 0)
				choices.erase(choices.begin() + choice);
			if (choices.size() <= 0)
				break;
		}
		affixes = sufpools;
		choices = sufchoices;
		afRange = helper.sRange;
		rndNumAffixes = numS;
	}
	Item* itm = new Item{ bi.id,itmAfvs,bi,bi.t,rarity };
	return itm;
}

