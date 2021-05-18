#include "TestScene.h"
#include "NetworkDataComponent.h"
#include "Game.h"

TestScene::TestScene(SceneId id_, FlagType flags_) :
    Scene(id_, flags_)
{}

TestScene::~TestScene() {};

void TestScene::load(Game& game) {
    EntityId id;
    EntitySystem::GenEntities(1, &id);
    EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
    EntitySystem::GetComp<NetworkDataComponent>(id)->set<int32_t>(0, 0);
    
    game.online.addOnlineComponent(id);
}

void TestScene::prePhysicsStep(Game& game) {
}

void TestScene::physicsStep(Game& game) {
    for(auto&& data : EntitySystem::GetPool<NetworkDataComponent>()) {
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







