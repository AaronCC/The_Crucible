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
Scroll* ItemGenerator::makeScroll(int aLvl, float mf)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	Item::Rarity rarity = getRarity(mf);
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
	std::map<EfType, std::vector<AbEffect>> pools;
	std::vector<EfType> choices;
	for (int i = 0; i < numEfType; i++)
	{
		for (auto eff : abAffixes[(EfType)i])
		{
			if (eff.second <= aLvl)
				pools[(EfType)i].push_back(eff.first);
		}
		if (pools[(EfType)i].size() > 0)
			choices.push_back((EfType)i);
	}
	if (choices.size() > 0)
		for (int i = 0; i < numaff; i++)
		{
			dist.reset();
			dist = std::uniform_int_distribution<>(0, choices.size() - 1);
			int choice = dist(gen);
			EfType efType = choices[choice];
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
	dist.reset();
	dist = std::uniform_int_distribution<>(0, Ability::numarea - 1);
	Ability::Area area = (Ability::Area)dist(gen);
	dist.reset();
	dist = std::uniform_int_distribution<>(0, 3);
	int range = dist(gen) + 1;
	Ability::AbInfo info{ area,range };
	ab->setInfo(info);
	for (auto eff : effs)
		ab->addEffect(eff);
	scroll = new Scroll(ab->name, Item::SlotType::SCR, ab, rarity);
	return scroll;
}
/*	switch (efType)
	{
	case EfType::INST:
		break;
	case EfType::BUFF:
		break;
	case EfType::DEBUFF:
		break;
	case EfType::DOT:
		break;
	}*/
	/*
		AbEffect eff = AbEffect(AbEffect::Effect({ 0,0,0,0 },
			AbEffect::Damage(
				AbEffect::DamageType::COLD, 1, 10),
			1));
		AbEffect eff2 = AbEffect(AbEffect::Effect({ 0,0,0,0 },
			AbEffect::Damage(
				AbEffect::DamageType::PHYS, 1, 1),
			5));*/
Item * ItemGenerator::makeItem(int aLvl, float mf)
{
	Helper helper;
	int offset = 0;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, numst - 1);

	int st = dist(gen);
	ST type = ST::MAH;//(ST)st;
	std::vector<BaseItem> basePool;
	for (auto base : bases[type])
	{
		if (base.first < aLvl)
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
	std::map<AF, std::vector<std::pair<AFV, int>>> affixes = prefixes;
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
	for (int c = 0; c < 2; c++)
	{
		for (int i = 0; i < rndNumAffixes; i++)
		{
			dist.reset();
			dist = std::uniform_int_distribution<>(0, afRange.second - afRange.first);
			AF af = (AF)(dist(gen) + afRange.first);
			while (affixes[af].size() <= 0)
			{
				af = (AF)(dist(gen) + afRange.first);
			}
			dist.reset();
			dist = std::uniform_int_distribution<>(0, affixes[af].size() - 1);
			int rndAfv = dist(gen);
			itmAfvs[af] = affixes[af][rndAfv].first;
			affixes[af].erase(affixes[af].begin() + rndAfv);
		}
		affixes = suffixes;
		afRange = helper.sRange;
		rndNumAffixes = numS;
	}
	Item* itm = new Item{ bi.id,itmAfvs,bi,bi.t,rarity };
	return itm;
}
