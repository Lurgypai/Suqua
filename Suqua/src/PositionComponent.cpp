#include "PositionComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"
#include <stdexcept>

using namespace PositionData;

PositionComponent::PositionComponent(EntityId id_, const Vec2f& pos) :
	id{ id_ },
	xPos{ nullptr },
	yPos{ nullptr }
{
    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    if(ndc == nullptr) throw std::runtime_error{"PositionComponent: NetworkDataComponent was not initialized"};
    ndc->set(X, pos.x);
    ndc->set(Y, pos.y);

    xPos = &ndc->get<float>(X);
    yPos = &ndc->get<float>(Y);
}

Vec2f PositionComponent::getPos() const {
	return Vec2f{ *xPos, *yPos };
}

void PositionComponent::setPos(const Vec2f& newPos) {
	*xPos = newPos.x;
	*yPos = newPos.y;
}
