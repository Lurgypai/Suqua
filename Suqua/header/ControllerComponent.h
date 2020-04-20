#pragma once
#include "EntitySystem.h"
#include "Controller.h"

//used to associate a controller with an entity
class ControllerComponent {
public:
	ControllerComponent(EntityId id_ = 0);
	const EntityId getId() const;

	//getter and setter in case i want to change the underlying whats of what controller is (base class or whatnot)
	void setController(const Controller & cont);
	Controller& getController();

private:
	Controller controller;
	EntityId id;
};