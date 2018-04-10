#include <stdio.h>
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include <map>

class CaveGen {
public:
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
					map[r*width + c] = 1; // start alive
				else
					map[r*width + c] = 0; // start dead
			}
		}
		for (int i = 0; i < iterations; i++)
			iterate(); // New cycle based on current map state
		for (int c = 0; c < width; c++) // Border columns
		{
			map[c] = 1;
			map[(height - 1)*width + c] = 1;
		}
		for (int r = 0; r < height; r++) // Border rows
		{
			map[r*width] = 1;
			map[r*width + (width - 1)] = 1;
		}
		//print();
	}
	CaveGen() {}
	char getTile(int c, int r) // Get tile at
	{
		if (map[r*width + c] != 1)
			return ' ';
		else
			return '#';
	}
	void print() { // Print the cave
		for (int r = 0; r < height; r++)
		{
			for (int c = 0; c < width; c++)
			{
				std::cout << iToChar(map[r*width + c]) << " ";
			}
			std::cout << "\n";
		}
	}
	void iterate() { // Iterate on current cave state
		const int size = width * height;
		newMap = new int[size]; // New state
		for (int c = 0; c < width; c++)
		{
			for (int r = 0; r < height; r++)
			{
				int nCount = getAliveNeighbors(c, r); // number of adjacent living neighbors
				if ((bool)map[r*width + c]) { // If alive
					if (nCount < dLimit) // If the number of alive neighbors is less than deathlimit
						newMap[r*width + c] = 0; // this node is dead
					else
						newMap[r*width + c] = 1; // this node is alive
				}
				else // If dead
				{
					if (nCount > bLimit) // If the number of alive neighbors is greater than birthlimit
						newMap[r*width + c] = 1; // this node is alive
					else
						newMap[r*width + c] = 0; // this node is dead
				}
			}
		}
		// Copy the new map to our map pointer
		std::memcpy(map, newMap, size * sizeof(int));
	}
	// Connects caves to main cave
	void floodConnect(std::map<int, std::vector<sf::Vector2i>> caves)
	{
		// Find which cave is largest
		int max = 0, maxCave = -1;
		for (auto c : caves)
		{
			if (c.second.size() > max)
			{
				max = c.second.size();
				maxCave = c.first;
			}
		}
		// For each cave
		for (auto cave : caves)
		{
			if (cave.first == maxCave)
				continue;
			// Find random start ( from largest cave )
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dist(0, caves[maxCave].size() - 1);
			sf::Vector2i start = caves[maxCave][dist(gen)];
			// Find random end ( from connection )
			dist.reset();
			dist = std::uniform_int_distribution<>(0, cave.second.size() - 1);
			sf::Vector2i end = cave.second[dist(gen)];
			// Clear a path from largest to connection
			for (auto point : getPath(start, end))
				map[point.y*width + point.x] = 1;
		}
	}
	// Get a path from two points
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
	// Use recursive flood fill to find caves
	std::map<int, std::vector<sf::Vector2i>> flood()
	{
		print();
		std::map<int, std::vector<sf::Vector2i>> caves;
		int fcount = 2;
		for (int r = 0; r < height; r++)
			for (int c = 0; c < width; c++)
				if (map[r*width + c] == 0)
				{
					caves[fcount] = floodfill(c, r, fcount);
					fcount++;
				}
		return caves;
	}
	// Recursively floods caves
	std::vector<sf::Vector2i> floodfill(int c, int r, int fill)
	{
		std::vector<sf::Vector2i> caves = {};
		if (c < 0 || r < 0 || c >= width || r >= height) // Check bounds
			return caves;
		int target = map[r*width + c];
		if (target == fill || target == 1) // Have we been here yet, or is this a wall
			return caves;
		else
		{
			map[r*width + c] = fill; // Fill the tile
			caves.push_back({ c,r }); // Add tile to current cave
			
			// Check surrounding tiles
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
	int getAliveNeighbors(int x, int y) // Get surrounding living neighbor count
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
	std::string iToChar(int i)
	{
		switch (i)
		{
		case 0:
			return " ";
		default:
			return "#";
			break;
		}
	}
};