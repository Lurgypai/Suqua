#pragma once
#include "StatBarComponent.h"
#include "gamemode.h"

class TeamPointsReader : public StatReader {
	friend class StatBarComponent;
public:
	virtual ~TeamPointsReader() override;
	TeamPointsReader(EntityId target_);
	DominationMode* targetMode;
	unsigned int targetTeamId;
protected:
	virtual float getRatio() override;

	virtual StatReader* clone() override;
};