#pragma once
#include "ComponentMacros.h"

#include "Level.h"
#include "Vec2.h"

// a bunch of rays for detecting things for now I guess
class FeelerComponent {
	CompMembers(FeelerComponent);

public:
	void update(const Level& level);
    Vec2f ray;

private:
};
