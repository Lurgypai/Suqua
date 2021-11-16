#pragma once
#include "PacketHandler.h"
#include "ClientBoxScene.h"

class PHBoxJoin : public PacketHandler {
public:
	PHBoxJoin(PacketId id_, SceneId targetSceneId_);
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override;
private:
	SceneId targetSceneId;
};