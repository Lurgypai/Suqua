#include "EntitySystem.h"
#include "ClimbableNavMesh.h"

class AIPlayerComponent {
public:
	AIPlayerComponent(EntityId id_ = 0);
	EntityId getId() const;

	enum class State {
		capture,
		defend,
		wander,
		combat
	};

	void update();
	const NavPath& getCurrentPath() const;
	const NavZone& getCurrZone() const;
	const NavZone& getTargetZone() const;

	static const ClimbableNavMesh& getNavMesh();

private:
	void chooseNewState();

	EntityId id;
	EntityId targetId;
	unsigned int timer;
	State state;
	NavZone currZone;
	NavZone targetZone;
	NavPath currPath;
	bool posTargeted;

	static bool meshGenerated;
	static ClimbableNavMesh navMesh;
};