#include "PlayerComponent.h"
#include "PhysicsComponent.h"
#include "ControllerComponent.h"
#include "NetworkDataComponent.h"

#include <iostream>


PlayerComponent::PlayerComponent(EntityId id_) :
	id{id_},
	actionFrame{ 0 },
	jumpSquatMax{ 8 },
	landingMax{ 8 },
	jumpVel{ 500 },
	moveSpeed{ 50 },
	groundedDecel{ 10 },
	state{PlayerState::idle}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PhysicsComponent>() || !EntitySystem::GetComp<PhysicsComponent>(id)) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
			auto* physics = EntitySystem::GetComp<PhysicsComponent>(id);
			physics->setRes({ 64, 64 });
			physics->teleport({ 480 / 2, 270 / 2 });
			physics->setWeight(10);
		}
		if (!EntitySystem::Contains<ControllerComponent>() || !EntitySystem::GetComp<ControllerComponent>(id)) {
			EntitySystem::MakeComps<ControllerComponent>(1, &id);
		}
	}
}

EntityId PlayerComponent::getId() const {
	return id;
}

void PlayerComponent::update() {
	switch (state) {
	case PlayerState::idle: idle(); break;
	case PlayerState::walking: walking(); break;
	case PlayerState::jumpsquat: jumpsquat(); break;
	case PlayerState::airborn: airborn(); break;
	case PlayerState::landing: landing(); break;
	}
}

PlayerComponent::PlayerState PlayerComponent::getState() const
{
	return state;
}

void PlayerComponent::idle() {
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if (c[ControllerBits::LEFT] || c[ControllerBits::RIGHT]) {
		state = PlayerState::walking;
		return;
	}
	if (c[ControllerBits::BUTTON_5]) {
		actionFrame = 0;
		state = PlayerState::jumpsquat;
		return;
	}
	if (!physics->isGrounded()) {
		state = PlayerState::airborn;
		return;
	}
	decelerate();
}

void PlayerComponent::walking() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();

	if (c[ControllerBits::BUTTON_5]) {
		actionFrame = 0;
		state = PlayerState::jumpsquat;
		return;
	}

	int dir = 0;
	if (c[ControllerBits::LEFT]) --dir;
	if (c[ControllerBits::RIGHT]) ++dir;

	auto vel = physics->getVel();
	vel.x = dir * moveSpeed;

	if (vel.x == 0) {
		state = PlayerState::idle;
		return;
	}
	else {
		physics->setVel(vel);
	}
}

void PlayerComponent::jumpsquat() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	++actionFrame;
	if (actionFrame == jumpSquatMax) {
		physics->setVel({ physics->getVel().x, -jumpVel });
		state = PlayerState::airborn;
		return;
	}
	decelerate();
}

void PlayerComponent::airborn() {

	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if (physics->isGrounded()) {
		actionFrame = 0;
		state = PlayerState::landing;
		return;
	}

	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();

	int dir = 0;
	if (c[ControllerBits::LEFT]) --dir;
	if (c[ControllerBits::RIGHT]) ++dir;

	auto vel = physics->getVel();
	vel.x = dir * moveSpeed;

	physics->setVel(vel);
}

void PlayerComponent::landing() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	const Controller& c = cont->getController();

	++actionFrame;
	if (actionFrame == landingMax) {
		if (c[ControllerBits::LEFT] || c[ControllerBits::RIGHT]) {
			state = PlayerState::walking;
		}
		else if (c[ControllerBits::BUTTON_5]) {
			state = PlayerState::jumpsquat;
		}
		else {
			state = PlayerState::idle;
		}
		return;
	}
	decelerate();
}

void PlayerComponent::decelerate() {
	PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(id);

	if (physics->getVel().x > groundedDecel / 2) {
		physics->accelerate({ -groundedDecel, 0 });
	}
	else if (physics->getVel().x < -groundedDecel / 2) {
		physics->accelerate({ groundedDecel, 0 });
	}
	else {
		physics->setVel({ 0, 0 });
	}
}
