#include "SyncSystem.h"
#include "Game.h"
#include "Packet.h"
#include "TestScene.h"
#include "TestScene2.h"
#include <iostream>

void TestNoData() {
    Game game{1.0 / 120, 1.0 / 60, 1.0 / 30, Game::Flag::client_flags}; 
   
    game.setGameTick(10);
    game.sync.storeCurrentState(game.getGameTick());
    
    ByteStream packet;
    game.sync.writeStatePacket(packet, game.getGameTick());
    
    game.physicsUpdate();

    PacketHandlerId pack;
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
    Game game{1.0 / 120, 1.0 / 60, 1.0 / 30, Game::Flag::client_flags}; 
    game.loadScene<TestScene>(Scene::Flag::physics);
    printNDCs();
    //store as state 1
    game.sync.storeCurrentState(1);
    //write as state 1
    ByteStream packet;
    game.sync.writeStatePacket(packet, 1);
    //update and replace state 1
    game.physicsUpdate();
    game.sync.overrideState(1);
    printNDCs();
    //resync
    PacketHandlerId pack;
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

void TestDataReUpdate() {
    Game game{1.0 / 120, 1.0 / 60, 1.0 / 30, Game::Flag::client_flags}; 
    game.loadScene<TestScene2>(Scene::Flag::physics);
    game.setGameTick(0);
    //store as state current state 
    //time 0 - val 0
    game.sync.storeCurrentState(game.getGameTick());
    game.sync.storeCurrentState(1);
    //write as current state 
    ByteStream packet;
    game.sync.writeStatePacket(packet, 1);
    //run and store 3 updates, (states 
    game.physicsUpdate();
    game.sync.overrideState(1);
    game.physicsUpdate();
    game.sync.storeCurrentState(game.getGameTick());
    game.physicsUpdate();
    game.sync.storeCurrentState(game.getGameTick());

    std::cout << "States before update:\n";
    printSyncSystemStates(game.sync);

    //resync
    PacketHandlerId pack;
    packet >> pack;
    game.sync.resyncStatePacket(packet, game);
    std::cout << "States after resync:\n";
    printSyncSystemStates(game.sync);
}

int main() {
    TestNoData();
    TestData();
    TestDataReUpdate();
    //repredict state call
    return 0;
}
