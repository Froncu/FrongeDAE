#include "Sprite.h"

#include "GameObject.h"
#include "RenderContext.h"
#include "ResourceManager.h"

#include <sdl_render.h>

#pragma region Constructors/Destructor
fro::Sprite::Sprite(Reference<GameObject> const parentingGameObject)
	: Renderable(parentingGameObject)
{
}
#pragma endregion Constructors/Destructor



#pragma region PublicMethods
void fro::Sprite::render() const
{
	RenderContext::getInstance().renderTexture(m_pTexture,
		parentingGameObject.get().getWorldTransform(),
		m_SourceRectangle);
}

void fro::Sprite::setFileName(std::string fileName)
{
	m_FileName = std::move(fileName);
	m_pTexture = ResourceManager::getInstance().getImageTexture(RenderContext::getInstance().getRenderer(), m_FileName);
	SDL_QueryTexture(m_pTexture, nullptr, nullptr, &m_Size.x, &m_Size.y);
}

void fro::Sprite::setSourceRectangle(SDL_FRect const sourceRectangle)
{
	m_SourceRectangle = sourceRectangle;
}

std::string_view fro::Sprite::getFileName() const 
{
	return m_FileName;
}

glm::ivec2 fro::Sprite::getSize() const
{
	return m_Size;
}
#pragma endregion PublicMethods