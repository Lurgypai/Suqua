#pragma once
#include "PacketHandler.h"
#include "ClientBoxScene.h"

class PHBoxClientNetworkId : public PacketHandler {
public:
	PHBoxClientNetworkId(PacketId id_, SceneId targetSceneId_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
	SceneId targetSceneId;
};