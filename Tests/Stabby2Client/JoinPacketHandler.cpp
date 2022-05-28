#include "JoinPacketHandler.h"
#include "ClientPlayingScene.h"
#include "Game.h"

JoinPacketHandler::JoinPacketHandler(PacketId id_, SceneId playingSceneId_) :
	PacketHandler{id_},
	playingSceneId{playingSceneId_}
{
}

void JoinPacketHandler::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	PacketId id;
	data >> id;
	NetworkId netId;
	data >> netId;

	ClientPlayingScene& playingScene = game.getScene<ClientPlayingScene>(playingSceneId);
	EntityId newPlayer = playingScene.addPlayer();
	game.online.registerOnlineComponent(newPlayer, netId);

	//set player to be frozen, and not to sync frozen, so that they'll only be moved when new positions come in

}
