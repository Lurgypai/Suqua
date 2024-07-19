#include "ComponentMacros.h"

class SideScrollMoverComponent {
	CompMembers(SideScrollMoverComponent);

public:
	void update();

	float moveSpeed;
	float jumpSpeed;

	float decel;
	float accelGrounded;
	float accelAirborn;
private:
	int jumpsMax;
	int jumpsRemaining;

	int fullJumpTick;
	int fullJumpTickMax;
};