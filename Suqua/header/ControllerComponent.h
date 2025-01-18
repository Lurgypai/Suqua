#pragma once
#include "ComponentMacros.h"
#include "Controller.h"


//used to associate a controller with an entity
class ControllerComponent {
    CompMembers(ControllerComponent);
public:
	ControllerComponent(EntityId id_ );
    bool operator==(const ControllerComponent& other) const = default;
    bool operator!=(const ControllerComponent& other) const = default;

	//getter and setter in case i want to change the underlying whats of what controller is (base class or whatnot)
	void setController(const Controller & cont);
	Controller& getController();
    const Controller& getController() const;

private:
	Controller controller;
};
