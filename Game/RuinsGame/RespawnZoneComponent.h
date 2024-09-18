#include "ComponentMacros.h"
#include "AABB.h"

class RespawnZoneComponent {
    CompMembers(RespawnZoneComponent);
public:
    AABB zone;
private:
};
