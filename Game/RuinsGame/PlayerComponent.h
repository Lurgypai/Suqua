#pragma once
#include "EntitySystem.h"
#include "NetworkDataComponent.h"
#include "ControllerComponent.h"

#include <iostream>

class PlayerComponent {
public:
	enum PlayerVars : NetworkDataComponent::DataId {
		playerStateVarId = 30,
		playerDirVarId = 31,
	};

	enum class State : uint32_t {
		idle,
	};

	PlayerComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update();

	inline State getPlayerState() const;
private:
	EntityId id;

	inline void setPlayerState(State newPlayerState);

	inline const Controller& getController() const;

	void beginIdle();
	void idle_();
};

inline PlayerComponent::State PlayerComponent::getPlayerState() const
{
	auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	return static_cast<State>(ndc->get<uint32_t>(playerStateVarId));
}

inline void PlayerComponent::setPlayerState(State newPlayerState) {
	auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	ndc->get<uint32_t>(playerStateVarId) = static_cast<uint32_t>(newPlayerState);
}

inline const Controller& PlayerComponent::getController() const {
	auto controllerComp = EntitySystem::GetComp<ControllerComponent>(id);
	return controllerComp->getController();
}