#pragma once
#include <SDL_mixer.h>
#include <string>

class Sound {

public:
	Sound(const std::string & file);
	~Sound();
	Mix_Chunk* sound_;
};