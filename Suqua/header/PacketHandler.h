#pragma once
#include "ByteStream.h"
#include <memory>

using PacketHandlerId = uint32_t;

class Game;

class PacketHandler {
public:
	PacketHandler(PacketHandlerId id_);
	virtual ~PacketHandler() = 0;
	virtual void handlePacket(ByteStream& data, Game& game) = 0;
	PacketHandlerId getId() const;
private:
	PacketHandlerId id;
};

using PacketHandlerPtr = std::unique_ptr<PacketHandler>;
