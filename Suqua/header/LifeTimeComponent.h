#include "ComponentMacros.h"

#include <cstdint>

class LifeTimeComponent {
	CompMembers(LifeTimeComponent);
public:
    LifeTimeComponent(EntityId id_, int32_t remainingLife_);
	void update();

	std::int32_t getRemainingLife() const;
	void setRemainingLife(std::int32_t remainingLife_);
private:
	std::int32_t remainingLife;
};
