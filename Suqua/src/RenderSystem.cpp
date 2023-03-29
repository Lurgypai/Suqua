#include "RenderSystem.h"
#include "AnimatedSprite.h"
#include "PositionComponent.h"

void RenderSystem::drawAll() const {
	if (EntitySystem::Contains<RenderComponent>()) {
		for (auto& comp : EntitySystem::GetPool<RenderComponent>()) {
			draw(comp);
		}
	}
}

inline void RenderSystem::draw(RenderComponent& render) const {
	if (EntitySystem::Contains<RenderComponent>()) {
		IDrawable* drawable = render.sprite.get();
		if (drawable != nullptr) {
			auto posComp = EntitySystem::GetComp<PositionComponent>(render.getId());
			drawable->setPos(posComp->getPos() + render.offset);
			drawable->draw();
		}
	}
}
