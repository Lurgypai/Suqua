#pragma once
#include "PacketHandler.h"
#include "Scene.h"

class PHClientMakePlayerPuppet : public PacketHandler {
public:
	PHClientMakePlayerPuppet(PacketId id_, Scene* scene_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
    Scene* scene;
};
