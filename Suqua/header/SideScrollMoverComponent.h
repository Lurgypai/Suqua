#include "ComponentMacros.h"

class SideScrollMoverComponent {
	CompMembers(SideScrollMoverComponent);

public:
	void update();
private:
	float moveSpeed;
	float jumpSpeed;

	float decel;
	float accelGrounded;
	float accelAirborn;

	int jumpsMax;
	int jumpsRemaining;

	int fullJumpTick;
	int fullJumpTickMax;
};