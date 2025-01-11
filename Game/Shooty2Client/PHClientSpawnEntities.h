#pragma once
#include "PacketHandler.h"
#include "Scene.h"

class PHClientSpawnEntities : public PacketHandler {
public:
	PHClientSpawnEntities(PacketId id_, Scene* scene_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
    Scene* scene;
};
