#include "PHBoxJoin.h"
#include "OnlineSystem.h"
#include "ByteStream.h"
#include "Game.h"
#include "PositionData.h"
#include "PhysicsData.h"
#include <iostream>

PHBoxJoin::PHBoxJoin(PacketId id_, SceneId targetSceneId_) :
	PacketHandler{id_},
	targetSceneId{targetSceneId_}
{}

void PHBoxJoin::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
	PacketId id;
	data >> id;

	NetworkId netId;
	data >> netId;

	auto& targetScene = game.getScene<ClientBoxScene>(targetSceneId);
	EntityId boxId = targetScene.addBox(game, true, netId);
	EntitySystem::MakeComps<RenderComponent>(1, &boxId);
	auto* render = EntitySystem::GetComp<RenderComponent>(boxId);

	if (netId == targetScene.ourBoxNetId) {
		render->loadDrawable<Sprite>("box");
		targetScene.linkInputToEntity(boxId);
		std::cout << "Registered us joining.\n";
	}
	else {
		std::cout << "Registered client " << netId << " joining.\n";
		render->loadDrawable<Sprite>("evil_box");

		auto* ndc = EntitySystem::GetComp<NetworkDataComponent>(boxId);
		ndc->setSyncMode(X, NetworkDataComponent::SyncMode::INTERPOLATED);
		ndc->setSyncMode(Y, NetworkDataComponent::SyncMode::INTERPOLATED);
		//set other players not to move except by interpolation
		ndc->setSyncMode(FROZEN, NetworkDataComponent::SyncMode::NONE);
		ndc->get<bool>(FROZEN) = true;
	}

	render->getDrawable<Sprite>()->setScale({ 3.0, 3.0 });

	//create the box with the netId
}
