#include "ComponentMacros.h"

#include <cstdint>
#include <functional>

class HealthComponent {
	CompMembers(HealthComponent);
public:
    using DeathCallback = std::function<void()>;
    HealthComponent(EntityId id_, int32_t health_, DeathCallback deathCallback_ = DeathCallback{});

	std::int32_t getHealth() const;
	void setHealth(std::int32_t health);
	void damage(std::int32_t amount);

	std::int32_t health;
    DeathCallback deathCallback;
};
