#include <stdio.h>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include "PathFinder.h"
#include <map>

class CaveGen {
public:
	enum Tile
	{
		Unused = ' ',
		Floor = '.',
		Corridor = ',',
		Wall = '#',
		ClosedDoor = '+',
		OpenDoor = '-',
		UpStairs = '<',
		DownStairs = '>'
	};
	char iToTile[2] = { '#', '.' };
	CaveGen(int _map[], const int _width, const int _height, int _dLimit, int _bLimit, int iterations) {
		this->map = _map;
		this->width = _width;
		this->height = _height;
		this->dLimit = _dLimit;
		this->bLimit = _bLimit;
		int liveChance = 40; // 40% chance to start open
		for (int r = 0; r < height; r++)
		{
			for (int c = 0; c < width; c++)
			{
				if (random() < liveChance)
					map[r*width + c] = 1;
				else
					map[r*width + c] = 0;
			}
		}
		for (int i = 0; i < iterations; i++)
			iterate();
		for (int c = 0; c < width; c++)
		{
			map[c] = 0;
			map[(height - 1)*width + c] = 0;
		}
		for (int r = 0; r < height; r++)
		{
			map[r*width] = 0;
			map[r*width + (width - 1)] = 0;
		}
		print();
	}
	CaveGen()
	{

	}
	char getTile(int c, int r)
	{
		if (map[r*width + c] == 0)
			return '#';
		else
			return ' ';
	}
	void print() {
		for (int r = 0; r < height; r++)
		{
			for (int c = 0; c < width; c++)
			{
				std::cout << iToChar(map[r*width + c]) << " ";
			}
			std::cout << "\n";
		}
	}
	void iterate() {
		const int size = width * height;
		newMap = new int[size];
		for (int c = 0; c < width; c++)
		{
			for (int r = 0; r < height; r++)
			{
				int nCount = getAliveNeighbors(c, r);
				if ((bool)map[r*width + c]) {
					if (nCount < dLimit)
						newMap[r*width + c] = 0;
					else
						newMap[r*width + c] = 1;
				}
				else
				{
					if (nCount > bLimit)
						newMap[r*width + c] = 1;
					else
						newMap[r*width + c] = 0;
				}
			}
		}
		std::memcpy(map, newMap, size * sizeof(int));
	}

	void floodConnect(std::map<int, std::vector<sf::Vector2i>> caves)
	{
		PathFinder pf;
		int max = 0, maxCave = -1;
		for (auto c : caves)
		{
			if (c.second.size() > max)
			{
				max = c.second.size();
				maxCave = c.first;
			}
		}
		for (auto cave : caves)
		{
			if (cave.first == maxCave)
				continue;
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dist(0, caves[maxCave].size() - 1);
			sf::Vector2i start = caves[maxCave][dist(gen)];
			dist.reset();
			dist = std::uniform_int_distribution<>(0, cave.second.size() - 1);
			sf::Vector2i end = cave.second[dist(gen)];
			for (auto point : getPath(start, end))
				map[point.y*width + point.x] = 1;
		}
	}

	std::vector<sf::Vector2i> getPath(sf::Vector2i start, sf::Vector2i end)
	{
		std::vector<sf::Vector2i> path = {};
		while (start != end)
		{
			path.push_back(start);
			if (start.x < end.x)
				start.x++;
			if (start.y < end.y)
				start.y++;
			if (start.x > end.x)
				start.x--;
			if (start.y > end.y)
				start.y--;
		}
		return path;
	}

	std::map<int, std::vector<sf::Vector2i>> flood()
	{
		std::map<int, std::vector<sf::Vector2i>> caves;
		int fcount = 2;
		for (int r = 0; r < height; r++)
			for (int c = 0; c < width; c++)
				if (map[r*width + c] == 1)
				{
					caves[fcount] = floodfill(c, r, fcount);
					fcount++;
				}
		return caves;
	}
	std::vector<sf::Vector2i> floodfill(int c, int r, int fill)
	{
		std::vector<sf::Vector2i> caves = {};
		if (c < 0 || r < 0 || c >= width || r >= height)
			return caves;
		int target = map[r*width + c];
		if (target == fill || target == 0)
			return caves;
		else
		{
			map[r*width + c] = fill;
			caves.push_back({ c,r });
			for (auto cave : floodfill(c - 1, r, fill))
				caves.push_back(cave);
			for (auto cave : floodfill(c + 1, r, fill))
				caves.push_back(cave);
			for (auto cave : floodfill(c, r - 1, fill))
				caves.push_back(cave);
			for (auto cave : floodfill(c, r + 1, fill))
				caves.push_back(cave);
		}
		return caves;
	}
private:
	int* map;
	int* newMap;
	int width, height, dLimit, bLimit;
	int random() // return a value 1-100
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 99);
		return dist(gen) + 1;
	}
	int getAliveNeighbors(int x, int y)
	{
		int count = 0;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				int c = x + i;
				int r = y + j;
				if (i == 0 && j == 0) {
					continue;
				}
				else if (c < 0 || r < 0 || c >= width || r >= height) {
					count = count + 1;
				}
				else if ((bool)map[r*width + c]) {
					count = count + 1;
				}
			}
		}
		return count;
	}
	//char clist[26] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
	std::string iToChar(int i)
	{
		switch (i)
		{
		case 1:
			return " ";
		case 0:
			return "#";
		default:
			return " ";
			break;
		}
	}
};