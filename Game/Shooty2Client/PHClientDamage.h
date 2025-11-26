
#pragma once
#include "PacketHandler.h"
#include "Scene.h"
#include "World.h"

class PHClientDamage : public PacketHandler {
public:
	PHClientDamage(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
};
