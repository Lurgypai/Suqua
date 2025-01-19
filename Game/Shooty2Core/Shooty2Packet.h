#include "Packet.h"

namespace Shooty2Packet {
    constexpr PacketId SpawnEntities = 10;
    /* SpawnEntity:
     *  string EntityTag
     *  Vec2f pos
     *  uint32_t uuid count
     *      UUID uuids
     *
     * NOTE:
     *  When a spawn function returns multiple entities,
     *  we expect the sub entities to all follow the initial parent entity. 
     *  no count is sent from server to client because the client knows how many sub entities there are
     */
};
