#include "EntitySystem.h"

class DirectionComponent {
public:
	DirectionComponent(EntityId id_ = 0);
	EntityId getId() const;

	float getDir() const;
	void setDir(float newDir);
	int getCardinalDir() const;
	
	bool isLocked();
	void setLocked(bool locked);
private:
	EntityId id;
	bool isLocked_;
	float* dir;
	int cardinalDir;
};