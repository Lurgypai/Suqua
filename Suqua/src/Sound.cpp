#include "Sound.h"

Sound::Sound(const std::string& file) :
	sound_{}
{
	sound_ = Mix_LoadWAV(file.c_str());
	if (!sound_)
		throw std::exception{};
}

Sound::~Sound() {
	Mix_FreeChunk(sound_);
}