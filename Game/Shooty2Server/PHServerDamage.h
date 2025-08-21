
#pragma once
#include "PacketHandler.h"
#include "Scene.h"
#include "World.h"

class PHServerDamage : public PacketHandler {
public:
	PHServerDamage(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
};
