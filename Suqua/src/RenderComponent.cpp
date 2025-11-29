#include "RenderComponent.h"
#include "PositionComponent.h"

RenderComponent::RenderComponent(EntityId id_, const Vec2f& offset_) :
	id{id_}
{
    if (!EntitySystem::Contains<PositionComponent>() || EntitySystem::GetComp<PositionComponent>(id) == nullptr) {
        EntitySystem::MakeComps<PositionComponent>(1, &id);
    }
}

RenderComponent::RenderComponent(const RenderComponent & other) :
	id{other.id}
{
    for(auto& sprite : other.sprites) {
        sprites.emplace_back(sprite->clone());
    }
}

RenderComponent & RenderComponent::operator=(const RenderComponent & other) {
	id = other.id;
    for(auto& sprite : other.sprites) {
        sprites.emplace_back(sprite->clone());
    }
    return *this;
}

size_t RenderComponent::allocateDrawable() {
    sprites.resize(sprites.size() + 1);
    return sprites.size() - 1;
}

bool RenderComponent::hasDrawable(size_t index) {
    return sprites[index] != nullptr;
}
