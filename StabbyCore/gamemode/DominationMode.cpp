
#include "DominationMode.h"
#include "../player/spawn/SpawnComponent.h"
#include "RandomUtil.h"
#include "PositionComponent.h"
#include "CapturePointComponent.h"
#include "DebugIO.h"

void DominationMode::load(SpawnSystem* spawns, unsigned int totalTeams_, unsigned int pointsPerCap_, unsigned int winningPoints_)
{
	loadData(totalTeams_, pointsPerCap_, winningPoints_);
	loadTeams();
	assignPlayers();
	assignSpawns(spawns);
	clearPoints();
}

void DominationMode::reset(SpawnSystem* spawns) {
	//clear all teams
	//reassign all teams

	//delete all capture points
	//reassign capture points

	teams.clear();
	loadTeams();

	assignPlayers();

	for (auto& capturePoint : EntitySystem::GetPool<CapturePointComponent>()) {
		EntityId id = capturePoint.getId();
		EntitySystem::FreeComps<CapturePointComponent>(1, &id);
	}

	spawns->clearTeamsAssigns();
	assignSpawns(spawns);
	clearPoints();

	for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
		player.chooseSpawn();
	}
}

void DominationMode::loadTeams() {
	for (int i = 1; i <= totalTeams; ++i) {
		teams[i].teamId = i;
	}
}

void DominationMode::loadData(unsigned int totalTeams_, unsigned int pointsPerCap_, unsigned int winningPoints_) {
	totalTeams = totalTeams_;
	pointsPerCap = pointsPerCap_;
	winningPoints = winningPoints_;

	restartDelayMax = 5 * 120;
	restartDelay = restartDelayMax;
}

void DominationMode::assignPlayers() {
	std::vector<EntityId> players;
	if (EntitySystem::Contains<PlayerLC>()) {
		size_t size = EntitySystem::GetPool<PlayerLC>().size();
		players.reserve(size);
		for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
			players.emplace_back(player.getId());
		}
	}

	while (!players.empty()) {
		size_t pos = randInt(0, players.size() - 1);
		EntityId playerId = players[pos];
		addPlayer(playerId);
		players.erase(players.begin() + pos);
	}
}

void DominationMode::assignSpawns(SpawnSystem* spawns) {
	std::vector<unsigned int> teamIds(totalTeams, 0);
	for (unsigned int i = 1; i != totalTeams + 1; teamIds[i - 1] = i, ++i);
	auto teamIdCpy = teamIds;

	//get all spawn points unassigned, assign them
	for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
		if (spawn.getTeamId() == 0) {
			if (spawn.isDefault()) {
				auto pos = randInt(0, teamIdCpy.size() - 1);
				auto teamId = teamIdCpy[pos];
				teamIdCpy.erase(teamIdCpy.begin() + pos);
				spawns->assignTeam(spawn.getId(), teamId);

				EntityId capturePoint = spawn.getId();
				createZone(capturePoint, spawn.getSpawnZone(), teamId, 13 * 120, 13 * 120);

				if (teamIdCpy.empty())
					teamIdCpy = teamIds;
			}
			else {
				EntityId capturePoint = spawn.getId();
				createZone(capturePoint, spawn.getSpawnZone(), 0, 13 * 120, 13 * 120);
			}
		}
	}
}

void DominationMode::createZone(EntityId capturePoint, AABB zone, uint64_t teamId, float totalCaptureTime_, float remainingCaptureTime) {
	EntitySystem::MakeComps<CapturePointComponent>(1, &capturePoint);
	CapturePointComponent* capture = EntitySystem::GetComp<CapturePointComponent>(capturePoint);
	capture->zone = zone;
	capture->currTeamId = teamId;
	capture->targetTeamid = teamId;
	capture->totalCaptureTime = totalCaptureTime_;
	capture->remainingCaptureTime = remainingCaptureTime;
}

void DominationMode::tickCapturePoints(SpawnSystem& spawns, double timeDelta) {
	if (restartDelay == restartDelayMax) {
		if (EntitySystem::Contains<CapturePointComponent>()) {
			for (auto& capturePoint : EntitySystem::GetPool<CapturePointComponent>()) {
				bool wasCapped = capturePoint.isCaptured();
				capturePoint.tickCapturePoint(timeDelta);
				if (capturePoint.isCaptured()) {
					if (capturePoint.currTeamId != 0)
						teams[capturePoint.currTeamId].points += pointsPerCap;
					//if we weren't capped, but now we are
					if (!wasCapped) {
						//propogate change to spawn
						spawns.assignTeam(capturePoint.getId(), capturePoint.currTeamId);
					}
				}
			}
		}
		//tick all the points
		//increment current team points
		//if any changed, propogate that change to the spawn

		for (auto& pair : teams) {
			Team& team = pair.second;
			if (team.points >= winningPoints) {
				DebugIO::printLine("Team " + std::to_string(team.teamId) + " has won!");
				restartDelay = 0;
				break;
			}
		}

		for (auto& pair : teams) {
			if (pair.second.teamId != 0)
				DebugIO::printLine("Team " + std::to_string(pair.second.teamId) + " has " + std::to_string(pair.second.points) + " points!");
		}
	}
	else {
		++restartDelay;
		if (restartDelay == restartDelayMax) {
			reset(&spawns);
		}
	}
}

void DominationMode::addPlayer(EntityId id) {
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);
	unsigned int targetTeamId = 1;
	int minSize = -1;
	for (auto& pair : teams) {
		if (pair.second.players.size() < minSize || minSize == -1) {
			targetTeamId = pair.second.teamId;
			minSize = pair.second.players.size();
		}
	}

	combat->teamId = targetTeamId;
	teams[targetTeamId].players.push_back(id);
}

void DominationMode::removePlayer(EntityId id) {
	for (auto& pair : teams) {
		for (auto iter = pair.second.players.begin(); iter != pair.second.players.end(); ++iter) {
			if (*iter == id) {
				pair.second.players.erase(iter);
				return;
			}
		}
	}
}

void DominationMode::clearPoints() {
	for (auto& team : teams) {
		team.second.points = 0;
	}
}

bool DominationMode::isRestarting() {
	return restartDelay != restartDelayMax;
}

unsigned int DominationMode::getWinningTeam() {
	unsigned int winningTeam = 0;
	int highestPoints = 0;
	for (auto& pair : teams) {
		if (pair.second.points > highestPoints) {
			highestPoints = pair.second.points;
			winningTeam = pair.second.teamId;
		}
	}
	return winningTeam;
}

/*
assuming there are 5 capture points on a map
you get 120 * pointsPerCap = 120 points a second
times 5 points = 120 * 5 = 600 points per second
times 60 seconds times 4 minutes = 600 * 60 * 4 = 144000 points 
*/