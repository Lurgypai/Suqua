#include "PHClientMakePlayerPuppet.h"
#include "Game.h"
#include "CharacterGFXComponent.h"

#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/EntityGenerator.h"

PHClientMakePlayerPuppet::PHClientMakePlayerPuppet(PacketId id_, Scene* scene_) :
	PacketHandler{ id_ },
    scene{scene_}
{}

void PHClientMakePlayerPuppet::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    std::cout << "Client received make puppet command\n";
    PacketId packetId;
    data >> packetId;

    EntityId entityId = EntityGenerator::SpawnPlayerPuppet(*scene)[0];
    NetworkId netId;
    data >> netId;
    game.online.registerOnlineComponent(entityId, netId);

	EntitySystem::MakeComps<CharacterGFXComponent>(1, &entityId);
	EntitySystem::GetComp<CharacterGFXComponent>(entityId)->loadSpriteSheet("hero", "stranded/Hero/Hero/Hero.json", Vec2f{ -13, -24 });
	EntitySystem::GetComp<CharacterGFXComponent>(entityId)->setHasUpDown(true);

}
