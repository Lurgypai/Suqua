#include "EntitySystem.h"

class DirectionComponent {
public:
	DirectionComponent(EntityId id_ = 0);
	EntityId getId() const;

	float getDir() const;
	void setDir(float newDir);
private:
	EntityId id;

	float* dir;
};