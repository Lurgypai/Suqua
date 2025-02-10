#include "PHClientState.h"
#include "Game.h"
#include "NetworkDataComponent.h"

using UUID = Suqua::UUID;

PHClientState::PHClientState(PacketId id_, Scene* scene_) :
	PacketHandler{ id_ },
    scene{scene_}
{}

void PHClientState::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId packetId;
    data >> packetId;

    bool propogate; // unused, but kept for consistency.
    data >> propogate;

    UUID uuid;
    while(data.hasMoreData()) {
        data >> uuid;

        EntityId id = NetworkDataComponent::GetEntityId(uuid);
        if(id == 0) {
            NetworkDataComponent::MoveStreamPast(data);
            continue;
        }

        NetworkDataComponent* ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
        if(ndc) ndc->unserialize(data);
        else NetworkDataComponent::MoveStreamPast(data);
    }
}
