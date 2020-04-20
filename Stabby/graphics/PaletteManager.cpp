
#include "PaletteManager.h"
#include <filesystem>
#include <iostream>

PaletteManager::PaletteManager() :
	palettes{},
	currentPalette{nullptr}
{}

void PaletteManager::loadPalettes(const std::string & path_) {
	namespace fs = std::filesystem;

	fs::path filePath{ path_ };
	for (auto& file : fs::directory_iterator{ path_ }) {
		palettes.emplace(file.path().stem().string(), Sprite{ file.path().string() });
	}

	setCurrentPalette("default");
}

void PaletteManager::clearPalettes() {
	palettes.clear();
}

void PaletteManager::reloadPalettes(const std::string& path_) {
	clearPalettes();
	reloadPalettes(path_);
} 

void PaletteManager::setCurrentPalette(const std::string& tag) {
	auto palette = palettes.find(tag);
	if(palette != palettes.end())
		currentPalette = &(palette->second);
}

const Sprite& PaletteManager::getCurrentPalette() const {
	return *currentPalette;
}

