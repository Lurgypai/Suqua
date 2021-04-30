#include <iostream>
#include <string>

#include "EntitySystem.h"
#include "SyncSystem.h"
#include "OnlineComponent.h"
#include "OnlineSystem.h"

void TestNoNdc() {
    OnlineSystem online;
    ByteStream bytes;
    SyncState stateA{5};
    SyncState stateB{0};

    std::cout << "a gametime before serialize: " << stateA.getGameTime() << '\n';
    std::cout << "b gametime before serialize: " << stateB.getGameTime() << '\n';
    
    std::cout << "Bytes before serialize: " << bytes.getData() << '\n';
    stateA.serialize(bytes);
    std::cout << "Bytes after serialize: " << bytes.getData() << '\n';

    stateB.unserialize(bytes, online);
    std::cout << "a gametime after serialize: " << stateA.getGameTime() << '\n';
    std::cout << "b gametime after serialize: " << stateB.getGameTime() << '\n';

}

void TestOneNdc() {
    OnlineSystem online;
    ByteStream bytes;

    EntityId ndcA;
    EntitySystem::GenEntities(1, &ndcA);
    EntitySystem::MakeComps<NetworkDataComponent>(1, &ndcA);
    online.addOnlineComponent(ndcA);
    EntitySystem::GetComp<NetworkDataComponent>(ndcA)->set<std::string>(0, "hello");
    EntitySystem::GetComp<NetworkDataComponent>(ndcA)->set<uint64_t>(1, 5);

    SyncState stateA{5};
    std::cout << "a gametime before serialize: " << stateA.getGameTime() << '\n';
    std::cout << "ndc:\n";
    for(auto&& pair : stateA.getStates()) {
        std::cout << "Entity " << pair.first
            << ", 0: " << pair.second.data.getConst<std::string>(0)  
            << ", 1: " << pair.second.data.getConst<uint64_t>(1) << '\n';
    }

    EntitySystem::GetComp<NetworkDataComponent>(ndcA)->set<std::string>(0, "hi");
    EntitySystem::GetComp<NetworkDataComponent>(ndcA)->set<uint64_t>(1, 0);
    SyncState stateB{0};
    std::cout << "b gametime before serialize: " << stateB.getGameTime() << '\n';
    std::cout << "ndc:\n";
    for(auto&& pair : stateB.getStates()) {
        std::cout << "Entity " << pair.first
            << ", 0: " << pair.second.data.getConst<std::string>(0)  
            << ", 1: " << pair.second.data.getConst<uint64_t>(1) << '\n';
    }
    
    stateA.serialize(bytes);

    stateB.unserialize(bytes, online);
    std::cout << "a gametime after serialize: " << stateA.getGameTime() << '\n';
    std::cout << "ndc:\n";
    for(auto&& pair : stateA.getStates()) {
        std::cout << "Entity " << pair.first
            << ", 0: " << pair.second.data.getConst<std::string>(0)  
            << ", 1: " << pair.second.data.getConst<uint64_t>(1) << '\n';
    }
    std::cout << "b gametime after serialize: " << stateB.getGameTime() << '\n';
    std::cout << "ndc:\n";
    for(auto&& pair : stateB.getStates()) {
        std::cout << "Entity " << pair.first
            << ", 0: " << pair.second.data.getConst<std::string>(0)  
            << ", 1: " << pair.second.data.getConst<uint64_t>(1) << '\n';
    }


}

void TestOneNdcAndCC() {
    OnlineSystem online;
    ByteStream bytes;

    EntityId ndcA;
    EntitySystem::GenEntities(1, &ndcA);
    EntitySystem::MakeComps<NetworkDataComponent>(1, &ndcA);
    online.addOnlineComponent(ndcA);
    EntitySystem::GetComp<NetworkDataComponent>(ndcA)->set<std::string>(0, "hello");
    EntitySystem::GetComp<NetworkDataComponent>(ndcA)->set<uint64_t>(1, 5);
    EntitySystem::MakeComps<ControllerComponent>(1, &ndcA);
    EntitySystem::GetComp<ControllerComponent>(ndcA)->getController().on(ControllerBits::ALL);
    EntitySystem::GetComp<ControllerComponent>(ndcA)->getController().mouseScroll = 5;


    SyncState stateA{5};
    std::cout << "a gametime before serialize: " << stateA.getGameTime() << '\n';
    std::cout << "ndc:\n";
    for(auto&& pair : stateA.getStates()) {
        std::cout << "Entity " << pair.first
            << ", 0: " << pair.second.data.getConst<std::string>(0)  
            << ", 1: " << pair.second.data.getConst<uint64_t>(1) << '\n';

        std::cout << "Controller Bits: "
            << pair.second.cont->getControllerConst().getState()
            << ", mouseScroll: "
            << pair.second.cont->getControllerConst().mouseScroll << '\n';
    }

    EntitySystem::GetComp<NetworkDataComponent>(ndcA)->set<std::string>(0, "hi");
    EntitySystem::GetComp<NetworkDataComponent>(ndcA)->set<uint64_t>(1, 0);
    EntitySystem::GetComp<ControllerComponent>(ndcA)->getController().off(ControllerBits::ALL);
    EntitySystem::GetComp<ControllerComponent>(ndcA)->getController().mouseScroll = 10;
    SyncState stateB{0};
    std::cout << "b gametime before serialize: " << stateB.getGameTime() << '\n';
    std::cout << "ndc:\n";
    for(auto&& pair : stateB.getStates()) {
        std::cout << "Entity " << pair.first
            << ", 0: " << pair.second.data.getConst<std::string>(0)  
            << ", 1: " << pair.second.data.getConst<uint64_t>(1) << '\n';

        std::cout << "Controller Bits: "
            << pair.second.cont->getControllerConst().getState()
            << ", mouseScroll: "
            << pair.second.cont->getControllerConst().mouseScroll << '\n';
    }
    
    stateA.serialize(bytes);

    stateB.unserialize(bytes, online);
    std::cout << "a gametime after serialize: " << stateA.getGameTime() << '\n';
    std::cout << "ndc:\n";
    for(auto&& pair : stateA.getStates()) {
        std::cout << "Entity " << pair.first
            << ", 0: " << pair.second.data.getConst<std::string>(0)  
            << ", 1: " << pair.second.data.getConst<uint64_t>(1) << '\n';

        std::cout << "Controller Bits: "
            << pair.second.cont->getControllerConst().getState()
            << ", mouseScroll: "
            << pair.second.cont->getControllerConst().mouseScroll << '\n';
    }
    std::cout << "b gametime after serialize: " << stateB.getGameTime() << '\n';
    std::cout << "ndc:\n";
    for(auto&& pair : stateB.getStates()) {
        std::cout << "Entity " << pair.first
            << ", 0: " << pair.second.data.getConst<std::string>(0)  
            << ", 1: " << pair.second.data.getConst<uint64_t>(1) << '\n';

        std::cout << "Controller Bits: "
            << pair.second.cont->getControllerConst().getState()
            << ", mouseScroll: "
            << pair.second.cont->getControllerConst().mouseScroll << '\n';
    }


}

int main() {
    //no ndcs
//    TestNoNdc(); 
    //one ndc
//    TestOneNdc();
    //ndc + cc
    TestOneNdcAndCC();

    //two ndcs

    //one ndc with cont

    //two ndcs with cont
    return 0;
}
