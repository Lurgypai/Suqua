#pragma once
#include "PacketHandler.h"
#include "Scene.h"

class JoinPacketHandler : public PacketHandler {
public:
	JoinPacketHandler(PacketId id_, SceneId playingSceneId_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
	SceneId playingSceneId;
};