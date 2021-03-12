#include "SuquaLib.h"
#include "DebugIO.h"
#include "nlohmann/json.hpp"
#include <SDL.h>
#include <fstream>

#if defined(WIN32) || defined(_WIN32)
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

using json = nlohmann::json;

int SuquaLib::SuquaInit(const std::string& settingsFile, unsigned char mode, Vec2i viewRes) {
	
	json settings;
	std::ifstream file{ settingsFile };
	Vec2i windowRes;
	if (file.good()) {
		file >> settings;
		file.close();
	}
	else {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to open settings file.");
		return 1;
	}

	if (settings.contains("window_width") &&  settings.contains("window_height")) {
		windowRes.x = settings["window_width"];
		windowRes.y = settings["window_height"];
	}
	else {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Settings file missing fields.");
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = NULL;
	window = SDL_CreateWindow("Stabby", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowRes.x, windowRes.y, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window Creation Failed.");
		return 1;
	}

	DebugIO::startDebug("suqua/fonts/consolas_0.png", "suqua/fonts/consolas.fnt");
	debugCamId = GLRenderer::addCamera(Camera{ Vec2f{ 0.0f, 0.0f }, Vec2i{ windowRes.x, windowRes.y }, .5 });

	GLRenderer::Init(window, windowRes, viewRes);

	return 0;
}

void SuquaLib::RunGame(Game& game) {
	game.loop();
}

CamId SuquaLib::getDebugCamId() {
	return debugCamId;
}

CamId SuquaLib::debugCamId{};
