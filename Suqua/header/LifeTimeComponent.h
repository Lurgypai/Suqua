#include "EntitySystem.h"
#include "ComponentMacros.h"

#include <cstdint>

class LifeTimeComponent {
	// generate component members
	CompMembers(LifeTimeComponent);

public:
	void update();

	std::uint32_t getRemainingLife() const;
	void setRemainingLife(std::uint32_t remainingLife_);
private:
	std::uint32_t* remainingLife;
};
