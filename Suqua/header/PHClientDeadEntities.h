#pragma once
#include "PacketHandler.h"

class PHClientDeadEntities : public PacketHandler {
public:
	PHClientDeadEntities(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};
