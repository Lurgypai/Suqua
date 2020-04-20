#pragma once
#include "EntitySystem.h"
#include "AABB.h"

enum CaptureState : uint8_t {
	idle = 0,
	capturing = 1,
	contested = 2
};

class CapturePointComponent {
	friend class DominationMode;
public:
	struct StateContainer {
		CaptureState state;
		float remainingCaptureTime;
		float totalCaptureTime;
		uint64_t currTeamId;
		uint64_t targetTeamId;
	};

	CapturePointComponent(EntityId id_ = 0);
	EntityId getId() const;

	void tickCapturePoint(double timeDelta);
	bool isCaptured() const;
	AABB getZone() const;
	StateContainer getState() const;

	void setState(StateContainer newState);

private:
	EntityId id;
	CaptureState state;
	static const double captureSpeed;
	static const double maxCaptureMultiplier;
	float totalCaptureTime;
	float remainingCaptureTime;
	AABB zone;
	unsigned int currTeamId;
	unsigned int targetTeamid;
};