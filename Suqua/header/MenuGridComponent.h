#include "EntitySystem.h"
#include "StringTree.h"
#include "AABB.h"

#include <vector>

class MenuGridComponent {

	friend class Menu;

public:
	MenuGridComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update(Vec2f mousePos, bool toggled_);
	void setPrefix(const std::string& s);

	bool pollToggled();
	const AABB& getBoundingBox() const;
	const std::string& getTag() const;
	const std::string& getSelected() const;
	StringTree getCurrButtons() const;
	const StringTree& getButtons() const;
	void setButtons(std::vector<std::string> s);
	std::vector<AABB> generateButtonBoxes();

	Vec2f buttonRes;
	Vec2f margins;	//space between buttons
	std::string prefix;
private:
	AABB boundingBox;
	StringTree buttons;
	EntityId id;

	bool isActive;
	bool wasActive;
	bool toggled;

	std::string selected;
	std::string tag;
};