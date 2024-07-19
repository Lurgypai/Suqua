#pragma once
#include "ComponentMacros.h"

#include "World.h"
#include "Vec2.h"

// a bunch of rays for detecting things for now I guess
class FeelerComponent {
	CompMembers(FeelerComponent);

public:
	void update(const World& level);
    Vec2f ray;

private:
};
