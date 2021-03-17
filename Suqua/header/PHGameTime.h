#include "PacketHandler.h"

class PHGameTime : public PacketHandler {
public:
	PHGameTime(PacketHandlerId id_);
	void handlePacket(ByteStream& data, Game& game) override;
};