#include "DamageComponent.h"

DamageComponent::DamageComponent(EntityId id_) :
	id{ id_ }
{}

uint32_t DamageComponent::getDamage() const {
	return damageCalculator->getDamage();
}