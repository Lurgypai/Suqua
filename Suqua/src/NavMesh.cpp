#include "NavMesh.h"
#include "EntitySystem.h"
#include "PhysicsComponent.h"
#include "NavNode.h"
#include "NavZone.h"

#include <unordered_set>
#include <algorithm>

NavMesh::NavMesh(float zoneHeight_, float ignoreGapSize_) :
	zoneHeight{zoneHeight_},
	ignoreGapSize{ignoreGapSize_},
	zoneId{0}
{}

void NavMesh::addPhysicsMesh() {
	//generate all walking zones
	for (auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
		if (physics.collideable) {
			NavZone zone{ AABB{{physics.getCollider().pos.x, (physics.getCollider().pos.y - zoneHeight)}, {physics.getCollider().res.x, zoneHeight}}, "walk", {}, ++zoneId, false, false };
			zones.emplace(zone.id, zone);
		}
	}

	bool keepSplitting = true;
	while (keepSplitting) {
		keepSplitting = false;

		std::vector<unsigned int> toRemoveIds;
		std::vector<NavZone> toAddZones;
		//compare them all to eachother to decrease the amount of cases.
		for (auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
			if (physics.collideable) {
				for (auto& pair : zones) {

					const AABB& zoneA = physics.getCollider();
					AABB& zoneB = pair.second.area;

					float aLeft = zoneA.pos.x;
					float aRight = aLeft + zoneA.res.x;
					float bLeft = zoneB.pos.x;
					float bRight = bLeft + zoneB.res.x;

					if (zoneA.intersects(zoneB)) {
						toRemoveIds.push_back(pair.second.id);
						bool newAdded = false;
						if (bLeft < aLeft) {
							auto zoneCpy = pair.second;
							zoneCpy.area.res.x = aLeft - bLeft;
							zoneCpy.id = ++zoneId;
							zoneCpy.rightBlocked = false; //reset the block on the updated side
							toAddZones.push_back(zoneCpy);

						}
						if (bRight > aRight) {
							auto zoneCpy = pair.second;
							zoneCpy.area.pos.x = aRight;
							zoneCpy.area.res.x = bRight - aRight;
							zoneCpy.id = ++zoneId;
							zoneCpy.leftBlocked = false; //reset the block on the updated side
							toAddZones.push_back(zoneCpy);
						}
					}
					else {
						if (zoneA.pos.y < zoneB.pos.y && zoneA.pos.y + zoneA.res.y > zoneB.pos.y) {
							//the >= 0 is for platforms underneath
							if (bLeft - aRight <= ignoreGapSize && bLeft - aRight >= 0) {
								pair.second.leftBlocked = true;
							}
							else if (aLeft - bRight <= ignoreGapSize && aLeft - bRight >= 0) {
								pair.second.rightBlocked = true;
							}
						}
					}
				}
			}
		}
		for (auto& id : toRemoveIds) {
			for (auto iter = zones.begin(); iter != zones.end(); ++iter) {
				if (iter->second.id == id) {
					zones.erase(iter);
					break;
				}
			}
		}
		if (!toAddZones.empty()) {
			for (auto& zone : toAddZones) {
				zones.emplace(zone.id, zone);
			}
			keepSplitting = true;
		}
	}

	for (auto& firstPair : zones) {

		NavZone* rightZone = nullptr;
		NavZone* leftZone = nullptr;

		auto& firstZone = firstPair.second;
		float firstLeft = firstZone.area.pos.x;
		float firstRight = firstLeft + firstZone.area.res.x;
		float firstHeight = firstZone.area.pos.y;

		float minLeftHeight = 10000;
		float minRightHeight = 10000;

		for (auto& secondPair : zones) {
			auto& secondZone = secondPair.second;
			if (firstZone.id == secondZone.id)
				continue;

			float secondLeft = secondZone.area.pos.x;
			float secondRight = secondLeft + secondZone.area.res.x;
			float secondHeight = secondZone.area.pos.y;

			if (firstHeight <= secondHeight + secondZone.area.res.y) {
				if (!firstZone.leftBlocked && firstLeft - secondRight <= ignoreGapSize && firstLeft > secondLeft) {
					if (secondHeight - firstHeight < minLeftHeight) {
						leftZone = &secondZone;
						minLeftHeight = secondHeight - firstHeight;
					}
				}
				else if (!firstZone.rightBlocked && secondLeft - firstRight <= ignoreGapSize && firstRight < secondRight) {
					if (secondHeight - firstHeight < minRightHeight) {
						rightZone = &secondZone;
						minRightHeight = secondHeight - firstHeight;
					}
				}
			}
		}

		if (leftZone)
			firstZone.adjacentZones.push_back(leftZone->id);
		if (rightZone)
			firstZone.adjacentZones.push_back(rightZone->id);
	}
}

