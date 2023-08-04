#include "RenderComponent.h"
#include "PositionComponent.h"

RenderComponent::RenderComponent(EntityId id_) :
	id{id_},
	offset{0, 0}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || EntitySystem::GetComp<PositionComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
		}
	}
}

RenderComponent::RenderComponent(const RenderComponent & other) :
	id{other.id},
	offset{other.offset}
{
	if (other.sprite != nullptr) {
		sprite = SpritePtr{ other.sprite->clone() };
	}
	else {
		sprite = nullptr;
	}
}

RenderComponent::RenderComponent(RenderComponent&& other) :
	id{ other.id },
	offset{ other.offset },
	sprite{ std::move(other.sprite) }
{}

RenderComponent & RenderComponent::operator=(const RenderComponent & other) {
	id = other.id;
	offset = other.offset;
	if (other.sprite != nullptr) {
		sprite = SpritePtr{ other.sprite->clone() };
	}
	else {
		sprite = nullptr;
	}
	return *this;
}

EntityId RenderComponent::getId() const {
	return id;
}
