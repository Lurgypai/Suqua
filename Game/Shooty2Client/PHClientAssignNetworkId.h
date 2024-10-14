#pragma once
#include "PacketHandler.h"
#include "Scene.h"

class PHClientAssignNetworkId : public PacketHandler {
public:
	PHClientAssignNetworkId(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
};
