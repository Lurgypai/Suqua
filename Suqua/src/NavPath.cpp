#include "NavPath.h"

NavPath::NavPath(std::list<NavZoneId>&& zones_) :
	zones{ std::move(zones_) },
	currZone {zones.begin()}
{
	
}

NavPath::NavPath(const std::list<NavZoneId>& zones_) :
	zones{zones_},
	currZone{zones.begin()}
{}

const NavZoneId& NavPath::getCurrZoneId() {
	return *currZone;
}

const NavZoneId& NavPath::getNextZoneId() {
	auto tmp = currZone++;
	auto next = currZone;
	currZone = tmp;
	return *next;
}

bool NavPath::finished() {
	return currZone != --zones.end();
}

bool NavPath::empty() {
	return zones.empty();
}

void NavPath::next() {
	++currZone;
}