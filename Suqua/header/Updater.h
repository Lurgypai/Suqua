#pragma once
#include "EntitySystem.h"
#include "NetworkDataComponent.h"

namespace Updater {
	//template<typename ComponentType>
	//void UpdateAll() {
	//	for (auto& comp : EntitySystem::GetPool<ComponentType>()) {
	//		comp.update();
	//	}
	//}

	template<typename ComponentType, typename... Args>
	void UpdateAll(Args&&... args) {
		if (!EntitySystem::Contains<ComponentType>()) return;

		for (auto& comp : EntitySystem::GetPool<ComponentType>()) {
			comp.update(std::forward<Args>(args)...);
		}
	}

	template<typename ComponentType, typename... Args>
	void UpdateOwned(Args&&... args) {
		if (!EntitySystem::Contains<ComponentType>()) return;

		for (auto& comp : EntitySystem::GetPool<ComponentType>()) {
			auto ndc = EntitySystem::GetComp<NetworkDataComponent>(comp.getId());
			if (ndc == nullptr || (ndc->owner != NetworkDataComponent::Owner::local_only && ndc->owner != NetworkDataComponent::Owner::local_shared)) continue;
			comp.update(std::forward<Args>(args)...);
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
