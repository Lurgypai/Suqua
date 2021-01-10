#include "SoundComponent.h"

SoundComponent::SoundComponent(EntityId id_) :
	id{id_}
{}

EntityId SoundComponent::getId() const {
	return id;
}

void SoundComponent::triggerSound(const std::string& tag) {
	triggered.emplace_back(tag);
}


void SoundComponent::triggerSound(std::string&& tag) {
	triggered.emplace_back(std::move(tag));
}