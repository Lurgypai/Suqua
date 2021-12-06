#pragma once
#include "ByteStream.h"
#include "PeerId.h"
#include <memory>

using PacketId = uint32_t;

class Game;

class PacketHandler {
public:
	PacketHandler(PacketId id_);
	virtual ~PacketHandler() = 0;
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) = 0;
	PacketId getId() const;
private:
	PacketId id;
};

using PacketHandlerPtr = std::unique_ptr<PacketHandler>;
