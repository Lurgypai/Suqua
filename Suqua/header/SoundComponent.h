#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include <deque>
#include <string>

class SoundComponent {

	friend class SoundSystem;

public:
	SoundComponent(EntityId id_ = 0);
	void triggerSound(const std::string& tag);
	void triggerSound(std::string&& tag);
	void triggerSound(const std::string& tag, Vec2f pos);
	void triggerSound(std::string&& tag, Vec2f&& pos);
	
	EntityId getId() const;
private:
	struct TriggeredSound {
		bool positional;
		std::string tag;
		Vec2f pos;
	};

	std::deque<TriggeredSound> triggered;
	EntityId id;
};