const NavZone& NavMesh::getZone(NavZoneId id) const {
	auto iter = zones.find(id);
	if (iter == zones.end())
		throw std::exception();
	else
		return iter->second;
}

const std::unordered_map<NavZoneId, NavZone> NavMesh::getZones() const {
	return zones;
}

const NavZone* NavMesh::getZone(Vec2f pos) const {
	for (auto& pair : zones) {
		if (pair.second.area.contains(pos))
			return &pair.second;
	}
	unsigned int minId = 0;
	float minDistance = 1000;
	for (auto& pair : zones) {
		if (pair.second.area.pos.x <= pos.x && pair.second.area.pos.x + pair.second.area.res.x >= pos.x) {
			float distance = pair.second.area.pos.y - pos.y;
			if (distance >= 0 && distance < minDistance) {
				minDistance = distance;
				minId = pair.first;
			}
		}
	}

	if (minId) {
		auto iter = zones.find(minId);
		if (iter != zones.end())
			return &(iter->second);
		else
			throw std::exception{};
	}

	return nullptr;
}

NavPath NavMesh::getPath(NavZoneId startZoneId, NavZoneId endZoneId) {
	std::list<NavNode> nodes;
	std::unordered_map<NavZoneId, NavNode> finishedNodes;
	for (auto& pair : zones) {
		if (pair.first != startZoneId) {
			nodes.emplace_back(NavNode{ pair.first, 0, -1 });
		}
	}
	nodes.emplace_front(NavNode{ startZoneId, startZoneId, 0 });

	//while we haven't arrived
	bool arrived = false;
	while(!arrived) {
		//pop and store us for later
		finishedNodes.emplace(nodes.front().zoneId, nodes.front());
		NavNode currNode = std::move(nodes.front());
		nodes.pop_front();

		//for all adjacent nodes
		for (const auto& adjacentId : zones[currNode.zoneId].adjacentZones) {
			if (adjacentId != currNode.fromid) {
				for (auto& adjacentNode : nodes) {
					if (adjacentNode.zoneId == adjacentId) {
						//find the total distance (distance from current node to adjacent + distance from current node to start)
						float distance = currNode.distance + zones[currNode.zoneId].area.center().distance(zones[adjacentId].area.center());
						//if the distance is less, update.
						if (adjacentNode.distance == -1 || adjacentNode.distance > distance) {
							adjacentNode.fromid = currNode.zoneId;
							adjacentNode.distance = distance;
						}
					}
				}
			}
		}
		//re-sort the nodes now that they've been updated
		nodes.sort([](const NavNode& a, const NavNode& b) {
			if (a.distance == -1)
				return false;
			if (b.distance == -1)
				return true;
			return a.distance < b.distance;
			});

		if (currNode.zoneId == endZoneId)
			arrived = true;
	}

	auto currId = endZoneId;
	std::list<NavZoneId> path{};
	while (currId != startZoneId) {
		path.push_front(currId);
		auto iter = finishedNodes.find(currId);
		if (iter != finishedNodes.end())
			currId = finishedNodes[currId].fromid;
		else
			throw std::exception{};
	}
	path.push_front(currId);
	return NavPath{path};
}

//return type