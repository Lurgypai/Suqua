#include "TestScene2.h"
#include "NetworkDataComponent.h"
#include "Game.h"

TestScene2::TestScene2(SceneId id_, FlagType flags_) :
    Scene(id_, flags_)
{}

TestScene2::~TestScene2() {};

void TestScene2::load(Game& game) {
    auto entities = addEntities(2);
    EntitySystem::MakeComps<NetworkDataComponent>(2, entities.data());
    EntitySystem::GetComp<NetworkDataComponent>(entities[0])->set<int32_t>(0, 0);
    EntitySystem::GetComp<NetworkDataComponent>(entities[1])->set<int32_t>(0, 5);
    
    game.online.addOnlineComponent(entities[0]);
    game.online.addOnlineComponent(entities[1]);
}

void TestScene2::prePhysicsStep(Game& game) {
}

void TestScene2::physicsStep(Game& game) {
    for(auto&& data : EntitySystem::GetPool<NetworkDataComponent>()) {
        std::cout << "Updating entity " << data.getId() << " from " << data.get<int32_t>(0) << " to " << data.get<int32_t>(0) + 1 << '\n';
        ++data.get<int32_t>(0);
    }
}

void TestScene2::postPhysicsStep(Game& game) {
}

void TestScene2::preRenderStep(Game& game) {
}

void TestScene2::renderStep(Game& game) {
}

void TestScene2::postRenderStep(Game& game) {
}

void TestScene2::unload(Game& game) {
}







