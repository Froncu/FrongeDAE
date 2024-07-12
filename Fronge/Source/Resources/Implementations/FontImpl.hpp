#if not defined FONT_IMPL_HPP
#define FONT_IMPL_HPP

#include "Resources/Font.hpp"
#include "Utility.hpp"

typedef struct _TTF_Font TTF_Font;

namespace fro
{
	class Font::Implementation final
	{
	public:
		Implementation(std::string_view const filePath, int const size);

		~Implementation() = default;

		FRO_NODISCARD TTF_Font* getSDLFont() const;

	private:
		FRO_NODISCARD static CustomUniquePointer<TTF_Font> createFont(std::string_view const filePath, int const size);

		Implementation(Implementation const&) = delete;
		Implementation(Implementation&&) noexcept = delete;

		Implementation& operator=(Implementation const&) = delete;
		Implementation& operator=(Implementation&&) noexcept = delete;

		CustomUniquePointer<TTF_Font> mSDLFont;
	};
}

#endif