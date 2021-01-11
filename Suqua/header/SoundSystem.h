#pragma once
#include <unordered_map>
#include "Sound.h"
#include "Music.h"
#include "GLRenderer.h"

class SoundSystem {
public:
	SoundSystem(Camera* targetCam_ = nullptr);
	~SoundSystem();
	void loadSound(const std::string& targetFile, const std::string& tag);
	void loadMusic(const std::string& targetFile, const std::string& tag);

	void playTriggeredSounds();

	void playSound(const std::string& tag);
	void playMusic(const std::string& tag);
	void pauseMusic();
	void resumeMusic();

	void setTargetCam(Camera* targetCam_);
	void setMaxVolume(int volume);
	void setMaxProximity(float distance);
private:
	std::unordered_map<std::string, Sound> sounds;
	std::unordered_map<std::string, Music> music;

	Camera* targetCam;
	int maxVolume;
	float maxProximity;
};