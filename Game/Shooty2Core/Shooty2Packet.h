#include "PacketHandler.h"

namespace Shooty2Packet {
    constexpr PacketId SpawnEntities = 10;
    /* SpawnEntity:
     *  string EntityTag
     *  Vec2f pos
     *  UUID uuid
     */
    constexpr PacketId AddPlayer = 11;
    /* AddPlayer:
     *  UUID uuid;
     */
    constexpr PacketId TeleportPlayer = 12;
    /* TeleportPlayer:
     *  Vec2f pos
     */
    constexpr PacketId Damage = 13;
    /* Damage:
     *  UUID uuid
     *  uint32_t damage
     */
};
