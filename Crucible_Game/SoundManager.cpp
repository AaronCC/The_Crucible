#include "SoundManager.h"

void SoundManager::loadSound(const std::string & name, const std::string & filename)
{	
	sf::SoundBuffer sound;
	if (!sound.loadFromFile(filename))
		return;

	this->sounds[name] = sound;

	return;
}

void SoundManager::loadMusic(const std::string & name, const std::string & filename)
{
	if (!musics[name].openFromFile(filename))
		return;

	return;
}

sf::SoundBuffer & SoundManager::getSoundRef(const std::string & sound)
{
	return this->sounds.at(sound);
}

sf::Music & SoundManager::getMusicRef(const std::string & music)
{
	return this->musics.at(music);
}