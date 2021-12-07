#include "EntitySystem.h"
#include "Game.h"

class BoxComponent {
public:
	static constexpr unsigned int PREV_X = 20;
	static constexpr unsigned int PREV_Y = 21;

	BoxComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update(const Game& game);
private:
	EntityId id;
};