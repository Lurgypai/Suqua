#pragma once
#include "Game.h"

class SuquaLib {
public:
	enum Mode : unsigned char {
		graphics = 1 << 0,
		physics = 1 << 1,
		all = graphics | physics
	};

	static int SuquaInit(const std::string& settings, unsigned char mode, Vec2i viewRes);
	static void RunGame(Game& game);
	static CamId getDebugCamId();
private:
	static CamId debugCamId;
};