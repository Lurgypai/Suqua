#include "SoundComponent.h"

SoundComponent::SoundComponent(EntityId id_) :
	id{id_}
{}

EntityId SoundComponent::getId() const {
	return id;
}

void SoundComponent::triggerSound(const std::string& tag) {
	triggered.emplace_back(TriggeredSound{ false, tag, Vec2f{} });
}


void SoundComponent::triggerSound(std::string&& tag) {
	triggered.emplace_back(TriggeredSound{ false, std::move(tag), Vec2f{} });
}

void SoundComponent::triggerSound(const std::string& tag, Vec2f pos) {
	triggered.emplace_back(TriggeredSound{ true, tag, pos });
}

void SoundComponent::triggerSound(std::string&& tag, Vec2f&& pos) {
	triggered.emplace_back(TriggeredSound{ true, std::move(tag), std::move(pos) });
}