#include "ComponentMacros.h"

#include <cstdint>

class LifeTimeComponent {
	// generate component members
	CompMembers(LifeTimeComponent);

public:
	void update();

	std::int32_t getRemainingLife() const;
	void setRemainingLife(std::int32_t remainingLife_);
private:
	std::int32_t* remainingLife;
};
