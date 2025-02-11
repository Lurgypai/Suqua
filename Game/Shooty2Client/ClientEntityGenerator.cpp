#include "ClientEntityGenerator.h"
#include "CharacterGFXComponent.h"
#include "RespawnGFXComponent.h"
#include "RectDrawable.h"
#include "AttackGFXComponent.h"
#include "GunGFXComponent.h"
#include "DebugFIO.h"
#include "Sprite.h"
#include "../Shooty2Core/OnHitComponent.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/EntityGenerator.h"

ClientEntityGenerator::ClientEntityGenerator(Host* host_) :
    host{host_},
    GFXFunctions{}
{}

static void AddPlayerGFX(EntityId playerId) {
	EntitySystem::MakeComps<CharacterGFXComponent>(1, &playerId,
            "hero",
            "stranded/Hero/Hero/Hero.json",
            Vec2f{ -13, -24 },
            Vec2f{ -3, -1} );

	EntitySystem::GetComp<CharacterGFXComponent>(playerId)->setHasUpDown(true);
	EntitySystem::MakeComps<OnHitComponent>(1, &playerId);
    // EntitySystem::MakeComps<RespawnGFXComponent>(1, &playerId);

	EntitySystem::MakeComps<GunGFXComponent>(1, &playerId);
}

static void AddBulletPlayerBasicGFX(EntityId bulletId) {
	EntitySystem::MakeComps<RenderComponent>(1, &bulletId);
	auto render = EntitySystem::GetComp<RenderComponent>(bulletId);
	render->loadDrawable<RectDrawable>(RectDrawable{ Color{0.0, 1.0, 0.0, 1.0}, true, -0.1, AABB{{0, 0 }, {3, 3}} });
}

static void AddEnemyGFX(EntityId enemyId) {
	EntitySystem::MakeComps<CharacterGFXComponent>(1, &enemyId,
            "enemy:basic",
            "enemy/basic.json",
            Vec2f{ -13, -24 },
            Vec2f{ -3, 5 });

    EntitySystem::MakeComps<OnHitComponent>(1, &enemyId);
    // EntitySystem::MakeComps<RespawnGFXComponent>(1, &enemyId);

    EntitySystem::MakeComps<AttackGFXComponent>(1, &enemyId, 80);
}

static void AddBulletEnemyBasicGFX(EntityId bulletId) {
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

EntityId ClientEntityGenerator::SpawnEntity(
        const std::string& tag, Scene& targetScene,
        const Vec2f& pos, NetworkDataComponent::Owner owner,
        const Suqua::UUID& uuid) {
    auto entity = EntityGenerator::SpawnEntity(tag, targetScene, pos, owner, uuid);
    GFXFunctions.at(tag)(entity);
    if(!host->isConnected() || owner != NetworkDataComponent::Owner::local_shared) return entity;

    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;
    spawn << tag;
    spawn << pos;
    spawn << uuid;
    DebugFIO::TimeOut("send.packet.log") << entity << " spawn\n";
    host->bufferAllDataByChannel(0, spawn);

    return entity;
}
