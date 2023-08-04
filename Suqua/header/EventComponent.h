#include "ComponentMacros.h"
#include <vector>

class EventComponent {
	CompMembers(EventComponent);

public:
	using EventId = unsigned int;
	void pushEvent(EventId event);
	const std::vector<EventId>& getEvents() const;

	// clears events
	void update();
private:
	std::vector<EventId> events;
};

/*
* I may com back to this later
* I decided against using event components because there's no provenance
* ie, there's nothing tying the thing that queued the event to the event itself
* two concerns here
*	anything can queue arbitrary events.
*	tracing through calls becomes more difficult. if I forget, or someone else wants to edit the code, there's no way to find where events are queued from.
* 
* both of these things can be solved through good external means, ie, making sure to properly only queue events from the correct places,
* and adding comments to event declarations stating where the events are queued from
* but for now, I'm going to avoid this pattern regardless. I'd rather enshrine these in fundamental designs of the lib over "best practice" reccomendations
*/