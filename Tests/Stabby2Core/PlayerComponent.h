#pragma once
#include "EntitySystem.h""

class PlayerComponent {
public:
	enum class PlayerState {
		idle,
		walking,
		jumpsquat,
		airborn,
		landing
	};

	PlayerComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update();
	PlayerState getState() const;
private:
	void idle();
	void walking();
	void jumpsquat();
	void airborn();
	void landing();

	void decelerate();

	int actionFrame;
	int jumpSquatMax;
	int landingMax;
	float jumpVel;
	float moveSpeed;
	float groundedDecel;

	PlayerState state;
	EntityId id;
};