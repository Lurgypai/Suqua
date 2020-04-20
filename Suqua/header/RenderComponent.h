#pragma once
#include "EntitySystem.h"
#include "IDrawable.h"
#include "FileNotFoundException.h"
#include "GLRenderer.h"

#include <iostream>

using SpritePtr = std::unique_ptr<IDrawable>;

class RenderComponent {
public:
	Vec2f offset;

	RenderComponent(EntityId id_ = 0);
	RenderComponent(const RenderComponent & other);
	
	RenderComponent & operator=(const RenderComponent & other);
	
	template<typename T, typename... Args>
	void loadDrawable(Args... args);

	template<typename T, typename U>
	void setDrawable(U&& u);
	
	template<typename U>
	void setDrawable(std::unique_ptr<U> u);

	template<typename T>
	T * getDrawable();

	EntityId getId() const;
protected:
	SpritePtr sprite;
	EntityId id;

	friend class RenderSystem;
};

template<typename T, typename ...Args>
inline void RenderComponent::loadDrawable(Args ...args) {
	try {
		sprite = std::make_unique<T>(args...);
	}
	catch (FileNotFoundException e) {
		std::cout << e.what() << '\n';
	}
}

template<typename T, typename U>
inline void RenderComponent::setDrawable(U && u) {
	try {
	sprite = std::make_unique<T>(std::forward<U>(u));
	}
	catch (FileNotFoundException e) {
		std::cout << e.what() << '\n';
	}
}

template<typename T>
inline T* RenderComponent::getDrawable() {
	return static_cast<T *>(sprite.get());
}
