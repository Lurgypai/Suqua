#include "Packet.h"

namespace Shooty2Packet {
    constexpr PacketId SpawnEntities = 10;
    /* SpawnEntity:
     * Client->Server
     *  string EntityTag
     *  EntityId targetEntity
     *  float x
     *  float y
     *  uint32_t subEntityCount
     *    EntityId target
     *
     *
     * Server->Client
     *   string EntityTag
     *   float x
     *   float y
     *   NetworkId netId
     *   NetworkOwnerComponent::Owner owner
     *     NetworkID netId
     *
     * NOTE:
     *  When a spawn function returns multiple entities,
     *  we expect the sub entities to all follow the initial parent entity. 
     *  no count is sent from server to client because the client knows how many sub entities there are
     */

    constexpr PacketId AssignNetworkId = 11;
    /*
     * AssignNetworkId:
     * Server->client
     *   EntityId targetEntity
     *   NetworkId netId
     */

};
