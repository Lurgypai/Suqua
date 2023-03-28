#pragma once
#include "EntitySystem.h"
#include "NetworkDataComponent.h"
#include "ControllerComponent.h"
#include "DirectionData.h"

#include <iostream>

class PlayerComponent {
public:
	enum PlayerVars : NetworkDataComponent::DataId {
		playerStateVarId = 30,
	};

	enum class State : uint32_t {
		idle,
		walking
	};

	PlayerComponent(EntityId id_ = 0);
	EntityId getId() const;

	void update();

	inline State getPlayerState() const;
	inline float getPlayerDir() const;
private:
	EntityId id;

	inline void setPlayerState(State newPlayerState);
	inline void setPlayerDir(float newPlayerDir) const;

	inline const Controller& getController() const;

	void beginIdle();
	void idle_();
	void beginWalking();
	void walking_();
};

inline PlayerComponent::State PlayerComponent::getPlayerState() const
{
	auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	return static_cast<State>(ndc->get<uint32_t>(playerStateVarId));
}

inline float PlayerComponent::getPlayerDir() const {
	auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	return ndc->get<float>(DIR);
}

inline void PlayerComponent::setPlayerState(State newPlayerState) {
	auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	ndc->get<uint32_t>(playerStateVarId) = static_cast<uint32_t>(newPlayerState);
}

inline void PlayerComponent::setPlayerDir(float newPlayerDir) const {
	auto ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
	ndc->get<float>(DIR) = newPlayerDir;
}

inline const Controller& PlayerComponent::getController() const {
	auto controllerComp = EntitySystem::GetComp<ControllerComponent>(id);
	return controllerComp->getController();
}