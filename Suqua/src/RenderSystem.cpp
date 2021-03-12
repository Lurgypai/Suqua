#include "RenderSystem.h"
#include "AnimatedSprite.h"
#include "NetworkDataComponent.h";
#include "PositionData.h"

using NDC = NetworkDataComponent;

void RenderSystem::drawAll() {
	if (EntitySystem::Contains<RenderComponent>()) {
		for (auto& comp : EntitySystem::GetPool<RenderComponent>()) {
			draw(comp);
		}
	}
}

inline void RenderSystem::draw(RenderComponent& render) {
	if (EntitySystem::Contains<RenderComponent>()) {
		IDrawable* drawable = render.sprite.get();
		if (drawable != nullptr) {
			NetworkDataComponent* data = EntitySystem::GetComp<NetworkDataComponent>(render.getId());
			Vec2f pos{ data->get<float>(X), data->get<float>(Y) };
			drawable->setPos(pos - render.offset);
			drawable->draw();
		}
	}
}