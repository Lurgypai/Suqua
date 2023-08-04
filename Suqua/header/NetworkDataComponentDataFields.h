#pragma once
#include "NetworkDataComponent.h"

//moved these into one file so they can be changed more easily without colliding

namespace PositionData {
	constexpr NetworkDataComponent::DataId X = 1;
	constexpr NetworkDataComponent::DataId Y = 2;
}

namespace DirectionData {
	constexpr NetworkDataComponent::DataId DIR = 3;
}

namespace PhysicsData {
	constexpr NetworkDataComponent::DataId WEIGHT = 4;			//float
	constexpr NetworkDataComponent::DataId XVEL = 5;			//float
	constexpr NetworkDataComponent::DataId YVEL = 6;			//float
	constexpr NetworkDataComponent::DataId GROUNDED = 7;		//bool
	constexpr NetworkDataComponent::DataId FROZEN = 8;			//bool
	constexpr NetworkDataComponent::DataId WEIGHTLESS = 9;		//bool
	constexpr NetworkDataComponent::DataId COLLIDEABLE = 10;	//bool
	constexpr NetworkDataComponent::DataId XRES = 11;			//float
	constexpr NetworkDataComponent::DataId YRES = 12;			//float
}

namespace MovementData {
	constexpr NetworkDataComponent::DataId MOVE_SPEED = 13;
}

namespace LifeTimeData {
	constexpr NetworkDataComponent::DataId LIFE_TIME = 14;
}

namespace HealthData {
	constexpr NetworkDataComponent::DataId HEALTH = 15;			//uint32_t
}