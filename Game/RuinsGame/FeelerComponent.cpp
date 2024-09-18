#include "FeelerComponent.h"
#include "PositionComponent.h"


#include "DebugIO.h"

FeelerComponent::FeelerComponent(EntityId id_) :
    id{id_}
{}

void FeelerComponent::update(const World& level) {
    std::string isTile = "False";

    auto posComp = EntitySystem::GetComp<PositionComponent>(id);
    if(level.hasTile(posComp->getPos() + ray)) isTile = "True";

    DebugIO::setLine(8, "isTile" + isTile);
}

