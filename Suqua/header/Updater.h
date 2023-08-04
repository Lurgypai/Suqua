#pragma once
#include "EntitySystem.h"
#include "EntityBaseComponent.h"

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
}