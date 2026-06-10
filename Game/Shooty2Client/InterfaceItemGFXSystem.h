#pragma once
#include <string>
#include <unordered_map>

#include "../Shooty2Core/ItemSystem.h"

struct InterfaceItemGFX {
    // the small ui sprite
    std::string smallTag;
    // the full image in menus
    std::string bigTag;

    std::string desc;
};

class InterfaceItemGFXSystem {
public:
    void loadGfx(const ItemSystem& items);
    InterfaceItemGFX getGFX(const std::string& itemTag) const;
private:
    std::unordered_map<std::string, InterfaceItemGFX> gfx;
};
