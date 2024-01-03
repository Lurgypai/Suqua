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

int SuquaLib::SuquaInit(const std::string& windowName, const std::string& settingsFile, ModeType mode_) {
	mode = mode_;
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

	if (mode & Mode::graphics) {
		if (settings.contains("window_width") && settings.contains("window_height")) {
			windowRes.x = settings["window_width"];
			windowRes.y = settings["window_height"];
		}
		else {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Settings file missing fields.");
			return 1;
		}

		SDL_Init(SDL_INIT_VIDEO);

		SDL_Window* window = NULL;
		window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowRes.x, windowRes.y, SDL_WINDOW_OPENGL);

		if (window == NULL) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window Creation Failed.");
			return 1;
		}

		GLRenderer::Init(window, windowRes);
		DebugIO::startDebug("suqua/fonts/consolas.fnt");
		debugCamId = GLRenderer::addCamera(Camera{ Vec2f{ 0.0f, 0.0f }, Vec2i{ windowRes.x, windowRes.y }, .5 });
		GLRenderer::LoadTexture("suqua/fonts/consolas_0.png", "debug_font");
	}
	if (mode & Mode::network) {
		enet_initialize();
	}

    SDL_StopTextInput();
	return 0;
}

void SuquaLib::RunGame(Game& game) {
	game.loop();
}

CamId SuquaLib::getDebugCamId() {
	return debugCamId;
}

void SuquaLib::DrawConsole() {
	GLRenderer::setCamera(debugCamId);
	DebugIO::drawLines();
}

CamId SuquaLib::debugCamId{};
SuquaLib::ModeType SuquaLib::mode{};
