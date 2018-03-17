#ifndef HELPER_H
#define HELPER_H
#include <SFML/Graphics.hpp>

class Helper
{
public:

	static const int numbuffs = 15;
	std::string buffnames[numbuffs] = {
		"fire damage",
		"cold damage",
		"lightning damage",
		"poison damage",
		"physical damage",
		"% global fire damage",
		"% global cold damage",
		"% global lightning damage",
		"% global poison damage",
		"% local physical damage",
		"% global physical damage",
		"agility",
		"defense",
		"knowledge",
		"attack"
	};
	sf::Color damageColors[5] = {
		{ 226, 165, 86 },
		{ 255, 74, 61 },
		{ 109, 165, 255 },
		{ 248, 255, 61 },
		{ 131, 211, 69 }
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
	std::pair<int, int> sRange = std::pair<int, int>{ 0,4 };
	std::pair<int, int> pRange = std::pair<int, int>{ 5,14 };
	enum Affix {
		//SUFFIX
		FIRE_FLT_DMG,
		COLD_FLT_DMG,
		LGHT_FLT_DMG,
		POIS_FLT_DMG,
		PHYS_FLT_DMG,
		//PREFIX
		FIRE_PRC_DMG,
		COLD_PRC_DMG,
		LGHT_PRC_DMG,
		POIS_PRC_DMG,
		PHYS_PRC_L_DMG,
		PHYS_PRC_G_DMG,
		AGI,
		DEF,
		KNO,
		ATK
	};

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
	std::string damagenames[5] = {
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
	Helper();
	~Helper();
};
#endif