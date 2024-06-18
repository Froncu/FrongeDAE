#if not defined BASE_EVENT_HPP
#define BASE_EVENT_HPP

#include "froch.hpp"

#include "Core.hpp"
#include "Utility.hpp"

namespace fro
{
	class BaseEvent
	{
	public:
		enum class Type
		{
			WINDOW_CLOSE, WINDOW_RESIZE, WINDOW_FOCUS, WINDOW_UNFOCUS, WINDOW_MOVE,

			KEY_PRESS, KEY_RELEASE,

			BUTTON_PRESSED, BUTTON_RELEASED,

			MOUSE_MOVE, MOUSE_SCROLL
		};

		enum class CategoryType
		{
			SYSTEM	= bitfield(0),
			WINDOW	= bitfield(1),
			INPUT	= bitfield(2),
			KEY		= bitfield(3),
			BUTTON	= bitfield(4),
			MOUSE	= bitfield(5),
		};

		virtual ~BaseEvent() = default;

		FRO_NODISCARD FRO_API virtual std::string getLogString() const;
		FRO_NODISCARD FRO_API bool isInCategory(CategoryType const categoryFlag) const;

		std::string_view const mName;
		Type const mType;
		Bitfield const mCategoryTypes;

	protected:
		template<std::same_as<CategoryType> ...Bits>
		BaseEvent(std::string_view const name, Type const type, Bits const... categoryTypes)
			: mName{ name }
			, mType{ type }
			, mCategoryTypes{ (static_cast<Bitfield>(categoryTypes) | ...) }
		{
			//Logger::info("{}!", getLogString());
		}

		BaseEvent(BaseEvent const&) = default;
		BaseEvent(BaseEvent&&) noexcept = default;

	private:
		BaseEvent& operator=(BaseEvent const&) = delete;
		BaseEvent& operator=(BaseEvent&&) noexcept = delete;
	};
}

#endif