#if not defined fro_INPUT_MANAGER_H
#define fro_INPUT_MANAGER_H

#include "Defines.hpp"
#include "EventQueue.hpp"
#include "Typenames.hpp"
#include "Singleton.hpp"

#include <glm.hpp>
#include <sdl_scancode.h>
#include <sdl_gamecontroller.h>

#include <map>
#include <set>
#include <variant>
#include <vector>
#include <xstring>

union SDL_Event;

namespace fro
{
	class InputManager final : public Singleton<InputManager>
	{
		friend class Singleton<InputManager>;

	public:
		enum class MouseButton
		{
			left = 1,
			middle = 2,
			right = 3
		};

		template<typename InputType>
		struct JoypadInput
		{
			JoypadInput(Sint32 deviceID, InputType input)
				: deviceID{ deviceID }
				, input{ input }
			{
			}

			Sint32 deviceID;
			InputType input;

			fro_NODISCARD bool operator<(JoypadInput const& other) const
			{
				return
					deviceID == other.deviceID ?
					input < other.input :
					deviceID < other.deviceID;
			}
		};

		enum class JoypadAxis
		{
			leftStickLeft,
			leftStickRight,
			leftStickUp,
			leftStickDown,
			rightStickLeft,
			rightStickRight,
			rightStickUp,
			rightStickDown,
			leftTrigger,
			rightTrigger
		};

		using Input = std::variant<SDL_Scancode, MouseButton, JoypadInput<SDL_GameControllerButton>, JoypadInput<JoypadAxis>>;

		virtual ~InputManager() override;

		void processInputContinous();
		void processInputEvent(SDL_Event const& event);
		void bindActionToInput(std::string const& actionName, Input const input);

		void setActionDeadzone(std::string const& actionName, float const deadzone);

		void simulateActionStrength(std::string const& actionName, float const strength);

		fro_NODISCARD float getInputStrength(Input const input);
		fro_NODISCARD float getActionStrength(std::string const& actionName);
		fro_NODISCARD float getActionStrengthAxis1D(std::string const& positiveActionName, std::string const& negativeActionName);
		fro_NODISCARD glm::vec2 getActionStrengthAxis2D(std::string const& positiveActionNameX,
			std::string const& negativeActionNameX,
			std::string const& positiveActionNameY,
			std::string const& negativeActionNameY);
		fro_NODISCARD bool isInputJustPressed(Input const input);
		fro_NODISCARD bool isActionJustPressed(std::string const& actionName);
		fro_NODISCARD bool isInputJustReleased(Input const input);
		fro_NODISCARD bool isActionJustReleased(std::string const& actionName);

	private:
		struct InputInfo final
		{
			float absoluteStrength{};
			float relativeStrength{};
		};

		struct ActionInfo final
		{
			std::set<InputInfo const*> spBoundInputInfos{};

			float simulatedAbsoluteStrength{};
			float simulatedRelativeStrength{};

			float deadzone{ 0.25f };
		};

		struct SimulateActionStrengthEvent final
		{
			bool operator==(SimulateActionStrengthEvent const& other) const;

			ActionInfo* pActionInfo{};
			float simulatedStrength{};
		};

		static fro_NODISCARD JoypadAxis SDLToJoypadTrigger(Uint8 const SDLAxis);
		static fro_NODISCARD JoypadAxis SDLToJoypadStick(Sint16 const stickValue, Uint8 const SDLAxis);
		static fro_NODISCARD std::pair<bool, bool> getStrengthInfo(float const absoluteStrength, float const relativeStrength, float const deadzone);
		static fro_NODISCARD float getLargestStrength(float const strength, float const largestStrength, float const deadzone);

		InputManager();
		InputManager(InputManager const&) = delete;
		InputManager(InputManager&&) noexcept = delete;

		InputManager& operator=(InputManager const&) = delete;
		InputManager& operator=(InputManager&&) noexcept = delete;

		void setInputState(float const newStrength, Input const input);

		fro_NODISCARD float getActionStrength(std::string const& actionName, float const deadzone);

		EventQueue<SimulateActionStrengthEvent, true> m_SimulateActionStrengthEvents
		{
			[](SimulateActionStrengthEvent&& event)
			{
				auto const [pActionInfo, simulatedStrength] { event };

				pActionInfo->simulatedRelativeStrength = simulatedStrength - pActionInfo->simulatedAbsoluteStrength;
				pActionInfo->simulatedAbsoluteStrength = simulatedStrength;
			}
		};

		// TODO: only reason for this is to open and close connected and disconnected joypads; not sure if this is needed
		std::map<SDL_JoystickID, CustomUniquePointer<SDL_GameController>> m_mpJoypads{};
		// END TODO
		std::map<SDL_JoystickID, Sint32> m_mpJoypadInstanceDeviceIDs{};
		std::map<Input, InputInfo> m_mInputs{};
		std::map<std::string, ActionInfo> m_mActions{};
	};
}

#endif