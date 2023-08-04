#pragma once

#include <memory>

#include "ComponentMacros.h"
#include "DamageCalculator.h"

class DamageComponent {
	CompMembers(DamageComponent);

public:
	uint32_t getDamage() const;
	template<typename T, typename... Args>
	void setDamageCalculator(Args... args);

	template<typename T>
	const T& getDamageCalculator() const;

	template <typename T>
	T& getDamageCalculator();
private:
	std::unique_ptr<DamageCalculator> damageCalculator;
};

template <typename T, typename... Args>
void DamageComponent::setDamageCalculator(Args... args) {
	damageCalculator = std::make_unique<T>(args...);
}

template<typename T>
const T& DamageComponent::getDamageCalculator() const {
	return *damageCalculator;
}


template<typename T>
T& DamageComponent::getDamageCalculator() {
	return *damageCalculator;
}