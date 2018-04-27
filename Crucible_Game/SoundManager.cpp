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
void SoundManager::mute() {
	this->isMute = !this->isMute;
	if (isMute)
		stopPlaying();
	else
		getMusicRef(playing);
}
void SoundManager::playSound(const std::string & sound)
{
	if (isMute) return;
	if (sCount > max_sCount) return;
	sCount++;
	effects[e_at].setBuffer(sounds.at(sound));
	effects[e_at].play();
	e_at++;
	e_at = e_at == e_count ? 0 : e_at;
}
void SoundManager::stopPlaying()
{
	if (playing != "")
		this->musics.at(playing).stop();
}
sf::Music & SoundManager::getMusicRef(const std::string & music)
{
	if (isMute) return this->musics.at(music);
	if (playing != "")
		this->musics.at(playing).stop();
	this->musics.at(music).setVolume(25);
	this->musics.at(music).play();
	this->musics.at(music).setLoop(true);
	playing = music;
	return this->musics.at(music);
}