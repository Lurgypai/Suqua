#include "SyncSystem.h"
#include "Game.h"

void TestNoData() {
    Game game{1.0 / 120, 1.0 / 60, 1.0 / 30, Game::Flag::client_flags}; 
   
    game.setGameTick(10);
    game.sync.storeCurrentState(game.getGameTick());
    
    ByteStream packet;
    game.sync.writeStatePacket(packet, game.getGameTick());
    
    game.physicsUpdate();


    game.sync.resyncStatePacket(packet, game);
    //store current state
    //write state packet
    //resync state packet
}

void TestData() {
}

int main() {
    TestNoData();
    TestData();
    return 0;
}
