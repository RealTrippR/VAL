#include <VAL/lib/system/windowCursor.hpp>

namespace val
{
	VAL_RETURN_CODE Cursor::createFromDisk(const fs::path& path, const int hotspotX, const int hotspotY)
	{
		GLFWimage images[1];
		images[0].pixels = stbi_load(path.string().c_str(), &images[0].width, &images[0].height, 0, 4); // Load as RGBA

		if (images[0].pixels) {
			_cursor = glfwCreateCursor(images, hotspotX, hotspotY);
			stbi_image_free(images[0].pixels);
			if (!_cursor) {
				VAL_FAILURE;
			}
			return VAL_SUCCESS;
		}
		else {
			return VAL_FAILURE;
		}
	}

	VAL_RETURN_CODE Cursor::createFromMemory(const void* pixels, const uint32_t srcWidth, const uint32_t srcHeight, const int hotspotX, const int hotspotY) {
		GLFWimage images[1];
		images[0].pixels = (unsigned char*)pixels;
		images[0].width = srcWidth;
		images[0].height = srcHeight;
		if (images[0].pixels) {
			_cursor = glfwCreateCursor(images, hotspotX, hotspotY);
			if (!_cursor) {
				VAL_FAILURE;
			}
			return VAL_SUCCESS;
		}
		else {
			return VAL_FAILURE;
		}
	}

	void Cursor::destroy() {
		glfwDestroyCursor(_cursor);
	}


	GLFWcursor* Cursor::getGLFWCursor() const
	{
		return _cursor;
	}

}