#include "SyncSystem.h"
#include "Game.h"
#include "Packet.h"
#include "TestScene.h"
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
}

int main() {
    TestNoData();
    TestData();
    //repredict state call
    return 0;
}
