#include "ClimbableNavMesh.h"
#include "../stage/ClimbableComponent.h"

ClimbableNavMesh::ClimbableNavMesh(float zoneHeight_, float ignoreGapSize_, float maxClimbableGrabDist_) :
	NavMesh(zoneHeight_, ignoreGapSize_),
	maxClimbableGrabDist{maxClimbableGrabDist_}
{}

void ClimbableNavMesh::addClimbableMesh() {
	for (auto& climbable : EntitySystem::GetPool<ClimbableComponent>()) {
		NavZone zone{ climbable.collider, "climb", {},  ++zoneId, false, false };
		zones.emplace(zone.id, zone);
	}

	for (auto& pair : zones) {
		auto& climbZone = pair.second;
		if (climbZone.typeTag == "climb") {

			float climbLeft = climbZone.area.pos.x;
			float climbRight = climbLeft + climbZone.area.res.x;
			float climbTop = climbZone.area.pos.y;
			float climbBottom = climbTop + climbZone.area.res.y;

			for (auto& walkPair : zones) {
				auto& walkZone = walkPair.second;
				if (walkZone.typeTag == "walk") {
					if (climbZone.area.intersects(walkZone.area)) {
						climbZone.adjacentZones.push_back(walkZone.id);
						walkZone.adjacentZones.push_back(climbZone.id);
					}
					else {
						float walkLeft = walkZone.area.pos.x;
						float walkRight = walkLeft + walkZone.area.res.x;
						float walkTop = walkZone.area.pos.y;
						float walkBottom = walkTop + walkZone.area.res.y;
						//next to or above
						if (climbLeft - walkRight < ignoreGapSize && walkLeft - climbRight < ignoreGapSize &&
							climbTop <= walkBottom &&
							climbBottom >= walkTop - maxClimbableGrabDist) {
							climbZone.adjacentZones.push_back(walkZone.id);
							walkZone.adjacentZones.push_back(climbZone.id);
						}
					}
				}
			}
		}
	}
}
