#include <print>

#include "Game.h"
#include "PHServerDamage.h"
#include "UUID.h"
#include "NetworkDataComponent.h"
#include "HealthComponent.h"

PHServerDamage::PHServerDamage(PacketId id_) : PacketHandler(id_) {}

void PHServerDamage::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId id;
    data >> id;

    while(data.hasMoreData()) {
        Suqua::UUID uuid;
        std::uint32_t damage;

        data >> uuid;
        data >> damage;

        EntityId entityId = NetworkDataComponent::GetEntityId(uuid);
        // this entity doesn't exist (already killed?)
        if(entityId == 0) continue;
        auto ndc = EntitySystem::GetComp<NetworkDataComponent>(entityId);

        auto healthComp = EntitySystem::GetComp<HealthComponent>(entityId);
        auto health = healthComp->health;
        healthComp->damage(damage);

        // update previous to prevent this from being sent out
        ndc->storePrev();
    }

    data.setPos(0);
    for(auto& peerId : game.host.getConnectedPeers()) {
        if(peerId != sourcePeer) game.host.bufferDataToChannel(peerId, 0, data);
    }
}

// change to only send deltas
//  the client sends a state delta to apply damage
//  the server applies that delta and tells the other clients about it
//
//  the client sends a position delta to apply movement
//  the server applies that delta and tells the other clients about it
//  
//  the server applies a local delta
//  tells all clients
//      cache the server delta after update before handling packets?
//      cache the server delta whenever we get a delta to apply?

/* THIS FILE HAS BEEN OUTMODED
 * Damage is now supplied by deltas
 *  the current state of an entity is stored to prevent it being part of the delta
 *  a delta is applied
 *  the deltas are sent
 * 
 * Current bugs
 *  Now we can kill a think client side and eventually it will be killed server side. However, it can still interact with the world server side before its died client side
 *      tie server side interaction (spawning bullets) to the entity, and ignore actions from dead entities <- THIS OPTION
 *          some complexity
 *          does it make sense to tie spawn requests to an entity asking to spawn them?
 *      turn interactions into requests for the client to decide on (server sends "shoot request")
 *          interaction from the server is delayed more (monsters aim gets worse the higher the latency)
 *          a new layer to interaction (now we have to use the request shoot interface as opposed to just running the same shoot code on client and server)
 *
 * also consider wrapping all packets in a "entity requester" indicating if/what entity is trying to take that packets action
 *
 * spawn system needs to support entities requesting entities to be spawned
 */
