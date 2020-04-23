#pragma once

#include "EntitySystem.h"
#include "AABB.h"

class MenuButtonComponent {
	friend class Menu;

public:
	MenuButtonComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update(Vec2f mousePos, bool toggled_);

	bool pollToggled();
	const AABB& getBoundingBox() const;
	const std::string& getTag() const;
private:
	EntityId id;

	AABB boundingBox;

	std::string tag;

	bool isActive;
	bool wasActive;
	bool toggled;
};