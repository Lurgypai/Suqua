#pragma once
#include <unordered_map>
#include "Sound.h"
#include "Music.h"

class SoundSystem {
public:
	SoundSystem();
	~SoundSystem();
	void loadSound(const std::string& targetFile, const std::string& tag);
	void loadMusic(const std::string& targetFile, const std::string& tag);

	void playTriggeredSounds();

	void playSound(const std::string& tag);
	void playMusic(const std::string& tag);
	void pauseMusic();
	void resumeMusic();
private:
	std::unordered_map<std::string, Sound> sounds;
	std::unordered_map<std::string, Music> music;
};