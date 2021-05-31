#include "TestScene.h"
#include "NetworkDataComponent.h"
#include "Game.h"

TestScene::TestScene(SceneId id_, FlagType flags_) :
    Scene(id_, flags_)
{}

TestScene::~TestScene() {};

void TestScene::load(Game& game) {
    EntityId entity = addEntities(1)[0];
    EntitySystem::MakeComps<NetworkDataComponent>(1, &entity);
    EntitySystem::GetComp<NetworkDataComponent>(entity)->set<int32_t>(0, 0);
    
    game.online.addOnlineComponent(entity);
}

void TestScene::prePhysicsStep(Game& game) {
}

void TestScene::physicsStep(Game& game) {
    for(auto&& data : EntitySystem::GetPool<NetworkDataComponent>()) {
        std::cout << "Updating entity " << data.getId() << " from " << data.get<int32_t>(0) << " to " << data.get<int32_t>(0) + 1 << '\n';
        ++data.get<int32_t>(0);
    }
}

void TestScene::postPhysicsStep(Game& game) {
}

void TestScene::preRenderStep(Game& game) {
}

void TestScene::renderStep(Game& game) {
}

void TestScene::postRenderStep(Game& game) {
}

void TestScene::unload(Game& game) {
}







