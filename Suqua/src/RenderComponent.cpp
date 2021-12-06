#include "RenderComponent.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"

using NDC = NetworkDataComponent;

RenderComponent::RenderComponent(EntityId id_) :
	id{id_},
	offset{0, 0}
{
	if (id != 0) {
		if (!EntitySystem::Contains<NDC>() || EntitySystem::GetComp<NDC>(id) == nullptr) {
			EntitySystem::MakeComps<NDC>(1, &id);
			NDC* data = EntitySystem::GetComp<NDC>(id);
			data->set<float>(X, 0);
			data->set<float>(Y, 0);
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
