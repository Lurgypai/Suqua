#pragma once
#include "ItemAbility.h"

class IABasicDash : public ItemAbility {
public:
	IABasicDash();

	virtual void update(float delta) override;
	virtual void doAbility(Scene& scene, EntityId sourceEntity, const Controller& controller, InventoryItem& sourceInvItem) override;
	virtual std::unique_ptr<ItemAbility> clone() const override;
private:
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
	EntityId activatingEntity;
};
