#include "SpriteAnimator.h"

#include "GameObject.h"
#include "Sprite.h"

#pragma region Constructors/Destructor
fro::SpriteAnimator::SpriteAnimator(GameObject& parentingGameObject)
	: Behaviour(parentingGameObject)
{
}
#pragma endregion Constructors/Destructor



#pragma region PublicMethods
void fro::SpriteAnimator::update(float const deltaSeconds)
{
	if (not m_Play or not m_pActiveAnimation)
		return;

	auto& [vAnimationFrames, frameTimeSeconds, shouldLoop] { *m_pActiveAnimation };

	if (vAnimationFrames.size() <= 1)
		return;

	m_ElapsedSeconds += deltaSeconds;
	if (m_ElapsedSeconds < frameTimeSeconds)
		return;

	m_ElapsedSeconds -= frameTimeSeconds;

	++m_CurrentFrameIndex %= vAnimationFrames.size();
	updateSprite();

	if (not shouldLoop and m_CurrentFrameIndex == vAnimationFrames.size() - 1)
		m_Play = false;
}

void fro::SpriteAnimator::setActiveAnimation(std::string_view const animationName)
{
	Animation& animation{ m_mAnimations[animationName] };
	if (m_pActiveAnimation not_eq &animation)
	{
		m_pActiveAnimation = &animation;
		reset();
	}
}

void fro::SpriteAnimator::play()
{
	m_Play = true;
}

void fro::SpriteAnimator::pause()
{
	m_Play = false;
}

void fro::SpriteAnimator::reset()
{
	m_CurrentFrameIndex = 0;
	updateSprite();

	m_ElapsedSeconds = 0.0f;
}

void fro::SpriteAnimator::stop()
{
	pause();
	reset();
}

void fro::SpriteAnimator::addAnimationFrames(std::string_view const animationName,
	std::initializer_list<AnimationFrame> const& animationFrames)
{
	Animation& animation{ m_mAnimations[animationName] };
	auto& vAnimationFrames{ animation.vAnimationFrames };
	vAnimationFrames.insert(vAnimationFrames.end(), animationFrames);

	if (not m_pActiveAnimation)
	{
		m_pActiveAnimation = &animation;
		updateSprite();
	}
}

void fro::SpriteAnimator::addAnimationFrames(std::string_view const animationName,
	std::string_view const animationFileName,
	glm::vec2 const& animationFrameSize,
	std::uint32_t const amountOfFramesX,
	std::uint32_t const amountOfFramesY)
{
	Animation& animation{ m_mAnimations[animationName] };

	for (std::uint32_t y{}; y < amountOfFramesY; ++y)
		for (std::uint32_t x{}; x < amountOfFramesX; ++x)
			animation.vAnimationFrames.push_back(AnimationFrame(animationFileName,
				SDL_FRect
				{
					.x{ x * animationFrameSize.x },
					.y{ y * animationFrameSize.y },
					.w{ animationFrameSize.x },
					.h{ animationFrameSize.y }
				}));

	if (not m_pActiveAnimation)
	{
		m_pActiveAnimation = &animation;
		updateSprite();
	}
}

void fro::SpriteAnimator::setFramesPerSecond(std::string_view const animationName, int const framesPerSecond)
{
	m_mAnimations[animationName].frameTimeSeconds = 1.0f / framesPerSecond;
}

void fro::SpriteAnimator::setLoop(std::string_view const animationName, bool const shouldLoop)
{
	m_mAnimations[animationName].shouldLoop = shouldLoop;
}
#pragma endregion PublicMethods



#pragma region PrivateMethods
void fro::SpriteAnimator::updateSprite() const
{
	AnimationFrame const& currentAnimationFrame
	{
		m_pActiveAnimation->vAnimationFrames[m_CurrentFrameIndex]
	};

	Sprite& spriteComponent{ *getParentingGameObject().getComponent<Sprite>() };

	if (spriteComponent.getFileName() not_eq currentAnimationFrame.fileName)
		spriteComponent.setFileName(currentAnimationFrame.fileName);

	spriteComponent.setSourceRectangle(currentAnimationFrame.sourceRectangle);
}
#pragma endregion PrivateMethods