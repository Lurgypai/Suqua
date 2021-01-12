#include "SoundSystem.h"
#include "Sound.h"

#include "EntitySystem.h"
#include "SoundComponent.h"

SoundSystem::SoundSystem(Camera* targetCam_) :
	targetCam{targetCam_}
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	//Mix_AllocateChannels(10);
}

SoundSystem::~SoundSystem() {
	Mix_Quit();
}

void SoundSystem::loadSound(const std::string& targetFile, const std::string& tag) {
	sounds.emplace(tag, targetFile);
}

void SoundSystem::loadMusic(const std::string& targetFile, const std::string& tag) {
	music.emplace(tag, targetFile);
}

void SoundSystem::playTriggeredSounds() {
	if (EntitySystem::Contains<SoundComponent>()) {
		for (auto& sound : EntitySystem::GetPool<SoundComponent>()) {
			while (!sound.triggered.empty()) {
				const auto& triggered = sound.triggered.front();
				if (triggered.positional) {
					if (targetCam != nullptr) {
						auto camCenter = targetCam->center();
						float distance = camCenter.distance(triggered.pos);
						float ratio = 1.0f - distance / maxProximity;
						if (ratio > 0) {
							Mix_Volume(-1, maxVolume * ratio);
							float horizDistance = triggered.pos.x - camCenter.x;
							float horizRatio = (std::abs(horizDistance) / maxProximity) * 0.5 + 0.5;
							if (horizDistance < 0) {
								float left = horizRatio * maxVolume;
								Mix_SetPanning(MIX_CHANNEL_POST, left, maxVolume - left);
								playSound(triggered.tag);
							}
							else {
								float right = horizRatio * maxVolume;
								Mix_SetPanning(MIX_CHANNEL_POST, maxVolume - right, right);
								playSound(triggered.tag);
							}
						}
					}
				}
				else {
					Mix_Volume(-1, maxVolume);
					Mix_SetPanning(-1, 128, 128);
					playSound(triggered.tag);
				}

				sound.triggered.pop_front();
			}
		}
	}
}

void SoundSystem::playSound(const std::string& tag) {
	Mix_PlayChannel(-1, sounds.at(tag).sound_, 0);
}

void SoundSystem::playMusic(const std::string& tag) {
	Mix_PlayMusic(music.at(tag).music_, -1);
}

void SoundSystem::pauseMusic() {
	Mix_PauseMusic();
}

void SoundSystem::resumeMusic() {
	Mix_ResumeMusic();
}

void SoundSystem::setTargetCam(Camera* targetCam_) {
	targetCam = targetCam_;
}

void SoundSystem::setMaxVolume(int volume) {
	maxVolume = volume;
}

void SoundSystem::setMaxProximity(float distance) {
	maxProximity = distance;
}