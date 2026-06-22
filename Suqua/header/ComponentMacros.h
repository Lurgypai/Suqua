#pragma once
#include "EntitySystem.h"

//defines the default and constructor
#define CompCtor(comp) comp() = default
// #define CompCtor2(comp) comp(EntityId id_) : id{id_} {};
#define CompGetId EntityId getId() const { return id; }

#define CompMembers(comp) public: CompCtor(comp); CompGetId; private: EntityId id;

/* Users are required to implement a constructor that takes at least an entityId, possibly more */

