#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <SFML/Audio.hpp>
#include <string>
#include <map>
class SoundManager
{
private:
	std::map<std::string, sf::SoundBuffer> sounds;

	std::map<std::string, sf::Music> musics;

public:
	void loadSound(const std::string& name, const std::string &filename);
	void loadMusic(const std::string& name, const std::string &filename);
	sf::SoundBuffer& getSoundRef(const std::string& sound);
	sf::Music& getMusicRef(const std::string& music);
	SoundManager()
	{
	}
};

#endif