#pragma once
#include "Tilemap.h"
#include "PhysicsComponent.h"

using TilemapId = std::size_t;

class PhysicsSystem {
public:
	PhysicsSystem();
	
	void runPhysics(double timeDelta);
    void runPhysicsOnOwned(double timeDelta);

	void runPhysics(double timeDelta, PhysicsComponent& physicsComp);

    TilemapId loadTileMap(Tilemap&& tilemap);
    const Tilemap& getTilemap(TilemapId tilemapId) const;
    const Tilemap* getTilemapContains(const Vec2f& pos) const;
private:
    // this is a bit of an ugly solution, but it should improve performance enough for now.
    void getActive();
    void getActiveOwned();
    std::vector<PhysicsComponent*> active;
    std::vector<PhysicsComponent*> collidesWith;

    std::vector<Tilemap> tilemaps;
};
