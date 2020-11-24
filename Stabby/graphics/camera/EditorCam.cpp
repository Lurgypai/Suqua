
#include "EditorCam.h"
#include "SDL.h"
#include "GLRenderer.h"
#include "DebugIO.h"

EditorCam::EditorCam() :
	id{0}
{}

EditorCam::EditorCam(int id_) :
	id{id_}
{
	Camera& cam = GLRenderer::getCamera(id);
	staticBox.res = Vec2f{ static_cast<float>(cam.res.x) - 50, static_cast<float>(cam.res.y) - 50 };
	staticBox.center(cam.center());
}

void EditorCam::update() {
	staticBox.center(GLRenderer::getCamera(id).center());

	Vec2i mousePos_{};
	SDL_GetMouseState(&mousePos_.x, &mousePos_.y);
	Vec2f mousePos = mousePos_;
	mousePos = GLRenderer::screenToWorld(mousePos, id);

	Vec2f newPos = staticBox.center();
	float speed = 5.0f;

	if (!staticBox.contains(mousePos)) {
		if (mousePos.x < staticBox.pos.x) {
			//left
			newPos.x -= speed;
		}
		else if (mousePos.x > staticBox.pos.x + staticBox.res.x) {
			//right
			newPos.x += speed;
		}
		if (mousePos.y < staticBox.pos.y) {
			//above
			newPos.y -= speed;
		}
		else if (mousePos.y > staticBox.pos.y + staticBox.res.y) {
			//below
			newPos.y += speed;
		}
	}

	staticBox.center(newPos);
	GLRenderer::getCamera(id).center(newPos);
}
