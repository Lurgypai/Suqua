#include "EntitySystem.h"

class AIPlayerComponent {
public:
	AIPlayerComponent(EntityId id_ = 0);
	EntityId getId() const;

	enum class State {
		IDLE,
		DEFEND,
		ATTACK
	};

	void update();

private:
	EntityId id;
};