#include "StatBarComponent.h"
#include "RenderComponent.h"
#include "RectDrawable.h"
#include "PositionComponent.h"

StatBarComponent::StatBarComponent(EntityId id_) :
	id{id_},
	flipped{false},
	c{1.0, 1.0, 1.0, 1.0},
	fadeIn{false}
{
	if (id != 0) {
		if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
			EntitySystem::GetComp<RenderComponent>(id)->loadDrawable<RectDrawable>();
			EntitySystem::GetComp<RenderComponent>(id)->getDrawable<RectDrawable>()->filled = true;
			EntitySystem::GetComp<RenderComponent>(id)->getDrawable<RectDrawable>()->c = {1.0, 1.0, 1.0, 1.0};
			EntitySystem::GetComp<RenderComponent>(id)->getDrawable<RectDrawable>()->depth = -1.0;
		}
	}
}

StatBarComponent::StatBarComponent(const StatBarComponent& other) :
	id{ other.id },
	flipped{other.flipped},
	statReader{},
	c{ other.c},
	fadeIn{other.fadeIn}
{
	if (other.statReader) {
		statReader = StatReaderPtr{ other.statReader->clone() };
	}
	else {
		statReader = nullptr;
	}
}

StatBarComponent& StatBarComponent::operator=(const StatBarComponent& other) {
	id = other.id;
	flipped = other.flipped;
	if (other.statReader) {
		statReader = StatReaderPtr{ other.statReader->clone() };
	}
	else {
		statReader = nullptr;
	}
	return *this;
}

EntityId StatBarComponent::getId() const {
	return id;
}

void StatBarComponent::update() {
	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
	RectDrawable* rect = render->getDrawable<RectDrawable>();
	rect->shape = getCurrBarSize();
	if (!fadeIn) {
		rect->c = c;
	}
	else {
		float ratio = statReader->getRatio();
		rect->c = { 1.0f - (1.0f - c.r) * ratio, 1.0f - (1.0f - c.g) * ratio, 1.0f - (1.0f - c.b) * ratio, c.a };
	}

	PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(id);
	pos->pos = rect->shape.pos;
}

void StatBarComponent::setTarget(EntityId target_) {
	target = target_;
}

AABB StatBarComponent::getCurrBarSize() const {
	if (!flipped) {
		return AABB{ fullSize.pos, {fullSize.res.x * statReader->getRatio(), fullSize.res.y} };
	}
	else {
		Vec2f corner = fullSize.pos;
		Vec2f start = { corner.x + (fullSize.res.x * (1.0f - statReader->getRatio())), corner.y };
		return AABB{ start, {fullSize.res.x * statReader->getRatio(), fullSize.res.y} };
	}
}
