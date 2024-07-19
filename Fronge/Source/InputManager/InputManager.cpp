#include "froch.hpp"

#include "InputManager.hpp"
#include "SystemEventManager/SystemEventManager.hpp"
#include "Utility/VariantVisitor.hpp"
#include "Maths/MathFunctions.hpp"

namespace fro
{
	bool InputManager::SimulateActionStrengthEvent::operator==(SimulateActionStrengthEvent const& other) const
	{
		return
			actionInfo == other.actionInfo and
			simulatedStrength == other.simulatedStrength;
	}

	void InputManager::initialize()
	{
		SystemEventManager::mInputEvent.addListener(sOnInputEvent);

		Logger::info("initialized InputManager!");
	}

	void InputManager::processInputContinous()
	{
		for (auto& [input, info] : sInputs)
			info.relativeStrength = 0.0;

		sSimulateActionStrengthEvents.processAllEvents();

		for (auto& [name, info] : sActions)
			if (info.simulatedAbsoluteStrength or info.simulatedRelativeStrength)
				sSimulateActionStrengthEvents.pushEvent(SimulateActionStrengthEvent
					{
						.actionInfo{ &info },
						.simulatedStrength{ 0.0 }
					});
	}

	void InputManager::bindActionToInput(std::string const& actionName, Input const input)
	{
		sActions[actionName].boundInputInfos.insert(&sInputs[input]);
	}

	void InputManager::setActionDeadzone(std::string const& actionName, double const deadzone)
	{
		sActions[actionName].deadzone = deadzone;
	}

	void InputManager::simulateActionStrength(std::string const& actionName, double const strength)
	{
		ActionInfo& actionInfo{ sActions[actionName] };

		SimulateActionStrengthEvent event
		{
			.actionInfo{ &actionInfo },
			.simulatedStrength{ strength }
		};

		sSimulateActionStrengthEvents.overridePushIf(
			[&actionInfo](SimulateActionStrengthEvent const& queuedEvent)
			{
				return
					queuedEvent.actionInfo == &actionInfo and
					queuedEvent.simulatedStrength == 0.0;
			}, std::move(event));
	}

	double InputManager::getInputStrength(Input const input)
	{
		return sInputs[input].absoluteStrength;
	}

	double InputManager::getActionStrength(std::string const& actionName)
	{
		return getActionStrength(actionName, sActions[actionName].deadzone);
	}

	double InputManager::getActionStrengthAxis1D(std::string const& positiveActionName, std::string const& negativeActionName)
	{
		return getActionStrength(positiveActionName) - getActionStrength(negativeActionName);
	}

	Vector2<double> InputManager::getActionStrengthAxis2D(
		std::string const& positiveActionNameX,
		std::string const& negativeActionNameX,
		std::string const& positiveActionNameY,
		std::string const& negativeActionNameY)
	{
		Vector2<double> const actionAxis2D
		{
			getActionStrength(positiveActionNameX, 0.0) - getActionStrength(negativeActionNameX, 0.0),
			getActionStrength(positiveActionNameY, 0.0) - getActionStrength(negativeActionNameY, 0.0)
		};

		std::array<double, 4> const actionDeadzones
		{
			sActions[positiveActionNameX].deadzone,
			sActions[negativeActionNameX].deadzone,
			sActions[positiveActionNameY].deadzone,
			sActions[negativeActionNameY].deadzone
		};

		double const avarageDeadzoneMagnitude
		{
			std::reduce(actionDeadzones.begin(), actionDeadzones.end(), 0.0) / actionDeadzones.size()
		};

		double const actionAxis2DMagnitude{ math::getMagnitude(actionAxis2D) };

		if (actionAxis2DMagnitude <= avarageDeadzoneMagnitude)
			return {};

		if (actionAxis2DMagnitude > 1.0)
			return actionAxis2D / actionAxis2DMagnitude;

		double const inverseLerpedLength{ (actionAxis2DMagnitude - avarageDeadzoneMagnitude) / (1.0 - avarageDeadzoneMagnitude) };
		return actionAxis2D * (inverseLerpedLength / actionAxis2DMagnitude);
	}

	bool InputManager::isInputJustPressed(Input const input)
	{
		auto const& [absoluteStrength, relativeStrength] { sInputs[input] };

		auto const [isOverDeadzone, didJustCrossDeadzone] { getStrengthInfo(absoluteStrength, relativeStrength, 0.0) };
		return isOverDeadzone and didJustCrossDeadzone;
	}

