#include "SoundSystem.h"
#include "Sound.h"

#include "EntitySystem.h"
#include "SoundComponent.h"

SoundSystem::SoundSystem() {
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
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
				playSound(sound.triggered.front());
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