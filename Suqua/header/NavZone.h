
#include <vector>

#include "AABB.h"

struct NavZone {
	enum class Type {
		walk,
		climb,
		jump
	};

	AABB area;
	Type type;
	std::vector<NavZone*> adjacentZones;
	unsigned int id; //unique id, to easily identify zone
	bool rightBlocked;
	bool leftBlocked;

	static constexpr float HEIGHT = 20.0f;
	static constexpr float JUMP_DISTANCE = 25.0f;
	static constexpr float JUMP_HEIGHT = 4.0f;
};