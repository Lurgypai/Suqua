#pragma once
#include "ComponentMacros.h"
#include "IDrawable.h"

#include <vector>

using SpritePtr = std::unique_ptr<IDrawable>;

class RenderComponent {
    CompMembers(RenderComponent);
public:
	RenderComponent(EntityId id_, const Vec2f& offset_ = {0.f, 0.f});
	RenderComponent(const RenderComponent & other);
	RenderComponent(RenderComponent&& other) = default;
	
	RenderComponent & operator=(const RenderComponent & other);

	size_t allocateDrawable();
	
	template<typename T, typename... Args>
	size_t loadDrawable(Args&&... args);

	template<typename T, typename... Args>
	void setDrawable(size_t index, Args&&... args);

	void clearDrawable(size_t index);
	
	template<typename T>
	T& getDrawable(size_t index);

	bool hasDrawable(size_t index);

protected:
    std::vector<SpritePtr> sprites;

	friend class RenderSystem;
};

template<typename T, typename ...Args>
inline size_t RenderComponent::loadDrawable(Args&& ...args) {
    sprites.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    return sprites.size() - 1;
}

template<typename T, typename ... Args>
inline void RenderComponent::setDrawable(size_t index, Args&&... args) {
	sprites[index] = std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
inline T& RenderComponent::getDrawable(size_t index) {
	return *(static_cast<T *>(sprites[index].get()));
}
