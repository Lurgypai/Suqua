#include "SyncSystem.h"
#include "Game.h"
#include "Packet.h"
#include "TestScene.h"
#include "TestScene2.h"
#include "TestScene3.h"
#include <iostream>

void TestNoData() {
    Game game{ Game::Flag::physics}; 
   
    game.setGameTick(10);
    game.sync.storeCurrentState(game.getGameTick());
    
    ByteStream packet;
    game.sync.writeStatePacket(packet, game.getGameTick());
    
    game.physicsUpdate();
    game.tickTime();

    PacketId pack;
    packet >> pack;
    game.sync.resyncStatePacket(packet, game);
    //store current state
    //write state packet
    //resync state packet
}

void printNDCs() {
    for(auto&& data : EntitySystem::GetPool<NetworkDataComponent>()) {
        std::cout << "Data at 0: " << data.get<int32_t>(0) << '\n';
    }
}
void TestData() {
    Game game{Game::Flag::physics };
    game.loadScene<TestScene>(Scene::Flag::physics);
    printNDCs();
    //store as state 1
    game.sync.storeCurrentState(1);
    //write as state 1
    ByteStream packet;
    game.sync.writeStatePacket(packet, 1);
    //update and replace state 1
    game.physicsUpdate();
    game.tickTime();
    game.sync.overrideState(1);
    printNDCs();
    //resync
    PacketId pack;
    packet >> pack;
    game.sync.resyncStatePacket(packet, game);
    printNDCs();
    game.close();
}

void printSyncSystemStates(const SyncSystem& sync) {
    for(auto&& state : sync.getStates()) {
        std::cout << "States at time: " << state.first << '\n';
        for(auto&& entityState : state.second.getStates()) {
            std::cout << "\tEntity: " << entityState.first << ", Data: ";
            std::cout << entityState.second.data.getConst<int32_t>(0) << '\n';
        }
    }
}

void TestInputReUpdate() {
    Game game{ Game::Flag::physics };
    auto sceneId = game.loadScene<TestScene3>(Scene::Flag::physics);
    EntityId entity = game.getScene<TestScene3>(sceneId).entity;
    game.setGameTick(0);

    //generate an alternate start state
    EntitySystem::GetComp<NetworkDataComponent>(entity)->set<int32_t>(0, 5);
    game.sync.storeCurrentState(0);
    ByteStream packet;
    game.sync.writeStatePacket(packet, 0);


    //replace the start state and simulate
    EntitySystem::GetComp<NetworkDataComponent>(entity)->set<int32_t>(0, 0);
    game.sync.overrideState(0);
    game.tickTime();
    game.physicsUpdate();
    game.sync.storeCurrentState(game.getGameTick());
    game.tickTime();
    game.physicsUpdate();
    game.sync.storeCurrentState(game.getGameTick());

    game.tickTime();
    EntitySystem::GetComp<ControllerComponent>(entity)->getController().on(ControllerBits::BUTTON_1);
    game.physicsUpdate();
    game.sync.storeCurrentState(game.getGameTick());
    game.tickTime();
    game.physicsUpdate();
    game.sync.storeCurrentState(game.getGameTick());
    game.tickTime();
    game.physicsUpdate();
    game.sync.storeCurrentState(game.getGameTick());

    game.tickTime();
    game.physicsUpdate();

    std::cout << "States before update:\n";
    printSyncSystemStates(game.sync);
    std::cout << "Final state before update: " << EntitySystem::GetComp<NetworkDataComponent>(entity)->get<int32_t>(0) << '\n';
    std::cout << "Time before update: " << game.getGameTick() << '\n';

    //resync
    PacketId pack;
    packet >> pack;
    game.sync.resyncStatePacket(packet, game);
    std::cout << "States after resync:\n";
    printSyncSystemStates(game.sync);
    std::cout << "Final state after update: " << EntitySystem::GetComp<NetworkDataComponent>(entity)->get<int32_t>(0) << '\n';
    std::cout << "Time after update: " << game.getGameTick() << '\n';
}

int main(int argv, char** argc) {
    //TestNoData();
    //TestData();
    //TestData();
    TestInputReUpdate();
    //repredict state call
    return 0;
}
