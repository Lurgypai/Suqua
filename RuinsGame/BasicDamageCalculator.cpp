#include "BasicDamageCalculator.h"

BasicDamageCalculator::BasicDamageCalculator(uint32_t damage_) :
	damage{ damage_ }
{}

uint32_t BasicDamageCalculator::getDamage() const
{
	return damage;
}

