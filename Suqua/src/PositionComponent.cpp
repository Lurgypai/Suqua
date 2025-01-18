#include "PositionComponent.h"
#include "NetworkDataComponent.h"
#include "NetworkDataComponentDataFields.h"

using namespace PositionData;

PositionComponent::PositionComponent(EntityId id_) :
	id{ id_ },
	xPos{ nullptr },
	yPos{ nullptr }
{
    if (!EntitySystem::Contains<NetworkDataComponent>() || !EntitySystem::GetComp<NetworkDataComponent>(id)) {
        EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
    }

    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
    ndc->set(X, 0.0f);
    ndc->set(Y, 0.0f);

    xPos = &ndc->get<float>(X);
    yPos = &ndc->get<float>(Y);
}

PositionComponent::PositionComponent(EntityId id_, const Vec2f& pos) :
	id{ id_ },
	xPos{ nullptr },
	yPos{ nullptr }
{
    if (!EntitySystem::Contains<NetworkDataComponent>() || !EntitySystem::GetComp<NetworkDataComponent>(id)) {
        EntitySystem::MakeComps<NetworkDataComponent>(1, &id);
    }

    auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
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
