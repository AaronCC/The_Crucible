#ifndef HELPER_H
#define HELPER_H
#include <SFML/Graphics.hpp>

#define NORM_COLOR sf::Color::White
#define MAGIC_COLOR { 85, 121, 255 }
#define RARE_COLOR { 196, 166, 60 }
#define ULTRA_COLOR { 147, 47, 173 }
#define DEX_MOD 5

class Helper
{
public:

	static const int numbuffs = 21;

	std::vector<std::string> buffnames = {
		"bonus health",
		"fire damage",
		"cold damage",
		"lightning damage",
		"poison damage",
		"physical damage",
		"% fire resistance",
		"% lightning resistance",
		"% cold resistance",
		"% poison resistance",
		"% global fire damage",
		"% global cold damage",
		"% global lightning damage",
		"% global poison damage",
		"% weapon physical damage",
		"% global physical damage",
		"armor rating",
		"agility",
		"defense",
		"knowledge",
		"attack"
	};

	float linearEq(float m, int x, float b, int mod)
	{
		float result = (m * (x / mod)) + b;
		return result;
	}
	static const int numpreprefixes = 10;
	std::string inamePrePrefixes[10] = {
		"Pain",
		"Deadly",
		"Godlike",
		"Demi",
		"Gorgon",
		"Steel",
		"Rift",
		"Brood",
		"Blood",
		"Demon"
	};
	std::map<int, std::vector<std::string>> inamePrefixes;
	sf::Color damageColors[6] = {
		{ 226, 165, 86 },
		{ 255, 74, 61 },
		{ 109, 165, 255 },
		{ 248, 255, 61 },
		{ 131, 211, 69 },
		sf::Color::White
	};
	//  PHYS = 0,
	//	FIRE = 1,
	//	ICE = 2,
	//	LGHT = 3,
	//	POIS = 4
	/*damageColors[AbEffect::DamageType::FIRE] = sf::Color({ 255, 74, 61 });
	damageColors[AbEffect::DamageType::ICE] = sf::Color({ 109, 165, 255 });
	damageColors[AbEffect::DamageType::LGHT] = sf::Color({ 248, 255, 61 });
	damageColors[AbEffect::DamageType::PHYS] = sf::Color({ 226, 165, 86 });
	damageColors[AbEffect::DamageType::POIS] = sf::Color({ 131, 211, 69 });*/
	std::pair<int, int> sRange = std::pair<int, int>{ 0,9 };
	std::pair<int, int> pRange = std::pair<int, int>{ 10,20 };
	char* c_buffnames[numbuffs] = {
		"bonus health",
		"fire damage",
		"cold damage",
		"lightning damage",
		"poison damage",
		"physical damage",
		"% fire resistance",
		"% lightning resistance",
		"% cold resistance",
		"% poison resistance",
		"% global fire damage",
		"% global cold damage",
		"% global lightning damage",
		"% global poison damage",
		"% weapon physical damage",
		"% global physical damage",
		"armor rating",
		"agility",
		"defense",
		"knowledge",
		"attack"
	};
	char* c_suffix_buffnames[10] = {
		"bonus health",
		"fire damage",
		"cold damage",
		"lightning damage",
		"poison damage",
		"physical damage",
		"% fire resistance",
		"% lightning resistance",
		"% cold resistance",
		"% poison resistance" };
	char* c_prefix_buffnames[11] = {
		"% global fire damage",
		"% global cold damage",
		"% global lightning damage",
		"% global poison damage",
		"% weapon physical damage",
		"% global physical damage",
		"armor rating",
		"agility",
		"defense",
		"knowledge",
		"attack"
	};
	enum Affix {
		//SUFFIX
		HEALTH,
		FIRE_FLT_DMG,
		COLD_FLT_DMG,
		LGHT_FLT_DMG,
		POIS_FLT_DMG,
		PHYS_FLT_DMG,
		FIRE_RES,
		LGHT_RES,
		COLD_RES,
		POIS_RES,
		//PREFIX
		FIRE_PRC_DMG,
		COLD_PRC_DMG,
		LGHT_PRC_DMG,
		POIS_PRC_DMG,
		PHYS_PRC_L_DMG,
		PHYS_PRC_G_DMG,
		ARM_RAT,
		AGI,
		DEF,
		KNO,
		ATK // 19
	};
	std::map<int, Affix> dTypeAffixTable = {
		{ 0, PHYS_FLT_DMG },
		{ 1, FIRE_FLT_DMG },
		{ 2, COLD_FLT_DMG },
		{ 3, LGHT_FLT_DMG },
		{ 4, POIS_FLT_DMG }
	};
	std::vector<std::string> areaNames = { "Single Target", "Line", "Square", "Circle" };
	std::map < Affix, std::pair<bool, int> > isDamage = {
		{ HEALTH,{false,5} },
		{ FIRE_FLT_DMG,{true,1} },
		{ COLD_FLT_DMG,{true,2} },
		{ LGHT_FLT_DMG,{ true,3 } },
		{ POIS_FLT_DMG,{ true,4 } },
		{ PHYS_FLT_DMG,{ true,0 } },
		{ FIRE_RES,{ false,5 } },
		{ LGHT_RES,{ false,5 } },
		{ COLD_RES,{ false,5 } },
		{ POIS_RES,{ false,5 } },
		//PREFIX
			{ FIRE_PRC_DMG,{ false,1 } },
			{ COLD_PRC_DMG,{ false,2 } },
			{ LGHT_PRC_DMG,{ false,3 } },
			{ POIS_PRC_DMG,{ false,4 } },
			{ PHYS_PRC_L_DMG,{ false,0 } },
			{ PHYS_PRC_G_DMG,{ false,0 } },
			{ ARM_RAT,{ false,5 } },
			{ AGI,{ false,5 } },
			{ DEF,{ false,5 } },
			{ KNO,{ false,5 } },
			{ ATK,{ false,5 } }
	};
	float getAgiMod(int agi);
	float getKnoMod(int kno);
	Affix dmgTypeToAffix[4] = {
		Affix::FIRE_RES,
		Affix::COLD_RES,
		Affix::LGHT_RES,
		Affix::POIS_RES
	};
	std::string cutTrailing0s(std::string in)
	{
		int newSize = 0;
		for (int c = in.size() - 1; c >= 0; c--)
		{
			if (in[c] == '.')
			{
				newSize = c;
				break;
			}
			else if (in[c] != '0')
			{
				newSize = c + 1;
				break;
			}
		}
		return in.substr(0, newSize);
	}
	struct AffVal {
		int v1 = -1,
			v2 = -1;
		std::string id;
		bool pre;
		bool local;

