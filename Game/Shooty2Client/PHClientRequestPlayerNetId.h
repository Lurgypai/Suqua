#pragma once
#include "PacketHandler.h"
#include "Scene.h"

class PHClientRequestPlayerNetId : public PacketHandler {
public:
	PHClientRequestPlayerNetId(PacketId id_, EntityId playerId_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
    EntityId playerId;
};
