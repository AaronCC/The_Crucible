#include "Item.h"

void Item::updateBaseVals()
{
	std::vector<AbEffect::Damage> damage;
	if (base.aff[AF::PHYS_FLT_DMG].getStr() != "" )
	{
		if (buffs[AF::PHYS_FLT_DMG].getStr() != "")
		{
			AFV buff = buffs[AF::PHYS_FLT_DMG];
			base.aff[AF::PHYS_FLT_DMG].v1 += buff.v1;
			base.aff[AF::PHYS_FLT_DMG].v2 += buff.v2;
		}
		if (buffs[AF::PHYS_PRC_L_DMG].getStr() != "")
		{
			AFV buff = buffs[AF::PHYS_PRC_L_DMG];
			float newMin = base.aff[AF::PHYS_FLT_DMG].v1 * ((buff.v1 / 100.f) + 1.f),
				newMax = base.aff[AF::PHYS_FLT_DMG].v2 * ((buff.v1 / 100.f) + 1.f);
			base.aff[AF::PHYS_FLT_DMG].v1 = std::ceil(newMin);
			base.aff[AF::PHYS_FLT_DMG].v2 = std::ceil(newMax);
		}
		damage.push_back(AbEffect::Damage{ AbEffect::DamageType::PHYS,base.aff[AF::PHYS_FLT_DMG].v1,base.aff[AF::PHYS_FLT_DMG].v2 });
	}
	if (buffs[AF::FIRE_FLT_DMG].getStr() != "")
	{
		base.aff[AF::FIRE_FLT_DMG] = buffs[AF::FIRE_FLT_DMG];
		damage.push_back(AbEffect::Damage{ AbEffect::DamageType::FIRE,base.aff[AF::FIRE_FLT_DMG].v1,base.aff[AF::FIRE_FLT_DMG].v2 });
	}
	if (buffs[AF::POIS_FLT_DMG].getStr() != "")
	{
		base.aff[AF::POIS_FLT_DMG] = buffs[AF::POIS_FLT_DMG];
		damage.push_back(AbEffect::Damage{ AbEffect::DamageType::POIS,base.aff[AF::POIS_FLT_DMG].v1,base.aff[AF::POIS_FLT_DMG].v2 });
	}
	if (buffs[AF::LGHT_FLT_DMG].getStr() != "")
	{
		base.aff[AF::LGHT_FLT_DMG] = buffs[AF::LGHT_FLT_DMG];
		damage.push_back(AbEffect::Damage{ AbEffect::DamageType::LGHT,base.aff[AF::LGHT_FLT_DMG].v1,base.aff[AF::LGHT_FLT_DMG].v2 });
	}
	if (buffs[AF::COLD_FLT_DMG].getStr() != "")
	{
		base.aff[AF::COLD_FLT_DMG] = buffs[AF::COLD_FLT_DMG];
		damage.push_back(AbEffect::Damage{ AbEffect::DamageType::COLD,base.aff[AF::COLD_FLT_DMG].v1,base.aff[AF::COLD_FLT_DMG].v2 });
	}
	setDamage(damage);
}

Item::~Item()
{
}

Helper::Stats Item::getStatBuffs()
{
	Helper::Stats stats = Helper::Stats();
	for (auto buff : buffs)
	{
		if (!buff.second.local)
		{
			stats.buffs[buff.first] = buff.second;
		}
	}
	for (auto buff : base.aff)
	{
		if (!buff.second.local)
		{
			stats.buffs[buff.first] = stats.buffs[buff.first] + buff.second;
		}
	}
	return stats;
}