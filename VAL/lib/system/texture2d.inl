/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <VAL/lib/system/texture2d.hpp>

#ifndef VAL_TEXTURE_2D_INLINE
#define VAL_TEXTURE_2D_INLINE

namespace val {

	/* PUBLIC: */

	inline stbi_uc* texture2d::getPixels() const {
		return _pixels;
	}

	inline uint32_t texture2d::getWidth() const 
	{
		return _width;
	}

	inline uint32_t texture2d::getHeight() const 
	{
		return _height;
	}

	inline VkFormat texture2d::getVkFormat() const 
	{
		return _format;
	}

	inline VkImageLayout texture2d::getImageLayout() const 
	{
		return _layout;
	}

	inline bufferSpace texture2d::getBufferSpace() const
	{
		if (_pixels) {
			return bufferSpace::CPU_GPU;
		}
		else {
			return bufferSpace::GPU_ONLY;
		}
	}

	inline VkDeviceMemory texture2d::getDeviceMemory()
	{
		return _imgMemory;
	}

	inline VkImage texture2d::getVkImage()
	{
		return _img;
	}

	inline void texture2d::transitionLayout(VkCommandBuffer cmd_buff, VkImageLayout newLayout) 
	{
		_proc.transitionImageLayout(_img, _format, _layout, newLayout, cmd_buff, _mipLevels);
	}
}

#endif // !VAL_TEXTURE_2D_INLINE