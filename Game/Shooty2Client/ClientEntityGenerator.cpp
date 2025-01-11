#include "ClientEntityGenerator.h"
#include "CharacterGFXComponent.h"
#include "RespawnGFXComponent.h"
#include "GunGFXComponent.h"
#include "RectDrawable.h"
#include "AttackGFXComponent.h"
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

static void AddBulletPlayerBasicGFX(const std::vector<EntityId>& entities) {
    EntityId bulletId = entities[0];

	EntitySystem::MakeComps<RenderComponent>(1, &bulletId);
	auto render = EntitySystem::GetComp<RenderComponent>(bulletId);
	render->loadDrawable<RectDrawable>(RectDrawable{ Color{0.0, 1.0, 0.0, 1.0}, true, -0.1, AABB{{0, 0 }, {4, 4}} });
}

static void AddEnemyGFX(const std::vector<EntityId>& entities) {
    auto enemyId = entities[0];
	EntitySystem::MakeComps<CharacterGFXComponent>(1, &enemyId);
	EntitySystem::GetComp<CharacterGFXComponent>(enemyId)->loadSpriteSheet(
            "enemy:basic", "enemy/basic.json", Vec2f{ -13, -24 });
    EntitySystem::MakeComps<OnHitComponent>(1, &enemyId);
    EntitySystem::MakeComps<RespawnGFXComponent>(1, &enemyId);

    EntitySystem::MakeComps<AttackGFXComponent>(1, &enemyId);
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

std::vector<EntityId> ClientEntityGenerator::SpawnEntity(const std::string& tag, Scene& targetScene,
        const Vec2f& pos, NetworkOwnerComponent::Owner owner, bool shared) {
    auto entities = EntityGenerator::SpawnEntity(tag, targetScene, pos, owner, shared);
    GFXFunctions.at(tag)(entities);
    if(!shared) return entities;

    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;
    spawn << tag;
    spawn << pos;
    for(auto& entity : entities) {
        spawn << entity;
    }
    host->bufferAllDataByChannel(0, spawn);

    return entities;
}
