#include "TeamPointsReader.h"
#include "player.h"
#include "combat.h"

TeamPointsReader::~TeamPointsReader()
{}

TeamPointsReader::TeamPointsReader(EntityId target_) :
	StatReader{ target_ }
{}

float TeamPointsReader::getRatio() {
	unsigned int points = targetMode->getPoints(targetTeamId);
	if (points >= 0)
		return static_cast<float>(points) / static_cast<float>(targetMode->getWinningPoints());
	else
		return 0;
}

StatReader* TeamPointsReader::clone() {
	return new TeamPointsReader(*this);
}
