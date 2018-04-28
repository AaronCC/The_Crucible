#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <SFML/Audio.hpp>
#include <string>
#include <map>
#include <vector>
class SoundManager
{
private:
	std::map<std::string, sf::SoundBuffer> sounds;
	std::map<std::string, sf::Music> musics;
	std::string playing;
	sf::SoundBuffer buffer;
	std::vector<sf::Sound> effects;
	int e_at = 0;
	std::string lastPlayed;
	const int e_count = 4;
	int sCount = 0;
	const int max_sCount = 3;
public:
	bool isMute = false;
	void clearSounds() { sCount = 0; }
	void loadSound(const std::string& name, const std::string &filename);
	void loadMusic(const std::string& name, const std::string &filename);
	sf::SoundBuffer& getSoundRef(const std::string& sound);
	void mute();
	void playSound(const std::string & sound);
	void stopPlaying();
	sf::Music& getMusicRef(const std::string& music);
	SoundManager()
	{
		effects.reserve(e_count);
		for (int i = 0; i < e_count; i++)
			effects.push_back(sf::Sound());
	}
};

#endif