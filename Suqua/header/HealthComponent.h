#include "ComponentMacros.h"

#include <cstdint>

class HealthComponent {
	CompMembers(HealthComponent);
public:
	std::int32_t getHealth() const;
	void setHealth(std::int32_t health);
	void damage(std::int32_t amount);
private:
	std::int32_t* health;
};
