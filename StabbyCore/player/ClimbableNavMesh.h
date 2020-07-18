#pragma once
#include "NavMesh.h"

class ClimbableNavMesh : public NavMesh {
public:
	ClimbableNavMesh(float zoneHeight_, float ignoreGapSize_, float maxClimbableGrabDist_);
	void addClimbableMesh();
private:
	float maxClimbableGrabDist;
};