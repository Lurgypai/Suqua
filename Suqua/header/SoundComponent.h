#pragma once
#include <deque>
#include <string>

#include "ComponentMacros.h"
#include "Vec2.h"

class SoundComponent {
	friend class SoundSystem;
    CompMembers(SoundComponent);
public:
	SoundComponent(EntityId id_);
	void triggerSound(const std::string& tag);
	void triggerSound(std::string&& tag);
	void triggerSound(const std::string& tag, Vec2f pos);
	void triggerSound(std::string&& tag, Vec2f&& pos);
private:
	struct TriggeredSound {
		bool positional;
		std::string tag;
		Vec2f pos;
	};

	std::deque<TriggeredSound> triggered;
};
