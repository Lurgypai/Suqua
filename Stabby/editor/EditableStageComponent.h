#pragma once
#include "AABB.h"
#include "stage.h"

enum class AnchorSide {
	right,
	left,
	top,
	bottom,
	topright,
	bottomright,
	topleft,
	bottomleft
};

enum class EditableState {
	moving,
	resizing,
	idle
};

class EditableStageComponent {

	friend class EditableSystem;

public:
	EditableStageComponent(EntityId id_ = 0);

	const EntityId getId() const;
	void update(int camId);
	bool isDefaultSpawn();

	const AABB& getCollider() const;
	StageElement getType() const;
private:
	static constexpr int minSize = 2;

	void adjustLeft(const Vec2f mousePos);
	void adjustRight(const Vec2f mousePos);
	void adjustTop(const Vec2f mousePos);
	void adjustBottom(const Vec2f mousePos);

	EntityId id;

	StageElement type;

	Vec2i anchorPoint;
	Vec2f prevPoint;
	Vec2f prevRes;
	AABB collider;
	EditableState state;
	AnchorSide anchorSide;

	//this way, after the first one is toggled, none of the others will be. Effecively, editing the "top" editable
	bool prevButton1;
	bool prevButton2;
	bool prevButton3;

	bool defaultSpawn;
};
