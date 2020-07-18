#include "EntitySystem.h"
#include "ClimbableNavMesh.h"

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
	const std::list<unsigned int>& getCurrentPath() const;
	const NavZone& getCurrZone() const;
	const NavZone& getTargetZone() const;

	static const ClimbableNavMesh& getNavMesh();

private:
	EntityId id;
	NavZone currZone;
	NavZone targetZone;
	std::list<unsigned int> currPath;

	static bool meshGenerated;
	static ClimbableNavMesh navMesh;
};