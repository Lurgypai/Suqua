#pragma once
#include "EntitySystem.h"
#include <deque>
#include <string>

class SoundComponent {

	friend class SoundSystem;

public:
	SoundComponent(EntityId id_ = 0);
	void triggerSound(const std::string& tag);
	void triggerSound(std::string&& tag);
	
	EntityId getId() const;
private:
	std::deque<std::string> triggered;
	EntityId id;
};