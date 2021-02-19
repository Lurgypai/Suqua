#pragma once
#include <list>
#include "NavZone.h"
#include "NavTypes.h"

using NavIterator = std::list<NavZoneId>::iterator;

class NavPath {
public:
	NavPath(std::list<NavZoneId>&& zones);
	NavPath(const std::list<NavZoneId>& zones);
	NavPath() = default;

	const NavZoneId& getCurrZoneId();
	const NavZoneId& getNextZoneId();
	void next();
	//Checks if the currZone is one befor the end() iterator
	bool finished();
	bool empty();
private:
	std::list<NavZoneId> zones;
	NavIterator currZone;
};