	bool InputManager::isActionJustPressed(std::string const& actionName)
	{
		auto const& [boundInputInfos, simulatedAbsoluteStrength, simulatedRelativeStrength, deadzone]
			{
				sActions[actionName]
			};

		bool justCrossedDeadzoneInputPresent{};
		for (InputInfo const* const boundInputInfo : boundInputInfos)
		{
			auto const [isOverDeadzone, didJustCrossDeadzone]
				{
					getStrengthInfo(boundInputInfo->absoluteStrength, boundInputInfo->relativeStrength, deadzone)
				};

			if (isOverDeadzone)
				if (didJustCrossDeadzone)
					justCrossedDeadzoneInputPresent = true;
				else
					return false;
		}

		auto const [isOverDeadzone, didJustCrossDeadzone]
			{
				getStrengthInfo(simulatedAbsoluteStrength, simulatedRelativeStrength, deadzone)
			};

		if (isOverDeadzone)
			if (didJustCrossDeadzone)
				justCrossedDeadzoneInputPresent = true;
			else
				return false;

		return justCrossedDeadzoneInputPresent;
	}

	bool InputManager::isInputJustReleased(Input const input)
	{
		auto const& [absoluteStrength, relativeStrength] { sInputs[input] };

		auto const [isOverDeadzone, didJustCrossDeadzone] { getStrengthInfo(absoluteStrength, relativeStrength, 0.0) };
		return not isOverDeadzone and didJustCrossDeadzone;
	}

	bool InputManager::isActionJustReleased(std::string const& actionName)
	{
		auto const& [boundInputInfos, simulatedAbsoluteStrength, simulatedRelativeStrength, deadzone]
			{
				sActions[actionName]
			};

		bool justCrossedDeadzoneInputPresent{};
		for (InputInfo const* const boundInputInfo : boundInputInfos)
		{
			auto const [isOverDeadzone, didJustCrossDeadzone]
				{
					getStrengthInfo(boundInputInfo->absoluteStrength, boundInputInfo->relativeStrength, deadzone)
				};

			if (isOverDeadzone)
				return false;

			if (didJustCrossDeadzone)
				justCrossedDeadzoneInputPresent = true;
		}

		auto const [isOverDeadzone, didJustCrossDeadzone]
			{
				getStrengthInfo(simulatedAbsoluteStrength, simulatedRelativeStrength, deadzone)
			};

		if (isOverDeadzone)
			return false;

		if (didJustCrossDeadzone)
			justCrossedDeadzoneInputPresent = true;

		return justCrossedDeadzoneInputPresent;
	}

	void InputManager::setInputStrength(Input const input, double const newStrength)
	{
		auto& [absoluteStrength, relativeStrength] { sInputs[input] };

		relativeStrength = newStrength - absoluteStrength;
		absoluteStrength = newStrength;
	}

	std::pair<bool, bool> InputManager::getStrengthInfo(
		double const absoluteStrength,
		double const relativeStrength,
		double const deadzone)
	{
		bool const isOverDeadzone{ absoluteStrength > deadzone };
		double const previousAbsoluteStrength{ absoluteStrength - relativeStrength };

		return
		{
			isOverDeadzone,
			isOverDeadzone ?
			previousAbsoluteStrength <= deadzone :
			previousAbsoluteStrength > deadzone
		};
	}

	double InputManager::getLargestStrength(double const strength, double const largestStrength, double const deadzone)
	{
		double const deadzonedStrength
		{
			strength <= deadzone ?
			0.0 :
			(strength - deadzone) / (1.0 - deadzone)
		};

		return std::max(deadzonedStrength, largestStrength);
	}

	double InputManager::getActionStrength(std::string const& actionName, double const deadzone)
	{
		double largestStrength{};
		ActionInfo const& actionInfo{ sActions[actionName] };

		largestStrength = getLargestStrength(actionInfo.simulatedAbsoluteStrength, largestStrength, deadzone);
		if (largestStrength == 1.0)
			return largestStrength;

		for (InputInfo const* const boundInputInfo : sActions[actionName].boundInputInfos)
		{
			largestStrength = getLargestStrength(boundInputInfo->absoluteStrength, largestStrength, deadzone);
			if (largestStrength == 1.0)
				return largestStrength;
		}

		return largestStrength;
	}

	EventListener<InputEvent const> InputManager::sOnInputEvent
	{
		VariantVisitor
		{
			[](MouseButtonDownEvent const& event)
			{
				setInputStrength(event.button, 1.0);
				return true;
			},

			[](MouseButtonUpEvent const& event)
			{
				setInputStrength(event.button, 0.0);
				return true;
			},

			[](KeyDownEvent const& event)
			{
				setInputStrength(event.key, 1.0);
				return true;
			},

			[](KeyUpEvent const& event)
			{
				setInputStrength(event.key, 0.0);
				return true;
			},

			[](auto&&)
			{
				return false;
			}
		}
	};

	UniqueEventQueue<InputManager::SimulateActionStrengthEvent> InputManager::sSimulateActionStrengthEvents
	{
		[](SimulateActionStrengthEvent&& event)
		{
			auto const [actionInfo, simulatedStrength] { event };

			actionInfo->simulatedRelativeStrength = simulatedStrength - actionInfo->simulatedAbsoluteStrength;
			actionInfo->simulatedAbsoluteStrength = simulatedStrength;
		}
	};

	std::map<Input, InputManager::InputInfo> InputManager::sInputs{};
	std::map<std::string, InputManager::ActionInfo> InputManager::sActions{};
}