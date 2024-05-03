#include "Rotator.h"

#include "GameObject.h"
#include "Timer.h"
#include "Transform.h"

#include <glm.hpp>
#include <gtc/constants.hpp>

#pragma region Constructors/Destructor
fro::Rotator::Rotator(GameObject const& parentingGameObject)
	: Behaviour(parentingGameObject)
{
}
#pragma endregion Constructors/Destructor



#pragma region PublicMethods
void fro::Rotator::update()
{
	float constexpr twoPi{ 2 * glm::pi<float>() };

	m_Angle += Timer::getInstance().getDeltaSeconds() * twoPi / m_Period;

	m_Angle = glm::mod(m_Angle, twoPi);

	getParentingGameObject().getComponent<Transform>()->setWorldTranslation(m_Center + m_Radius * glm::vec2(glm::cos(m_Angle), glm::sin(m_Angle)));
	getParentingGameObject().getComponent<Transform>()->setWorldRotation(m_Angle);
}

void fro::Rotator::setCenter(glm::vec2 const& center)
{
	m_Center = center;
}

void fro::Rotator::setRadius(float const radius)
{
	m_Radius = radius;
}

void fro::Rotator::setPeriod(float const period)
{
	m_Period = period;
}
#pragma endregion PublicMethods