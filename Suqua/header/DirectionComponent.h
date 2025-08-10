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
private:
	bool isLocked_;
	float dir;
	int cardinalDir;
};
