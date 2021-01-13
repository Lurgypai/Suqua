#pragma once
#include <vector>
#include <unordered_map>

#include "../stage/Stage.h"
#include "../combat/CombatComponent.h"

struct Team {
	std::vector<EntityId> players;
	unsigned int teamId;
	unsigned int points;
};

class DominationMode {
public:
	void load(SpawnSystem* spawns, unsigned int totalTeams_, unsigned int pointsPerCap_, unsigned int winningPoints_);
	void reset(SpawnSystem* spawns);
	void loadTeams();
	void loadData(unsigned int totalTeams_, unsigned int pointsPerCap_, unsigned int winningPoints_);
	void assignPlayers();
	void assignSpawns(SpawnSystem* spawns);
	void createZone(EntityId capturePoint, AABB zone, uint64_t teamId, float totalCaptureTime_, float remainingCaptureTime);
	void tickCapturePoints(SpawnSystem& spawns, double timeDelta);
	void tickRestart(SpawnSystem* spawns);
	bool restarting();
	void addPlayer(EntityId id);
	void removePlayer(EntityId id);
	void clearPoints();
	bool isRestarting();
	unsigned int getPoints(unsigned int team) const;
	unsigned int getWinningTeam() const;
	unsigned int getWinningPoints() const;
	unsigned int getWinner() const;

	unsigned int getPointsPerCap() const;
	unsigned int getTeamCount() const;

	void setPoints(unsigned int team, unsigned int points);

	const std::unordered_map<unsigned int, Team>& getTeams() const;
private:
	std::unordered_map<unsigned int, Team> teams;
	unsigned int winningPoints;
	unsigned int pointsPerCap;
	unsigned int totalTeams;

	unsigned int restartDelay;
	unsigned int restartDelayMax;
};