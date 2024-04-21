#include <iostream>

#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Text.h"
#include "FPSCounter.h"
#include "Transform.h"
#include "Sprite.h"
#include "InputManager.h"
#include "MoveCommand.h"
#include "SystemEventManager.h"
#include "GUIContext.h"
#include "Steam.h"
#include "Timer.h"
#include "RenderContext.h"
#include "Fronge.h"

#include <SDL_main.h>

int main(int, char**)
{
	constexpr bool loadFPSCounter{ true };

	fro::Scene& scene{ fro::SceneManager::getInstance().addScene("Test") };

	[[maybe_unused]] fro::GameObject* pGameObject;

	if constexpr (loadFPSCounter)
	{
		pGameObject = &scene.addGameObject();
		fro::Text& text{ *pGameObject->addComponent<fro::Text>() };
		text.setFont("Lingua.otf", 64);
		pGameObject->addComponent<fro::FPSCounter>();
		pGameObject->getComponent<fro::Transform>()->setLocalPosition({ 320, 30 });
	}

	fro::GameObject& player1{ scene.addGameObject() };
	player1.addComponent<fro::Sprite>()->setFileName("DigDug.png");
	player1.getComponent<fro::Transform>()->setLocalPosition({ 300, 300 });

	fro::InputManager::getInstance().bindKeyInputToAction({ SDL_SCANCODE_D, fro::ButtonInput::State::down }, "moveRight");
	fro::InputManager::getInstance().bindKeyInputToAction({ SDL_SCANCODE_A, fro::ButtonInput::State::down }, "moveLeft");
	fro::InputManager::getInstance().bindKeyInputToAction({ SDL_SCANCODE_W, fro::ButtonInput::State::down }, "moveUp");
	fro::InputManager::getInstance().bindKeyInputToAction({ SDL_SCANCODE_S, fro::ButtonInput::State::down }, "moveDown");

	fro::InputManager::getInstance().bindActionToCommand<fro::MoveCommand>("moveRight", player1).setMoveDirection({ 1.0f, 0.0f });
	fro::InputManager::getInstance().bindActionToCommand<fro::MoveCommand>("moveLeft", player1).setMoveDirection({ -1.0f, 0.0f });
	fro::InputManager::getInstance().bindActionToCommand<fro::MoveCommand>("moveUp", player1).setMoveDirection({ 0.0f, -1.0f });
	fro::InputManager::getInstance().bindActionToCommand<fro::MoveCommand>("moveDown", player1).setMoveDirection({ 0.0f, 1.0f });

	fro::GameObject& player2{ scene.addGameObject() };
	player2.addComponent<fro::Sprite>()->setFileName("DigDug.png");
	player2.getComponent<fro::Transform>()->setLocalPosition({ 200, 300 });

	fro::InputManager::getInstance().bindKeyInputToAction({ SDL_CONTROLLER_BUTTON_DPAD_RIGHT, fro::ButtonInput::State::down }, "moveRightController");
	fro::InputManager::getInstance().bindKeyInputToAction({ SDL_CONTROLLER_BUTTON_DPAD_LEFT, fro::ButtonInput::State::down }, "moveLeftController");
	fro::InputManager::getInstance().bindKeyInputToAction({ SDL_CONTROLLER_BUTTON_DPAD_UP, fro::ButtonInput::State::down }, "moveUpController");
	fro::InputManager::getInstance().bindKeyInputToAction({ SDL_CONTROLLER_BUTTON_DPAD_DOWN, fro::ButtonInput::State::down }, "moveDownController");

	fro::InputManager::getInstance().bindActionToCommand<fro::MoveCommand>("moveRightController", player2).setMoveDirection({ 1.0f, 0.0f });
	fro::InputManager::getInstance().bindActionToCommand<fro::MoveCommand>("moveLeftController", player2).setMoveDirection({ -1.0f, 0.0f });
	fro::InputManager::getInstance().bindActionToCommand<fro::MoveCommand>("moveUpController", player2).setMoveDirection({ 0.0f, -1.0f });
	fro::InputManager::getInstance().bindActionToCommand<fro::MoveCommand>("moveDownController", player2).setMoveDirection({ 0.0f, 1.0f });

	return fro::Fronge::getInstance().run();
}