#pragma once
#include "PacketHandler.h"
#include "Scene.h"

class OwnedNetIdPacketHandler : public PacketHandler {
public:
	OwnedNetIdPacketHandler(PacketId id_, SceneId playingSceneId_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;

private:
	SceneId playingSceneId;
};