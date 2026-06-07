#pragma once
#include <string>
#include <unordered_map>

struct InterfaceItemGFX {
    // the small ui sprite
    std::string smallTag;
    // the full image in menus
    std::string bigTag;

    std::string desc;
};

class InterfaceItemGFXSystem {
public:
    void registerGFX(const std::string& itemTag, const std::string& smallTag, const std::string& bigTag, const std::string& desc);
    InterfaceItemGFX getGFX(const std::string& itemTag) const;
private:
    std::unordered_map<std::string, InterfaceItemGFX> gfx;
};
