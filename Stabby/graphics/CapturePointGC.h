#pragma once
#include "EntitySystem.h"
#include "Color.h"
#include <array>

class CapturePointGC {
public:
	CapturePointGC(EntityId id_ = 0);
	EntityId getId() const;

	void update(double timeDelta);
	
private:

	Color currColor;
	//ticker for color swapper
	int currTick;
	//current time of color swap counter
	double currTime;
	//total delay for color swap counter
	double colorSwapDelay;
	EntityId id;

	const static std::array<Color, 3> teamColors;
};