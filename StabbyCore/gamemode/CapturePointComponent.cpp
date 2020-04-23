
#include "CapturePointComponent.h"
#include "../player/PlayerLC.h"
#include <DebugIO.h>

CapturePointComponent::CapturePointComponent(EntityId id_) :
	id{id_},
	state{idle},
	currTeamId{0},
	targetTeamid{0}
{}

EntityId CapturePointComponent::getId() const {
	return id;
}

void CapturePointComponent::tickCapturePoint(double timeDelta) {

	std::list<EntityId> players;
	bool multipleTeams = false;
	//0 is the neutral team, fix
	unsigned int prevPlayerTeam = 0;
	if (EntitySystem::Contains<PlayerLC>()) {
		for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
			PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(player.getId());
			if (zone.contains(physics->getPos())) {
				CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(player.getId());
				if (combat->isAlive()) {
					players.push_back(player.getId());
					if (prevPlayerTeam != 0 && prevPlayerTeam != combat->teamId) {
						multipleTeams = true;
					}
					prevPlayerTeam = combat->teamId;
				}
			}
		}
	}

	switch (state) {
	case idle:
		//DebugIO::printLine(std::to_string(id) + " Idle.");
		if (!players.empty()) {
			if (!multipleTeams) {
				CombatComponent* frontPlayer = EntitySystem::GetComp<CombatComponent>(players.front());
				targetTeamid = frontPlayer->teamId;
				if (targetTeamid != currTeamId) {
					state = capturing;
				}
			}
			else {
				state = contested;
			}
		}
		else {
			if (currTeamId != targetTeamid) {
				//refill our time required to switch teams
				if (remainingCaptureTime < totalCaptureTime) {
					remainingCaptureTime += captureSpeed;
					if (remainingCaptureTime > totalCaptureTime)
						remainingCaptureTime = totalCaptureTime;
				}
				//then reset to the team that already had it captured.
				else if (remainingCaptureTime == totalCaptureTime) {
					targetTeamid = currTeamId;
				}
			}
		}
		break;
	case capturing:
		//DebugIO::printLine(std::to_string(id) + " Capturing.");
		if (multipleTeams) {
			state = contested;
		}
		else {
			if (players.empty()) {
				state = idle;
			}
			else {
				//DebugIO::printLine(std::to_string(id) + " Capping point:" + std::to_string(remainingCaptureTime));

				CombatComponent* frontPlayer = EntitySystem::GetComp<CombatComponent>(players.front());
				targetTeamid = frontPlayer->teamId;
				//fix this, it grows linearly.
				remainingCaptureTime -= players.size() * captureSpeed;
				SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(id);
				if (remainingCaptureTime <= 0) {
					if (!spawn->isDefault()) {
						//DebugIO::printLine(std::to_string(id) + " Point capped.");
						remainingCaptureTime = totalCaptureTime;
						currTeamId = targetTeamid;
						state = idle;
					}
					else {
						//DebugIO::printLine("Unable to cap point, its a default point and right now that means you can't okay.");
					}
				}
			}
		}
		break;
	case contested:
		//DebugIO::printLine(std::to_string(id) + " Contested.");
		if (players.empty()) {
			state = idle;
		}
		else if(!multipleTeams) {
			CombatComponent* frontPlayer = EntitySystem::GetComp<CombatComponent>(players.front());
			targetTeamid = frontPlayer->teamId;
			if (targetTeamid != currTeamId) {
				state = capturing;
			}
		}
		break;
	}

}

bool CapturePointComponent::isCaptured() const {
	return currTeamId == targetTeamid;
}

AABB CapturePointComponent::getZone() const {
	return zone;
}

CapturePointComponent::StateContainer CapturePointComponent::getState() const {
	StateContainer stateContainer;
	stateContainer.state = state;
	stateContainer.remainingCaptureTime = remainingCaptureTime;
	stateContainer.totalCaptureTime = totalCaptureTime;
	stateContainer.currTeamId = currTeamId;
	stateContainer.targetTeamId = targetTeamid;
	return stateContainer;
}

void CapturePointComponent::setState(StateContainer newState) {
	state = newState.state;
	remainingCaptureTime = newState.remainingCaptureTime;
	currTeamId = newState.currTeamId;
	targetTeamid = newState.targetTeamId;
	totalCaptureTime = newState.totalCaptureTime;
}

const double CapturePointComponent::captureSpeed{ 1.0 };
const double CapturePointComponent::maxCaptureMultiplier{ 3.0 };