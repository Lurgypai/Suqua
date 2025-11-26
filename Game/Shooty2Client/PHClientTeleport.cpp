#include "PHClientTeleport.h"
#include "PhysicsComponent.h"

PHClientTeleport::PHClientTeleport(PacketId id_, EntityId& playerId_) :
    PacketHandler(id_),
    playerId{playerId_}
{}

void PHClientTeleport::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId id;
    data >> id;

    Vec2f pos;
    data >> pos;

    auto physComp = EntitySystem::GetComp<PhysicsComponent>(playerId);
    physComp->teleport(pos);
}
