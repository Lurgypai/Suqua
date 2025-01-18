#pragma once

#include "ComponentMacros.h"
#include "AABB.h"


class MenuButtonComponent {
	friend class Menu;

    CompMembers(MenuButtonComponent);
public:
	MenuButtonComponent(EntityId id_, const AABB& boundingBox_, const std::string& tag_);

	void update(Vec2f mousePos, bool toggled_);

	bool pollToggled();
	const AABB& getBoundingBox() const;
	const std::string& getTag() const;
private:
	AABB boundingBox;

	std::string tag;

	bool isActive;
	bool wasActive;
	bool toggled;
};
