#include "EntityAddRectGFX.h"
#include "RenderComponent.h"
#include "RectDrawable.h"

void EntityAddRectGFX(EntityId id, const EntityAddRectGFXArgs& args) {
    EntitySystem::MakeComps<RenderComponent>(1, &id);
    auto* render = EntitySystem::GetComp<RenderComponent>(id);
    render->loadDrawable<RectDrawable>(RectDrawable {
            args.c,
            args.filled,
            0.f,
            AABB{{0.f, 0.f}, args.res}
            });
}
