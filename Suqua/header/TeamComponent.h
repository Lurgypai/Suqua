#pragma once
#include "ComponentMacros.h"

using TeamId = std::uint64_t;

class TeamComponent {
	CompMembers(TeamComponent);

public:
	TeamId teamId;
};

/* teams
0 - neutral
1 - player
2 - enemy
*/