		sf::Color color;
		std::string getStr() {
			if (v1 != -1)
			{
				if (v2 != -1)
				{
					return std::to_string(v1) + "-" + std::to_string(v2);
				}
				else
				{
					return std::to_string(v1);
				}
			}
			return "";
		}
		AffVal& operator-(const AffVal& other)
		{
			this->v1 = (this->v1 == -1 && other.v1 != -1) ? 0 : this->v1;
			this->v2 = (this->v2 == -1 && other.v2 != -1) ? 0 : this->v2;
			if (other.v1 != -1)
				this->v1 -= other.v1;
			if (other.v2 != -1)
				this->v2 -= other.v2;
			return *this;
		}
		AffVal& operator+(const AffVal& other)
		{
			this->v1 = (this->v1 == -1 && other.v1 != -1) ? 0 : this->v1;
			this->v2 = (this->v2 == -1 && other.v2 != -1) ? 0 : this->v2;
			if (other.v1 != -1)
				this->v1 += other.v1;
			if (other.v2 != -1)
				this->v2 += other.v2;
			return *this;
		}
		AffVal(int v1, int v2, std::string id, bool pre, sf::Color c, bool local) :
			v1(v1),
			v2(v2),
			id(id),
			color(c),
			pre(pre),
			local(local)
		{}
		AffVal() :
			v1(-1),
			v2(-1),
			id(""),
			color(sf::Color::White),
			pre(false),
			local(local)
		{}
	};

	int statCount = 4;
	std::string statnames[4] = {
		"Attack",
		"Defense",
		"Agility",
		"Knowledge"
	};
	std::vector<std::string> damagenames = {
		"Physical",
		"Fire",
		"Ice",
		"Lightning",
		"Poison" };
	struct Stats {
		std::map<Affix, AffVal> buffs;
		int speed = 1;
		Stats() {
			reset();
		}
		std::map<Affix, AffVal> getActiveBuffs() {
			std::map<Affix, AffVal> active;
			for (auto buff : buffs)
			{
				if (buff.second.v1 != -1)
					active[buff.first] = buff.second;
			}
			return active;
		}
		void reset()
		{
			for (int i = 0; i < numbuffs; i++)
			{
				buffs[(Affix)i] = AffVal();
			}
		}
		Stats& operator-(const Stats& other)
		{
			if (this != &other)
			{
				for (auto buff : buffs)
				{
					this->buffs[buff.first] = this->buffs[buff.first] - other.buffs.at(buff.first);
				}
			}
			return *this;
		}
		Stats& operator+(const Stats& other)
		{
			if (this != &other)
			{
				for (auto buff : buffs)
				{
					this->buffs[buff.first] = this->buffs[buff.first] + other.buffs.at(buff.first);
				}
			}
			return *this;
		}
		Stats& operator=(const Stats& other)
		{
			if (this != &other)
			{
				for (auto buff : buffs)
				{
					this->buffs[buff.first] = other.buffs.at(buff.first);
				}
				this->speed = other.speed;
			}
			return *this;
		}
	};
	float dotProduct(sf::Vector2f v1, sf::Vector2f v2);
	std::vector<std::pair<int, int>> getNeighbors(int x, int y, int width, int height, int area);
	std::vector<std::pair<int, int>> getNeighbors(int x, int y, int width, int height);
	sf::FloatRect resizeView(float windowW, float windowH, float aspectRatio);
	float magnitude(sf::Vector2f);
	sf::Vector2f normalized(sf::Vector2f vec, float mag);
	sf::Vector2f clamp(sf::Vector2f vec, float max);
	Helper()
	{
		/*HED = 0,
		BDY = 1,
		MAH = 2,
		OFH = 3,
		RNG = 4,
		AMU = 5,
		CLK = 6,
		BLT = 7,
		SCR = 8*/
		inamePrefixes[0].push_back("Keep");
		inamePrefixes[0].push_back("Tower");
		inamePrefixes[1].push_back("Mantle");
		inamePrefixes[1].push_back("Carapace");
		inamePrefixes[2].push_back("Stinger");
		inamePrefixes[2].push_back("Slayer");
		inamePrefixes[3].push_back("Hold");
		inamePrefixes[3].push_back("Wall");
		inamePrefixes[4].push_back("Loop");
		inamePrefixes[4].push_back("Snake");
		inamePrefixes[5].push_back("Chocker");
		inamePrefixes[5].push_back("Locket");
		inamePrefixes[6].push_back("Shroud");
		inamePrefixes[6].push_back("Fog");
		inamePrefixes[7].push_back("Coil");
		inamePrefixes[7].push_back("Snake");
		inamePrefixes[8].push_back("Scroll");
		inamePrefixes[8].push_back("Papyrus");
	}
	~Helper();
};
#endif