#include "Music.h"

Music::Music(const std::string& file) :
	music_{}
{
	music_ = Mix_LoadMUS(file.c_str());
	if (!music_) //unable to load file, freak out
		throw std::exception{};
}

Music::~Music() {
	Mix_FreeMusic(music_);
}