#pragma once
#include "PacketHandler.h"

class PHServerDeadEntities : public PacketHandler {
public:
	PHServerDeadEntities(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};
