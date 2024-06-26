#if not defined fro_SPRITE_H
#define fro_SPRITE_H

#include "Defines.hpp"
#include "Renderable.h"

#include <sdl_rect.h>
#include <vec2.hpp>

#include <string>

struct SDL_Texture;

namespace fro
{
	class Sprite final : public Renderable
	{
	public:
		Sprite(Reference<GameObject> const parentingGameObject);

		virtual ~Sprite() override = default;

		virtual void render() const override;

		void setFileName(std::string fileName);
		void setSourceRectangle(SDL_FRect const sourceRectangle);

		std::string_view getFileName() const;
		glm::ivec2 getSize() const;

	private:
		Sprite(Sprite const&) = delete;
		Sprite(Sprite&&) noexcept = delete;

		Sprite& operator=(Sprite const&) = delete;
		Sprite& operator=(Sprite&&) noexcept = delete;

		std::string m_FileName{};
		SDL_Texture* m_pTexture{};
		SDL_FRect m_SourceRectangle{};

		glm::ivec2 m_Size{};
	};
}

#endif