#include "ClientEntityGenerator.h"
#include "CharacterGFXComponent.h"
#include "RespawnGFXComponent.h"
#include "GunGFXComponent.h"
#include "RectDrawable.h"
#include "AttackGFXComponent.h"
#include "DebugFIO.h"
#include "../Shooty2Core/OnHitComponent.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/EntityGenerator.h"
#include <cstdint>
#include <vector>

ClientEntityGenerator::ClientEntityGenerator(Host* host_) :
    host{host_},
    GFXFunctions{}
{}

static void AddPlayerGFX(const std::vector<EntityId>& entities) {
    auto playerId = entities[0];
	EntitySystem::MakeComps<CharacterGFXComponent>(1, &playerId,
            "hero",
            "stranded/Hero/Hero/Hero.json",
            Vec2f{ -13, -24 } );

	EntitySystem::GetComp<CharacterGFXComponent>(playerId)->setHasUpDown(true);
	EntitySystem::MakeComps<OnHitComponent>(1, &playerId);
    EntitySystem::MakeComps<RespawnGFXComponent>(1, &playerId);

    auto gunId = entities[1];
	EntitySystem::MakeComps<GunGFXComponent>(1, &gunId);
}

static void AddBulletPlayerBasicGFX(const std::vector<EntityId>& entities) {
    EntityId bulletId = entities[0];

	EntitySystem::MakeComps<RenderComponent>(1, &bulletId);
	auto render = EntitySystem::GetComp<RenderComponent>(bulletId);
	render->loadDrawable<RectDrawable>(RectDrawable{ Color{0.0, 1.0, 0.0, 1.0}, true, -0.1, AABB{{0, 0 }, {4, 4}} });
}

static void AddEnemyGFX(const std::vector<EntityId>& entities) {
    auto enemyId = entities[0];
	EntitySystem::MakeComps<CharacterGFXComponent>(1, &enemyId,
            "enemy:basic",
            "enemy/basic.json",
            Vec2f{ -13, -24 } );

    EntitySystem::MakeComps<OnHitComponent>(1, &enemyId);
    EntitySystem::MakeComps<RespawnGFXComponent>(1, &enemyId);

    EntitySystem::MakeComps<AttackGFXComponent>(1, &enemyId, 80);
}

static void AddBulletEnemyBasicGFX(const std::vector<EntityId>& entities) {
    EntityId bulletId = entities[0];

	EntitySystem::MakeComps<RenderComponent>(1, &bulletId);
	auto render = EntitySystem::GetComp<RenderComponent>(bulletId);
	render->loadDrawable<RectDrawable>(RectDrawable{ Color{0.675, 0.196, 0.196, 1.0}, true, -0.1, AABB{{0, 0 }, {4, 4}} });
}

void ClientEntityGenerator::RegisterSpawnFunctions() {
    EntityGenerator::RegisterSpawnFunctions();
    
    ClientEntityGenerator::GFXFunctions.insert(std::make_pair("player.basic", AddPlayerGFX));
    ClientEntityGenerator::GFXFunctions.insert(std::make_pair("enemy.basic", AddEnemyGFX));
    ClientEntityGenerator::GFXFunctions.insert(std::make_pair("bullet.player.basic", AddBulletPlayerBasicGFX));
    ClientEntityGenerator::GFXFunctions.insert(std::make_pair("bullet.enemy.basic", AddBulletEnemyBasicGFX));
}

std::vector<EntityId> ClientEntityGenerator::SpawnEntity(
        const std::string& tag, Scene& targetScene,
        const Vec2f& pos, NetworkDataComponent::Owner owner,
        const std::vector<UUID>& uuids) {
    auto entities = EntityGenerator::SpawnEntity(tag, targetScene, pos, owner, uuids);
    GFXFunctions.at(tag)(entities);
    if(!host->isConnected() || owner != NetworkDataComponent::Owner::local_shared) return entities;

    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;
    spawn << tag;
    spawn << pos;
    spawn << static_cast<std::uint32_t>(entities.size());
    for(auto& entity : entities) {
        auto ndc = EntitySystem::GetComp<NetworkDataComponent>(entity);
        spawn << ndc->getUUID();
        DebugFIO::TimeOut("send.packet.log") << entity << " spawn\n";
    }
    host->bufferAllDataByChannel(0, spawn);

    return entities;
}
