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

/* I think you applied the changes you'd planned for networking. Everything still works on deltas cleanly.
 * You use a damage packet specifically because just applying the damage isn't enough,
 * entities have to respond to being damaged by calling the "damage" function
 * and a packet is cleaner than tracking specifically this change in the state parsing
 *
 * look to the the games ordering to see how this panned out
 * essential, the server (and client) first broadcast any changes they've made,
 * then apply deltas they've received
 *   those deltas are directly sent to the non-sending peer
 *   and applied locally
 * the previous state is set at the end of all of this, so no additional packets relating to these are sent
 * when we get to fixing networking, consider decoupling ndc::storeprev
 * just use a function that stores the previous state of ndcs
 *
 * Current bugs
 *  dead entities packets (shooting) may arrive after they've died
 *      tie server side interaction (spawning bullets) to the entity, and ignore actions from dead entities
 *
 * also consider wrapping all packets in a "entity requester" indicating if/what entity is trying to take that packets action
 *
 * spawn system needs to support entities requesting entities to be spawned
 */
