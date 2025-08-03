#ifndef VAL_WINDOW_CURSOR_HPP
#define VAL_WINDOW_CURSOR_HPP


#include <VAL/lib/system/system_utils.hpp>


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace val
{
	class Cursor 
	{
	public:

		Cursor() = default;

		Cursor(const fs::path& path, const int hotspotX, const int hotspotY) {
			createFromDisk(path, hotspotX, hotspotY);
		}

		~Cursor() {
			destroy();
		}

		VAL_RETURN_CODE createFromDisk(const fs::path& path, const int hotspotX, const int hotspotY);

		VAL_RETURN_CODE createFromMemory(const void* pixels, const uint32_t srcWidth, const uint32_t srcHeight, const int hotspotX, const int hotspotY);

		void destroy();

		GLFWcursor* getGLFWCursor() const;

		operator GLFWcursor* () const {
			return _cursor;
		}

		operator const GLFWcursor*() const {
			return _cursor;
		}

	protected:
		GLFWcursor* _cursor;
	};
}

#endif // !VAL_WINDOW_CURSOR_HPP