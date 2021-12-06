#pragma once
#include "Game.h"

class SuquaLib {
public:
	using ModeType = unsigned char;
	enum Mode : ModeType {
		graphics = 1 << 0,
		network = 1 << 1,
		all = graphics | network
	};

	static int SuquaInit(const std::string& settings, ModeType mode_, Vec2i viewRes);
	static void RunGame(Game& game);
	static CamId getDebugCamId();
private:
	static CamId debugCamId;
	static ModeType mode;
};