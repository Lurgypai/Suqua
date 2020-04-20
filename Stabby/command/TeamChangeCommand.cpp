
#include "TeamChangeCommand.h"
#include "network.h"
#include <DebugIO.h>

#include "../player/ClientPlayerComponent.h"
#include "../player/OnlinePlayerLC.h"

TeamChangeCommand::TeamChangeCommand(Client* client_) :
	client{ client_ }
{}

std::string TeamChangeCommand::getTag() const {
	return "team";
}

void TeamChangeCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		if (client->getConnected()) {
			for (auto& clientPlayer : EntitySystem::GetPool<ClientPlayerComponent>()) {
				OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(clientPlayer.getid());
				uint64_t newTeam = std::stoi(args[1]);
				TeamChangePacket p;
				p.id = online->getNetId();
				p.targetTeamId = newTeam;

				client->send(p);
				DebugIO::printLine("Requested team change to: " + std::to_string(newTeam));
			}
		}
		else {
			for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
				uint64_t newTeam = std::stoi(args[1]);
				EntitySystem::GetComp<CombatComponent>(player.getId())->teamId = newTeam;
			}
		}
	}
}
