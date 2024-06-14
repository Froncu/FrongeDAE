#if not defined WINDOW_HPP
#define WINDOW_HPP

#include "Core.hpp"

#include <memory>
#include <string>

namespace fro
{
	class Window final
	{
	public:
		FRO_API Window(std::string title = "Fronge",
			int width = 640, int height = 480,
			bool vSync = true);

		FRO_API ~Window();

		FRO_API void update();

	private:
		Window(Window const&) = delete;
		Window(Window&) noexcept = delete;

		Window& operator=(Window const&) = delete;
		Window& operator=(Window&) noexcept = delete;

		struct Data final
		{
			std::string title;
			int width;
			int height;
			bool vSync;
		};

		Data mData;

		class Implementation;
		std::unique_ptr<Implementation> mImplementation;
	};
}

#endif