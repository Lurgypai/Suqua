#include "ComponentMacros.h"

#include <cstdint>

class HealthComponent {
	CompMembers(HealthComponent);
public:
    HealthComponent(EntityId id_, int32_t health_);

	std::int32_t getHealth() const;
	void setHealth(std::int32_t health);
	void damage(std::int32_t amount);

	std::int32_t health;
private:
};
