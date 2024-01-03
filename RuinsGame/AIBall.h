#pragma once
#include "Level.h"
#include "ComponentMacros.h"

class AIBallComponent {
    CompMembers(AIBallComponent);

public:
	void update();

    Vec2f left;
    Vec2f right;
    Vec2f downleft;
    Vec2f downright;

    Level* level;
private:
    float dir;
};
