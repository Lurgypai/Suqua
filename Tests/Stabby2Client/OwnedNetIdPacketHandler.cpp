#include "OwnedNetIdPacketHandler.h"
#include "ClientPlayingScene.h"
#include "Game.h"

OwnedNetIdPacketHandler::OwnedNetIdPacketHandler(PacketId id_, SceneId playingSceneId_) :
	PacketHandler{id_},
	playingSceneId{playingSceneId_}
{}

void OwnedNetIdPacketHandler::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	PacketId packetId;
	NetworkId netId;
	data >> packetId;
	data >> netId;

	game.addOwnedNetId(netId);

	ClientPlayingScene& playingScene = game.getScene<ClientPlayingScene>(playingSceneId);
	playingScene.addControl(game.online.getEntity(netId));
}
