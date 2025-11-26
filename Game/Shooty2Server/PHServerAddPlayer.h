#pragma once
#include "PacketHandler.h"
#include "Scene.h"

class PHServerAddPlayer : public PacketHandler {
public:
	PHServerAddPlayer(PacketId id_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
};
