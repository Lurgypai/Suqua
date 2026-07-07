#include "EntityAddDaemon.h"
#include "DaemonComponent.h"
#include "HandComponent.h"
#include <stdexcept>

void EntityAddDaemon(EntityId id, const EntityAddDaemonArgs& args) {
	EntitySystem::MakeComps<DaemonComponent>(1, &id, args.followRate, args.followOffset);

    auto* hands = EntitySystem::GetComp<HandComponent>(id);
    if(hands == nullptr) throw std::runtime_error{
        "EntityAddDaemon: Missing hand component (does your JSON have a \"hands\"?)"
    };

    hands->handFlags[0] = ControllerBits::BUTTON_7;
    hands->handFlags[1] = ControllerBits::BUTTON_8;
}
