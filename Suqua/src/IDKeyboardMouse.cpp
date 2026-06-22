#include "IDKeyboardMouse.h"

IDKeyboardMouse::IDKeyboardMouse(InputDeviceId id_) :
	InputDevice{id_}
{}

Controller IDKeyboardMouse::getControllerState() {
	return controller;
}

void IDKeyboardMouse::update() {
	auto mouseState = SDL_GetMouseState(&controller.pointerPos.x, &controller.pointerPos.y);
	const Uint8* state = SDL_GetKeyboardState(NULL);

	controller.set(ControllerBits::UP, state[SDL_SCANCODE_E]);
	controller.set(ControllerBits::DOWN, state[SDL_SCANCODE_D]);
	controller.set(ControllerBits::LEFT, state[SDL_SCANCODE_S]);
	controller.set(ControllerBits::RIGHT, state[SDL_SCANCODE_F]);

	controller.set(ControllerBits::BUTTON_1, state[SDL_SCANCODE_R]);
	controller.set(ControllerBits::BUTTON_2, state[SDL_SCANCODE_W]);
	controller.set(ControllerBits::BUTTON_3, state[SDL_SCANCODE_A]);
	controller.set(ControllerBits::BUTTON_4, state[SDL_SCANCODE_G]);

	controller.set(ControllerBits::BUTTON_5, mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
	controller.set(ControllerBits::BUTTON_6, mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT));

	controller.set(ControllerBits::BUTTON_7, state[SDL_SCANCODE_Z]);
	controller.set(ControllerBits::BUTTON_8, state[SDL_SCANCODE_SPACE]);
	controller.set(ControllerBits::BUTTON_9, state[SDL_SCANCODE_X]);
	controller.set(ControllerBits::BUTTON_10, state[SDL_SCANCODE_V]);

	controller.set(ControllerBits::BUTTON_11, state[SDL_SCANCODE_BACKSPACE]);
	controller.set(ControllerBits::BUTTON_12, state[SDL_SCANCODE_ESCAPE]);

	controller.stick1 = {};
	if (controller[ControllerBits::LEFT]) controller.stick1.x -= 1;
	if (controller[ControllerBits::RIGHT]) controller.stick1.x += 1;
	if (controller[ControllerBits::UP]) controller.stick1.y -= 1;
	if (controller[ControllerBits::DOWN]) controller.stick1.y += 1;
	controller.stick1 = controller.stick1.norm();

	controller.stick2 = { 1, 0 };
	auto mouseWorldPos = GLRenderer::screenToWorld(controller.pointerPos, camera);
	auto delta = mouseWorldPos - entityPos;
	controller.stick2.angle(delta.angle());
}
