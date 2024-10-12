#include "PHClientState.h"
#include "Game.h"
#include "NetworkDataComponent.h"
#include "../Shooty2Core/EntityGenerator.h"
#include "CharacterGFXComponent.h"

PHClientState::PHClientState(PacketId id_, Scene* scene_) :
	PacketHandler{ id_ },
    scene{scene_}
{}

void PHClientState::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId packetId;
    data >> packetId;

    NetworkId netId;
    data >> netId;

    EntityId id = game.online.getEntity(netId);
    if(id == 0) {
        // TEMP FIX, NEED TO KEEP TRACK OF ENTITIES THE CLIENTS OWN ON THE SERVER SO THEY CAN BE SENT TO THE CONNECTING CLIENTS
        id = EntityGenerator::SpawnPlayerPuppet(*scene)[0];
        game.online.registerOnlineComponent(id, netId);

        EntitySystem::MakeComps<CharacterGFXComponent>(1, &id);
        EntitySystem::GetComp<CharacterGFXComponent>(id)->loadSpriteSheet("hero", "stranded/Hero/Hero/Hero.json", Vec2f{ -13, -24 });
        EntitySystem::GetComp<CharacterGFXComponent>(id)->setHasUpDown(true);
    }


    NetworkDataComponent* ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    if(ndc) ndc->unserialize(data);
}
