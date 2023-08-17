#pragma once
#include "EntitySystem.h"
#include "EntityBaseComponent.h"
#include "NetworkOwnerComponent.h"

namespace Updater {
	//template<typename ComponentType>
	//void UpdateAll() {
	//	for (auto& comp : EntitySystem::GetPool<ComponentType>()) {
	//		comp.update();
	//	}
	//}

	template<typename ComponentType, typename... Args>
	void UpdateAll(Args... args) {
		if (!EntitySystem::Contains<ComponentType>()) return;

		for (auto& comp : EntitySystem::GetPool<ComponentType>()) {
			comp.update(args...);
		}
	}

	template<typename ComponentType, typename... Args>
	void UpdateOwned(Args... args) {
		if (!EntitySystem::Contains<ComponentType>()) return;

		for (auto& comp : EntitySystem::GetPool<ComponentType>()) {
			auto onlineComp = EntitySystem::GetComp<NetworkOwnerComponent>(comp.getId());
			if (onlineComp == nullptr || onlineComp->owner != NetworkOwnerComponent::Owner::local) continue;
			comp.update(args...);
		}
	}
}

/*
* Add server world scene
* add update owned
* 
* network test
*	joining
*		request to spawn a player, returns online id to be assigned
*		server tells us about other players
*/