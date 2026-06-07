#pragma once
#include "ItemAbility.h"

class IABasicDash : public ItemAbility {
public:
	IABasicDash();

	virtual void update(float delta) override;
	virtual void doAbility(Scene& scene, EntityId sourceEntity, EntityId targetEntity,
            const Controller& controller, const Vec2f& heldPos, float angle) override;
	virtual std::unique_ptr<ItemAbility> clone() const override;
private:
    EntityId targetEntity;

	float dashTime;
	float cooldownTime;

	float elapsedTime;

	enum class DashState {
		ready,
		dashing,
		cooldown
	};

	DashState state;
	Vec2f dashDir;
	float dashSpeed;
};
