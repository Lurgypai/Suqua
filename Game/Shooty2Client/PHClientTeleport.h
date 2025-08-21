#pragma once
#include "PacketHandler.h"
#include "Scene.h"
#include "World.h"

class PHClientTeleport : public PacketHandler {
public:
	PHClientTeleport(PacketId id_, EntityId& playerId_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
    EntityId& playerId;
};
