#include "ComponentMacros.h"

class GolfSwingComponent {
	CompMembers(GolfSwingComponent);

public:
	void update();
	int delay;
	int duration;
	int lag;
private:
	int tick;
	bool isActive_;

	EntityId hitboxId;
};