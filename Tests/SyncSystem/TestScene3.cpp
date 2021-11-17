#include "TestScene3.h"
#include "NetworkDataComponent.h"
#include "ControllerComponent.h"
#include "Game.h"

TestScene3::TestScene3(SceneId id_, FlagType flags_) :
    Scene(id_, flags_)
{}

TestScene3::~TestScene3() {};

void TestScene3::load(Game& game) {
    auto entities = addEntities(2);
    EntitySystem::MakeComps<NetworkDataComponent>(1, entities.data());
    EntitySystem::GetComp<NetworkDataComponent>(entities[0])->set<int32_t>(0, 0);
    EntitySystem::MakeComps<ControllerComponent>(1, entities.data());
    
    game.online.addOnlineComponent(entities[0]);
    entity = entities[0];
}

void TestScene3::prePhysicsStep(Game& game) {
}

void TestScene3::physicsStep(Game& game) {
    for(auto&& data : EntitySystem::GetPool<NetworkDataComponent>()) {
        int prevValue = data.get<int32_t>(0);
        if (EntitySystem::GetComp<ControllerComponent>(data.getId())->getController()[ControllerBits::BUTTON_1]) {
            ++data.get<int32_t>(0);
        }
        std::cout << "Updating entity " << data.getId() << " from " << prevValue << " to " << data.get<int32_t>(0) << '\n';
        std::cout << "Controller state " << EntitySystem::GetComp<ControllerComponent>(data.getId())->getController().getState() << " at time " << game.getGameTick() << '\n';
    }
}

void TestScene3::postPhysicsStep(Game& game) {
}

void TestScene3::preRenderStep(Game& game) {
}

void TestScene3::renderStep(Game& game) {
}

void TestScene3::postRenderStep(Game& game) {
}

void TestScene3::unload(Game& game) {
}

void TestScene3::onConnect(Game& game, PeerId connectingId)
{
}







