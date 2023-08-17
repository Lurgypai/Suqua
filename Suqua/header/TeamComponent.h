#pragma once
#include "ComponentMacros.h"

class TeamComponent {
	CompMembers(TeamComponent);


public:
	enum class TeamId {
	neutral,
	player,
	enemy,
	};

	TeamId teamId;
};

/* teams
0 - neutral
1 - player
2 - enemy
*/