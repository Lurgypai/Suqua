#include "EntitySystem.h"

class BoxComponent {
public:
	BoxComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update();
private:
	EntityId id;
};