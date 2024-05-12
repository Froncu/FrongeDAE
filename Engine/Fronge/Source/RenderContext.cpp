#include "RenderContext.h"

#include "Steam.h"

#include <sdl.h>

#include <array>
#include <format>
#include <stdexcept>
#include <tuple>

#pragma region Constructors/Destructor
fro::RenderContext::RenderContext()
{
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) not_eq 0)
		throw std::runtime_error(std::format("SDL_InitSubSystem() failed: {}", SDL_GetError()));

	m_pWindow = { SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_ViewportSize.x, m_ViewportSize.y, NULL), SDL_DestroyWindow };
	if (not m_pWindow.get())
		throw std::runtime_error(std::format("SDL_CreateWindow() failed: {}", SDL_GetError()));

	// HACK: Steam must be initialized before the renderer, otherwise the overlay does not work for some reason
	std::ignore = Steam::getInstance();
	// END HACK

	m_pRenderer = { SDL_CreateRenderer(m_pWindow.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), SDL_DestroyRenderer };
	if (not m_pRenderer.get())
		throw std::runtime_error(std::format("SDL_CreateRenderer() failed: -> {}", SDL_GetError()));

	updateViewPort();
}

fro::RenderContext::~RenderContext()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
#pragma endregion Constructors/Destructor



#pragma region PublicMethods
void fro::RenderContext::processSystemEvent(SDL_Event const& event)
{
	if (event.type == SDL_WINDOWEVENT and
		event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		updateViewPort();
}

void fro::RenderContext::clear() const
{
	SDL_Color constexpr clearColor{ 32, 32, 32, 255 };

	SDL_SetRenderDrawColor(m_pRenderer.get(), clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	SDL_RenderClear(m_pRenderer.get());
	SDL_SetRenderDrawColor(m_pRenderer.get(), 255, 255, 255, 255);
}

void fro::RenderContext::present() const
{
	SDL_RenderPresent(m_pRenderer.get());
}

void fro::RenderContext::renderTexture(SDL_Texture* const pTexture, TransformationMatrix2D const& transform,
	SDL_FRect sourceRectangle) const
{
	if (not pTexture)
		return;

	int textureWidth;
	int textureHeight;
	SDL_QueryTexture(pTexture, nullptr, nullptr, &textureWidth, &textureHeight);

	if (not sourceRectangle.w or not sourceRectangle.h)
	{
		sourceRectangle.x = 0;
		sourceRectangle.y = 0;
		sourceRectangle.w = static_cast<float>(textureWidth);
		sourceRectangle.h = static_cast<float>(textureHeight);
	}

	float const halfSourceWidth{ sourceRectangle.w / 2 };
	float const halfSourceHeight{ sourceRectangle.h / 2 };

	SDL_Color constexpr vertexColor
	{
		.r{ 255 },
		.g{ 255 },
		.b{ 255 },
		.a{ 255 }
	};

	glm::vec2 const topLeftTexture
	{
		sourceRectangle.x / textureWidth,
		sourceRectangle.y / textureHeight
	};

	glm::vec2 const bottomRightTexture
	{
		(sourceRectangle.x + sourceRectangle.w) / textureWidth,
		(sourceRectangle.y + sourceRectangle.h) / textureHeight,
	};

	std::array<SDL_Vertex, 4> vVertices
	{
		SDL_Vertex({ -halfSourceWidth, -halfSourceHeight },
		vertexColor, { topLeftTexture.x, topLeftTexture.y }),

		SDL_Vertex({ halfSourceWidth, -halfSourceHeight },
		vertexColor, { bottomRightTexture.x, topLeftTexture.y }),

		SDL_Vertex({ halfSourceWidth, halfSourceHeight },
		vertexColor, { bottomRightTexture.x, bottomRightTexture.y }),

		SDL_Vertex({ -halfSourceWidth, halfSourceHeight },
		vertexColor, { topLeftTexture.x, bottomRightTexture.y }),
	};

	for (SDL_Vertex& vertex : vVertices)
	{
		glm::ivec3 position{ vertex.position.x, vertex.position.y, 1.0f };

		position = position * transform.getTransformation();
		vertex.position.x = static_cast<float>(position.x);
		vertex.position.y = static_cast<float>(position.y);
	}

	std::array<int, 6> constexpr vIndices
	{
		0, 1, 2,
		2, 3, 0
	};

	SDL_RenderGeometry(m_pRenderer.get(), pTexture, vVertices.data(), static_cast<int>(vVertices.size()), vIndices.data(), static_cast<int>(vIndices.size()));
}

SDL_Window* fro::RenderContext::getWindow() const
{
	return m_pWindow.get();
}

SDL_Renderer* fro::RenderContext::getRenderer() const
{
	return m_pRenderer.get();
}

glm::ivec2 const& fro::RenderContext::getViewportSize() const
{
	return m_ViewportSize;
}

void fro::RenderContext::setFullScreen(bool const enable) const
{
	SDL_SetWindowFullscreen(m_pWindow.get(), enable ? SDL_WINDOW_FULLSCREEN_DESKTOP : NULL);
}

void fro::RenderContext::setWindowResizeable(bool const enable) const
{
	SDL_SetWindowResizable(m_pWindow.get(), static_cast<SDL_bool>(enable));
}

void fro::RenderContext::setWindowSize(int const width, int const height) const
{
	setFullScreen(false);

	SDL_SetWindowSize(m_pWindow.get(), width, height);
	SDL_SetWindowPosition(m_pWindow.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void fro::RenderContext::setWindowTitle(std::string_view const title) const
{
	SDL_SetWindowTitle(m_pWindow.get(), title.data());
}

void fro::RenderContext::setResolution(int const width, int const height)
{
	m_ViewportSize.x = width;
	m_ViewportSize.y = height;

	updateViewPort();
}

void fro::RenderContext::setScalingMode(ScalingMode const scalingMode)
{
	m_ScalingMode = scalingMode;

	updateViewPort();
}
#pragma endregion PublicMethods



#pragma region PrivateMethods
void fro::RenderContext::updateViewPort() const
{
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(m_pWindow.get(), &windowWidth, &windowHeight);

	if ((m_ScalingMode == ScalingMode::none or m_ScalingMode == ScalingMode::fill) and
		(m_ViewportSize.x <= windowWidth and m_ViewportSize.y <= windowHeight))
	{
		SDL_Rect viewport
		{
			.w{ m_ViewportSize.x },
			.h{ m_ViewportSize.y }
		};

		switch (m_ScalingMode)
		{
		case ScalingMode::none:
			viewport.x = windowWidth / 2 - m_ViewportSize.x / 2;
			viewport.y = windowHeight / 2 - m_ViewportSize.y / 2;
			SDL_RenderSetScale(m_pRenderer.get(), 1.0f, 1.0f);
			break;

		case ScalingMode::fill:
			SDL_RenderSetScale(m_pRenderer.get(), static_cast<float>(windowWidth) / m_ViewportSize.x, static_cast<float>(windowHeight) / m_ViewportSize.y);
			break;
		}

		SDL_RenderSetViewport(m_pRenderer.get(), &viewport);
	}
	else
		SDL_RenderSetLogicalSize(m_pRenderer.get(), m_ViewportSize.x, m_ViewportSize.y);
}
#pragma endregion PrivateMethods