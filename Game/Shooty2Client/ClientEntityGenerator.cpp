#include "ClientEntityGenerator.h"
#include "CharacterGFXComponent.h"
#include "RespawnGFXComponent.h"
#include "GunGFXComponent.h"
#include "RectDrawable.h"
#include "../Shooty2Core/OnHitComponent.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/EntityGenerator.h"
#include <vector>

ClientEntityGenerator::ClientEntityGenerator(Host* host_) :
    host{host_},
    GFXFunctions{}
{}

static void AddPlayerGFX(const std::vector<EntityId>& entities) {
    auto playerId = entities[0];
	EntitySystem::MakeComps<CharacterGFXComponent>(1, &playerId);
	EntitySystem::GetComp<CharacterGFXComponent>(playerId)->loadSpriteSheet("hero", "stranded/Hero/Hero/Hero.json", Vec2f{ -13, -24 });
	EntitySystem::GetComp<CharacterGFXComponent>(playerId)->setHasUpDown(true);
	EntitySystem::MakeComps<OnHitComponent>(1, &playerId);
    EntitySystem::MakeComps<RespawnGFXComponent>(1, &playerId);

    auto gunId = entities[1];
	EntitySystem::MakeComps<GunGFXComponent>(1, &gunId);
}

static void AddBulletGFX(const std::vector<EntityId>& entities) {
    EntityId bulletId = entities[0];

	EntitySystem::MakeComps<RenderComponent>(1, &bulletId);
	auto render = EntitySystem::GetComp<RenderComponent>(bulletId);
	render->loadDrawable<RectDrawable>(RectDrawable{ Color{0.0, 1.0, 0.0, 1.0}, true, -0.1, AABB{{0, 0 }, {4, 4}} });
}

static void AddEnemyGFX(const std::vector<EntityId>& entities) {
}

void ClientEntityGenerator::RegisterSpawnFunctions() {
    EntityGenerator::RegisterSpawnFunctions();
    
    ClientEntityGenerator::GFXFunctions.insert(std::make_pair("player.basic", AddPlayerGFX));
    ClientEntityGenerator::GFXFunctions.insert(std::make_pair("enemy.basic", AddEnemyGFX));
    ClientEntityGenerator::GFXFunctions.insert(std::make_pair("bullet.basic", AddBulletGFX));
}

std::vector<EntityId> ClientEntityGenerator::SpawnEntity(const std::string& tag, Scene& targetScene,
        const Vec2f& pos, NetworkOwnerComponent::Owner owner, bool shared) {
    auto entities = EntityGenerator::SpawnEntity(tag, targetScene, pos, owner, shared);
    GFXFunctions.at(tag)(entities);
    if(!shared) return entities;

    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;
    spawn << tag;
    spawn << entities[0];
    spawn << pos;
    std::uint32_t count = entities.size() - 1;
    spawn << count;
    for(int i = 1; i != entities.size(); ++i) {
        spawn << entities[i];
    }
    host->bufferAllDataByChannel(0, spawn);

    return entities;
}
