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

	static int SuquaInit(const std::string& windowName, const std::string& settings, ModeType mode_);
	static void RunGame(Game& game);
	static CamId getDebugCamId();
private:
	static CamId debugCamId;
	static ModeType mode;
};