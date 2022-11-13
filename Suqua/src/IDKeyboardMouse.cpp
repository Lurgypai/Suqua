#include "IDKeyboardMouse.h"
#include "SDL.h"

IDKeyboardMouse::IDKeyboardMouse(InputDeviceId id_) :
	InputDevice{id_}
{}

Controller IDKeyboardMouse::getControllerState() {
	auto mouseState = SDL_GetMouseState(&controller.pointerPos.x, &controller.pointerPos.y);

	controller.set(ControllerBits::BUTTON_11, mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
	controller.set(ControllerBits::BUTTON_12, mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT));

	const Uint8* state = SDL_GetKeyboardState(NULL);

	controller.set(ControllerBits::UP, state[SDL_SCANCODE_W]);
	controller.set(ControllerBits::DOWN, state[SDL_SCANCODE_S]);
	controller.set(ControllerBits::LEFT, state[SDL_SCANCODE_A]);
	controller.set(ControllerBits::RIGHT, state[SDL_SCANCODE_D]);
	controller.set(ControllerBits::BUTTON_1, state[SDL_SCANCODE_H]);
	controller.set(ControllerBits::BUTTON_2, state[SDL_SCANCODE_J]);
	controller.set(ControllerBits::BUTTON_3, state[SDL_SCANCODE_K]);
	controller.set(ControllerBits::BUTTON_4, state[SDL_SCANCODE_L]);

	controller.set(ControllerBits::BUTTON_5, state[SDL_SCANCODE_SPACE]);
	controller.set(ControllerBits::BUTTON_6, state[SDL_SCANCODE_LSHIFT]);
	controller.set(ControllerBits::BUTTON_7, state[SDL_SCANCODE_LCTRL]);

	return controller;
}
