#include "SideScrollMoverComponent.h"
#include "ControllerComponent.h"
#include "PhysicsComponent.h"
#include "DirectionComponent.h"
#include "AngleUtil.h"

SideScrollMoverComponent::SideScrollMoverComponent(EntityId id_) :
	id{ id_ },
	moveSpeed{ 90.0f },
	jumpSpeed{ 130 },
	decel{4.0f},
	accelGrounded{ 8.0f },
	accelAirborn{ 2.0f },
	jumpsMax { 1 },
	jumpsRemaining{ 0 },
	fullJumpTick{ 0 },
	fullJumpTickMax{ 16 }
{}

void SideScrollMoverComponent::update() {
	auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);
	auto* physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
	if (contComp && physicsComp) {
		const auto& controller = contComp->getController();
		Vec2f vel = physicsComp->getVel();
		// LEFT RIGHT MOVEMENT
		// get direction
		float dir = 0.0f;
		if (controller.stick1.x > 0.5f) dir = 1.0f;
		else if (controller.stick1.x < -0.5f) dir = -1.0f;

		auto dirComp = EntitySystem::GetComp<DirectionComponent>(id);
		if (dir < 0) dirComp->setDir(0);
		else if (dir > 0) dirComp->setDir(-PI);

		// if moving, accelerate
		if (dir != 0.0f) {
			float xAccel = 0.0f;

			if (physicsComp->isGrounded()) {
				xAccel = dir * accelGrounded;
			}
			else {
				xAccel = dir * accelAirborn;
			}

			vel.x += xAccel;
			if (vel.x > moveSpeed) vel.x = moveSpeed;
			else if (vel.x < -moveSpeed) vel.x = -moveSpeed;
		}
		// otherwise decelerate if grounded
		else {
			if (physicsComp->isGrounded()) {
				if (vel.x > decel) vel.x -= decel;
				else if (vel.x < -decel) vel.x += decel;
				else vel.x = 0;
			}
		}

		// JUMPING
		if (physicsComp->isGrounded()) {
			jumpsRemaining = jumpsMax;
		}

		if (controller[ControllerBits::BUTTON_5]) {
			// started jump
			if (controller.toggled(ControllerBits::BUTTON_5)) {
				if (jumpsRemaining > 0) {
					jumpsRemaining -= 1;
					vel.y = -jumpSpeed;

					//if its not the first jump, disable "full jumping"
					if (jumpsRemaining < jumpsMax - 1) fullJumpTick = fullJumpTickMax;
					else fullJumpTick = 0;
				}
			}
			// holding jump
			else {
				if (fullJumpTick != fullJumpTickMax) {
					vel.y = -jumpSpeed;
				}
			}
		}

		if (fullJumpTick < fullJumpTickMax) {
			++fullJumpTick;
		}

		physicsComp->setVel(vel);
	}

}
