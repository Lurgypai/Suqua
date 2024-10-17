#pragma once
#include "PacketHandler.h"
#include "Scene.h"

class PHServerSpawnEntities : public PacketHandler {
public:
	PHServerSpawnEntities(PacketId id_, Scene* scene_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
    Scene* scene;
};
