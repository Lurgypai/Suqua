#include "NavMesh.h"
#include "EntitySystem.h"
#include "PhysicsComponent.h"

NavMesh::NavMesh()
{
	unsigned int zoneId{ 0 };
	//generate all walking zones
	for (auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
		if(physics.collideable) {
			NavZone zone{ AABB{{physics.getCollider().pos.x, physics.getCollider().pos.y - NavZone::HEIGHT}, {physics.getCollider().res.x, NavZone::HEIGHT}}, NavZone::Type::walk, {}, ++zoneId, false, false };
			zones.push_back(zone);
		}
	}

	bool keepSplitting = true;
	while (keepSplitting) {
		keepSplitting = false;

		std::vector<unsigned int> toRemoveIds;
		std::vector<NavZone> toAddZones;
		//compare them all to eachother to decrease the amount of cases.
		for (auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
			for (auto& zone : zones) {

				const AABB& zoneA = physics.getCollider();
				AABB& zoneB = zone.area;

				float aLeft = zoneA.pos.x;
				float aRight = aLeft + zoneA.res.x;
				float bLeft = zoneB.pos.x;
				float bRight = bLeft + zoneB.res.x;

				if (zoneA.intersects(zoneB)) {
					toRemoveIds.push_back(zone.id);
					bool newAdded = false;
					if (bLeft < aLeft) {
						auto zoneCpy = zone;
						zoneCpy.area.res.x = aLeft - bLeft;
						zoneCpy.id = ++zoneId;
						zoneCpy.rightBlocked = true;
						toAddZones.push_back(zoneCpy);

					}
					if (bRight > aRight) {
						auto zoneCpy = zone;
						zoneCpy.area.pos.x = aRight;
						zoneCpy.area.res.x = bRight - aRight;
						zoneCpy.id = ++zoneId;
						zoneCpy.leftBlocked = true;
						toAddZones.push_back(zoneCpy);
					}
				}
			}
		}
		for (auto& id : toRemoveIds) {
			for (auto iter = zones.begin(); iter != zones.end(); ++iter) {
				if (iter->id == id) {
					zones.erase(iter);
					break;
				}
			}
		}
		if (!toAddZones.empty()) {
			zones.insert(zones.end(), toAddZones.begin(), toAddZones.end());
			keepSplitting = true;
		}
	}
	

	auto zonesCopy = zones;
	for (auto leftIter = zonesCopy.begin(); leftIter != zonesCopy.end() - 1; ++leftIter) {
		for (auto rightIter = leftIter + 1; rightIter != zonesCopy.end(); ++rightIter) {
			NavZone& zoneA = *leftIter;
			NavZone& zoneB = *rightIter;
;			if (std::abs(zoneA.area.pos.y - zoneB.area.pos.y) < NavZone::JUMP_HEIGHT) {
				float aDistance = zoneA.area.pos.x - (zoneB.area.pos.x + zoneB.area.res.x);
				float bDistance = zoneB.area.pos.x - (zoneA.area.pos.x + zoneA.area.res.x);
				if (aDistance > 0 && aDistance < NavZone::JUMP_DISTANCE) {
					NavZone zone{ AABB{{zoneB.area.pos.x + zoneB.area.res.x, zoneB.area.pos.y}, {zoneA.area.pos.x - (zoneB.area.pos.x + zoneB.area.res.x), NavZone::HEIGHT}}, NavZone::Type::jump, {}, ++zoneId };
					zones.push_back(zone);
				}
				else if (bDistance > 0 && bDistance < NavZone::JUMP_DISTANCE) {
					NavZone zone{ AABB{{zoneA.area.pos.x + zoneA.area.res.x, zoneA.area.pos.y}, {zoneB.area.pos.x - (zoneA.area.pos.x + zoneA.area.res.x), NavZone::HEIGHT}}, NavZone::Type::jump, {}, ++zoneId };
					zones.push_back(zone);
				}
			}
		}
	}
	
	for (auto& firstZone : zones) {

		NavZone* rightZone = nullptr;
		NavZone* leftZone = nullptr;

		for (auto& secondZone : zones) {
			if (firstZone.id == secondZone.id)
				continue;

			float firstLeft = firstZone.area.pos.x;
			float firstRight = firstLeft + firstZone.area.res.x;
			float firstHeight = firstZone.area.pos.y;
			float secondLeft = secondZone.area.pos.x;
			float secondRight = secondLeft + secondZone.area.res.x;
			float secondHeight = secondZone.area.pos.y;

			float minLeftHeight = 10000;
			float minRightHeight = 10000;

			if (firstHeight <= secondHeight) {
				if (firstLeft - secondRight <= 1 && firstLeft > secondLeft) {
					if (secondHeight - firstHeight < minLeftHeight) {
						leftZone = &secondZone;
						minLeftHeight = secondHeight - firstHeight;
					}
				}
				else if (secondLeft - firstRight <= 1 && firstRight < secondRight) {
					if (secondHeight - firstHeight < minRightHeight) {
						rightZone = &secondZone;
						minRightHeight = secondHeight - firstHeight;
					}
				}
			}
		}

		if (leftZone)
			firstZone.adjacentZones.push_back(leftZone);
		if (rightZone)
			firstZone.adjacentZones.push_back(rightZone);
	}
}

const std::vector<NavZone>& NavMesh::getZones() const {
	return zones;
}