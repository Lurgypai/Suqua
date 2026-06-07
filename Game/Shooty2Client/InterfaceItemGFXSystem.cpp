#include "InterfaceItemGFXSystem.h"

void InterfaceItemGFXSystem::registerGFX(const std::string& itemTag,
        const std::string& smallTag, const std::string& bigTag, const std::string& desc) 
{
    gfx.emplace(itemTag, InterfaceItemGFX{smallTag, bigTag, desc});
}

InterfaceItemGFX InterfaceItemGFXSystem::getGFX(const std::string& itemTag) const {
    auto item = gfx.find(itemTag);
    if(item == gfx.end()) return InterfaceItemGFX{ "none", "none", "Uh Oh!"};
    return item->second;
}

