#include "Vec2.h"
#include "NavZone.h"
#include <vector>
#include <list>
class NavMesh {
public:
	NavMesh();
	std::list<NavZone> getPath(Vec2f targetPos);
	const std::vector<NavZone>& getZones() const;
private:
	std::vector<NavZone> zones;
};