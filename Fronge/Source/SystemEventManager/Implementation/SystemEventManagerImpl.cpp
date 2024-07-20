#include "froch.hpp"

#include "Input/Implementation/InputImpl.hpp"
#include "SystemEventManagerImpl.hpp"
#include "Logger/Logger.hpp"

#include <SDL.h>

namespace fro
{
	void SystemEventManager::Implementation::dispatchSDLWindowEvent(SDL_WindowEvent const& SDLEvent)
	{
		switch (SDLEvent.event)
		{
		case SDL_WINDOWEVENT_CLOSE:
		{
			WindowCloseEvent event{ SDLEvent.windowID };
			Logger::info("dispatched {}!", event.getLogString());

			mWindowEvent.notify(std::move(event));
			break;
		}
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		{
			WindowResizeEvent event
			{
				.ID{ SDLEvent.windowID },
				.size{ SDLEvent.data1, SDLEvent.data2 }
			};

			Logger::info("dispatched {}!", event.getLogString());

			mWindowEvent.notify(std::move(event));
			break;
		}
		}
	}

	void SystemEventManager::Implementation::dispatchSDLKeyboardEvent(SDL_KeyboardEvent const& SDLEvent)
	{
		switch (SDLEvent.type)
		{
		case SDL_KEYDOWN:
		{
			KeyDownEvent event{ convertSDLKeyCode(SDLEvent.keysym.sym) };

			mInputEvent.notify(std::move(event));
			break;
		}
		case SDL_KEYUP:
		{
			KeyUpEvent event{ convertSDLKeyCode(SDLEvent.keysym.sym) };

			mInputEvent.notify(std::move(event));
			break;
		}
		}
	}

	void SystemEventManager::Implementation::dispatchSDLControllerDeviceEvent(SDL_ControllerDeviceEvent const& SDLEvent)
	{
		switch (SDLEvent.type)
		{
		case SDL_CONTROLLERDEVICEADDED:
		{
			GamepadConnectedEvent event{ SDLEvent.which };
			Logger::info("dispatched {}!", event.getLogString());

			mInputEvent.notify(std::move(event));
			break;
		}
		case SDL_CONTROLLERDEVICEREMOVED:
		{
			GamepadDisconnectedEvent event{ SDLEvent.which };
			Logger::info("dispatched {}!", event.getLogString());

			mInputEvent.notify(std::move(event));
			break;
		}
		}
	}

	void SystemEventManager::Implementation::dispatchSDLControllerButtonEvent(SDL_ControllerButtonEvent const& SDLEvent)
	{
		switch (SDLEvent.type)
		{
		case SDL_CONTROLLERBUTTONDOWN:
		{
			GamepadButtonDownEvent event{ { SDLEvent.which, convertSDLControllerButton(SDLEvent.button) } };

			mInputEvent.notify(std::move(event));
			break;
		}
		case SDL_CONTROLLERBUTTONUP:
		{
			GamepadButtonUpEvent event{ { SDLEvent.which, convertSDLControllerButton(SDLEvent.button) } };

			mInputEvent.notify(std::move(event));
			break;
		}
		}
	}

	void SystemEventManager::Implementation::dispatchSDLControllerAxisEvent(SDL_ControllerAxisEvent const& SDLEvent)
	{
		resetOppositeAxis(SDLEvent);

		auto&& [input, value] { convertSDLControllerAxis(SDLEvent.axis, SDLEvent.value) };

		GamepadAxisEvent event
		{
			.input{ SDLEvent.which, input },
			.value{ value }
		};

		mInputEvent.notify(std::move(event));
	}

	// SDL treats horizontal and veritcal gamepad sticks as one value,
	// Fronge does not. This means that if a stick goes from e.g. 128 to -128,
	// Fronge is not notified that the opposite axis now should be 0. This fixes that.
	void SystemEventManager::Implementation::resetOppositeAxis(SDL_ControllerAxisEvent const& SDLEvent)
	{
		Sint16* previousValue;
		switch (SDLEvent.axis)
		{
		case SDL_CONTROLLER_AXIS_RIGHTX:
			previousValue = &sPreviousRightStickX;
			break;

		case SDL_CONTROLLER_AXIS_RIGHTY:
			previousValue = &sPreviousRightStickY;
			break;

		case SDL_CONTROLLER_AXIS_LEFTX:
			previousValue = &sPreviousLeftStickX;
			break;

		case SDL_CONTROLLER_AXIS_LEFTY:
			previousValue = &sPreviousLeftStickY;
			break;

		default:
			return;
		}

		if (*previousValue < 0 and SDLEvent.value > 0 or
			*previousValue > 0 and SDLEvent.value < 0)
		{
			auto&& [oppositeInput, _] { convertSDLControllerAxis(SDLEvent.axis, SDLEvent.value < 0 ? 1 : -1) };

			GamepadAxisEvent event
			{
				.input{ SDLEvent.which, oppositeInput },
				.value{}
			};

			mInputEvent.notify(std::move(event));
		};

		*previousValue = SDLEvent.value;
	}

	Sint16 SystemEventManager::Implementation::sPreviousRightStickX{};
	Sint16 SystemEventManager::Implementation::sPreviousRightStickY{};
	Sint16 SystemEventManager::Implementation::sPreviousLeftStickX{};
	Sint16 SystemEventManager::Implementation::sPreviousLeftStickY{};

	void SystemEventManager::initialize()
	{
		SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

		Logger::info("initialized SystemEventManager!");
	}

	void SystemEventManager::shutDown()
	{
		SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);

		Logger::info("shut down SystemEventManager!");
	}

	void SystemEventManager::pollEvents()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				Implementation::dispatchSDLWindowEvent(event.window);
				break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				// TODO: no support for repeated key down events
				if (event.key.repeat == 0)
					Implementation::dispatchSDLKeyboardEvent(event.key);
				break;

			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
				Implementation::dispatchSDLControllerDeviceEvent(event.cdevice);
				break;

			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
				Implementation::dispatchSDLControllerButtonEvent(event.cbutton);
				break;

			case SDL_CONTROLLERAXISMOTION:
				Implementation::dispatchSDLControllerAxisEvent(event.caxis);
				break;
			}
	}

	EventDispatcher<WindowEvent const> SystemEventManager::mWindowEvent{};
	EventDispatcher<InputEvent const> SystemEventManager::mInputEvent{};
}