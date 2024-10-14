#pragma once
#include "PacketHandler.h"

class PHServerSpawnEntities : public PacketHandler {
public:
	PHServerSpawnEntities(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};
