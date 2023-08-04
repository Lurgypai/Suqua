#pragma once
#include "EntitySystem.h"

#define CompCtor(comp) comp##(EntityId id_ = 0)
#define CompGetId EntityId getId() const { return id; }

#define CompMembers(comp) public: CompCtor(comp); CompGetId; private: EntityId id;

/* for now you still have to manually define the ctor, as it might need to initialize all of your members*/