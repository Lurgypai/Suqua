#include "EventComponent.h"

EventComponent::EventComponent(EntityId id_) :
	id{ id_ },
	events{}
{}

void EventComponent::pushEvent(EventId event) {
	events.emplace_back(event);
}

const std::vector<EventComponent::EventId>& EventComponent::getEvents() const {
	return events;
}

void EventComponent::update() {
	events.clear();
}

