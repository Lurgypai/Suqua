#pragma once
#include <SDL_mixer.h>
#include <string>

class Music {

public:
	Music(const std::string& file);
	~Music();
	Mix_Music* music_;
};