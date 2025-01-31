#include "CommandRespawn.h"
#include "PlayerSpawnComponent.h"
#include "PhysicsComponent.h"

CommandRespawn::CommandRespawn(World& world_) : 
    world{ &world_ }
{ }

std::string CommandRespawn::getTag() const {
    return "respawn";
}

void CommandRespawn::onCommand(const std::vector<std::string>& args) {
    auto levelId = args[1];

    bool found = false;
    for(auto& level : world->getLevels()) {
        if(level.first == levelId) {
            found = true;
            break;
        }
    }

    if(!found) {
        std::cout << "Unable to locate level \"" << levelId << "\"\n";
        return;
    }

    for(auto& playerSpawn : EntitySystem::GetPool<PlayerSpawnComponent>()) {
        auto* physics = EntitySystem::GetComp<PhysicsComponent>(playerSpawn.getId());

        auto curLevel = world->getActiveLevel(physics->position());
        if(curLevel != nullptr) curLevel->deactivate();
        physics->teleport(playerSpawn.getSpawnPos(levelId));
        curLevel = world->getActiveLevel(physics->position());
        if(curLevel != nullptr) curLevel->activate();
    }
}
