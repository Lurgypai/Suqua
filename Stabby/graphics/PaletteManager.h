#pragma once
#include "Sprite.h"
#include <unordered_map>

class PaletteManager {
public:
	PaletteManager();

	void loadPalettes(const std::string & path_);
	void clearPalettes();
	void reloadPalettes(const std::string & path_);
	void setCurrentPalette(const std::string& tag);
	const Sprite& getCurrentPalette() const;
private:
	std::unordered_map<std::string, Sprite> palettes;
	Sprite* currentPalette;
};