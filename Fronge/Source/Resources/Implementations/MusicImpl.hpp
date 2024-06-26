#if not defined MUSIC_IMPL_HPP
#define MUSIC_IMPL_HPP

#include "froch.hpp"

#include "Resources/Music.hpp"
#include "Utility.hpp"

typedef struct _Mix_Music Mix_Music;

namespace fro
{
	class Music::Implementation final
	{
	public:
		Implementation(std::string_view const filePath);

		~Implementation() = default;

		Mix_Music* get() const;

	private:
		Implementation(Implementation const&) = delete;
		Implementation(Implementation&&) noexcept = delete;

		Implementation& operator=(Implementation const&) = delete;
		Implementation& operator=(Implementation&&) noexcept = delete;

		CustomUniquePointer<Mix_Music> mData;
	};
}

#endif