#include "Fronge.h"

#include "EventManager.h"
#include "SceneManager.h"
#include "GUI.h"
#include "ResourceManager.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Sprite.h"
#include "Rotator.h"
#include "Text.h"
#include "FPSCounter.h"
#include "Plot.h"
#include "InputManager.h"
#include "MoveCommand.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <VLD/vld.h>
#include <cassert>

#pragma region EntryFunction
int main(int, char**)
{
	int result;

	result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	assert(result == 0 && SDL_GetError());

	constexpr int imageFlags{ NULL };
	result = IMG_Init(imageFlags);
	assert(result == imageFlags);

	result = TTF_Init();
	assert(result == 0);

	fro::Fronge().run();

	fro::ResourceManager::clearCaches();

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}
#pragma endregion EntryFunction



#pragma region Constructors/Destructor
fro::Fronge::Fronge()
	: m_pWindow{ SDL_CreateWindow("Fronge", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, NULL), SDL_DestroyWindow }
	, m_pRenderer{ SDL_CreateRenderer(m_pWindow.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), SDL_DestroyRenderer }
{
	assert(m_pWindow.get() && SDL_GetError());
	assert(m_pRenderer.get() && SDL_GetError());

	fro::GUI::initialise(m_pWindow.get(), m_pRenderer.get());
}

fro::Fronge::~Fronge()
{
	fro::GUI::destroy();
}
#pragma endregion Constructors/Destructor



#pragma region PublicMethods
void fro::Fronge::run()
{
	constexpr bool loadSpiral{ true };
	constexpr bool loadFPSCounter{ true };

	Scene& scene{ SceneManager::addScene("Test") };

	[[maybe_unused]] GameObject* pGameObject;

	if constexpr (loadSpiral)
	{
		pGameObject = &scene.addGameObject();
		pGameObject->addComponent<Sprite>()->setFileName("logo.tga");
		pGameObject->getComponent<Transform>()->setLocalPosition({ 320, 240 });

		for (size_t index{}; index < 20'000; ++index)
		{
			constexpr float basePeriod{ 0.2f };
			constexpr float childPeriodDelayMultiplier{ 0.0002f };

			GameObject& gameObject{ scene.addGameObject() };
			gameObject.addComponent<Sprite>()->setFileName("logo.tga");
			Rotator* const pRotator{ gameObject.addComponent<Rotator>() };
			pRotator->setPeriod(basePeriod + index * childPeriodDelayMultiplier);
			pRotator->setRadius(0.2f);

			gameObject.setParent(pGameObject, false);
			pGameObject = &gameObject;
		}
	}

	if constexpr (loadFPSCounter)
	{
		pGameObject = &scene.addGameObject();
		Text& text{ *pGameObject->addComponent<Text>() };
		text.setFont("Lingua.otf", 64);
		pGameObject->addComponent<FPSCounter>();
		pGameObject->getComponent<Transform>()->setLocalPosition({ 320, 30 });
	}

	InputManager::bindKeyInputToAction({ SDL_SCANCODE_D, KeyInput::State::down }, "moveRight");
	InputManager::bindKeyInputToAction({ SDL_SCANCODE_RIGHT, KeyInput::State::down }, "moveRight");
	InputManager::bindKeyInputToAction({ SDL_SCANCODE_A, KeyInput::State::down }, "moveLeft");
	InputManager::bindKeyInputToAction({ SDL_SCANCODE_W, KeyInput::State::down }, "moveUp");
	InputManager::bindKeyInputToAction({ SDL_SCANCODE_S, KeyInput::State::down }, "moveDown");

	InputManager::bindActionToCommand<MoveCommand>("moveRight", *pGameObject).setMoveDirection({ 1.0f, 0.0f });
	InputManager::bindActionToCommand<MoveCommand>("moveLeft", *pGameObject).setMoveDirection({ -1.0f, 0.0f });
	InputManager::bindActionToCommand<MoveCommand>("moveUp", *pGameObject).setMoveDirection({ 0.0f, -1.0f });
	InputManager::bindActionToCommand<MoveCommand>("moveDown", *pGameObject).setMoveDirection({ 0.0f, 1.0f });

	while (true)
	{
		Timer::update();

		if (!EventManager::processEvents())
			break;

		SceneManager::update();
		SceneManager::render(m_pRenderer.get());
	}
}
#pragma endregion PublicMethods