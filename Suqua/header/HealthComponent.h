#include "ComponentMacros.h"

class HealthComponent {
	CompMembers(HealthComponent);
public:
	std::uint32_t getHealth() const;
	void setHealth(std::uint32_t health);
	void damage(std::uint32_t amount);
private:
	std::uint32_t* health;
};