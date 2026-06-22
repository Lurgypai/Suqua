#pragma once
#include "ComponentMacros.h"

class DirectionComponent {
    CompMembers(DirectionComponent);
public:
	DirectionComponent(EntityId id_);

	float getDir() const;
	void setDir(float newDir);
	int getCardinalDir() const;
	
	bool isLocked();
	void setLocked(bool locked);

	float dir;
private:
	bool isLocked_;
	int cardinalDir;
};
