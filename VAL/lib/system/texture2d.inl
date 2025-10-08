/*
Copyright � 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the �Software�), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <VAL/lib/system/texture2d.hpp>

#ifndef VAL_TEXTURE_2D_INLINE
#define VAL_TEXTURE_2D_INLINE

namespace val {


	inline void Texture2D::discardPixels()
	{
		if (_pixels) {
			stbi_image_free(_pixels);
			_pixels = NULL;
		}
	}

	inline stbi_uc* Texture2D::getPixels() const {
		return _pixels;
	}

	inline uint32_t Texture2D::getWidth() const
	{
		return _width;
	}

	inline uint32_t Texture2D::getHeight() const
	{
		return _height;
	}

	inline VkFormat Texture2D::getVkFormat() const
	{
		return _format;
	}

	inline VkImageLayout Texture2D::getImageLayout() const
	{
		return _layout;
	}

	inline BUFFER_SPACE Texture2D::getBufferSpace() const
	{
		if (_pixels) {
			return BUFFER_SPACE::CPU_GPU;
		}
		else {
			return BUFFER_SPACE::GPU_ONLY;
		}
	}

	inline VkDeviceMemory Texture2D::getDeviceMemory()
	{
		return _imgMemory;
	}

	inline VkImage Texture2D::getVkImage()
	{
		return _img;
	}

	inline void Texture2D::transitionLayout(Queue& q, VkCommandBuffer cmd_buff, VkImageLayout newLayout)
	{
		auto* _proc = q.getValProc();
		_proc->transitionImageLayout(q,q.getCmdPool(), _img, _format, _layout, newLayout, cmd_buff, _mipLevels);
		_layout = newLayout;
	}
}

#endif // !VAL_TEXTURE_2D_INLINE