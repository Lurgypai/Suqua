#pragma once
#include "PacketHandler.h"

class PHBoxQuit : public PacketHandler {
public:
	PHBoxQuit(PacketId id_);
	virtual ~PHBoxQuit();
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer);
};