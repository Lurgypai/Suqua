#include "PositionComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"
#include <stdexcept>

using namespace PositionData;

PositionComponent::PositionComponent(EntityId id_, const Vec2f& pos_) :
	id{ id_ },
    pos{ pos_ }
{ }
