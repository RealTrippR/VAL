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

	inline void Texture2D::setValProc(ValProc* proc)
	{
#ifndef NDEBUG
		if (_proc) {
			dbg::printError("Texture2D::setValProc: Texture2D @ %p: The ValProc of a Texture2D cannot be set more than once.", this);
			throw std::runtime_error("Texture2D::setValProc: The ValProc of a Texture2D cannot be set more than once.");
		}
#endif // !NDEBUG

		_proc = proc;
	}

	inline ValProc* Texture2D::getValProc()
	{
		return _proc;
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

	inline bufferSpace Texture2D::getBufferSpace() const
	{
		if (_pixels) {
			return bufferSpace::CPU_GPU;
		}
		else {
			return bufferSpace::GPU_ONLY;
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

	inline void Texture2D::transitionLayout(VkCommandBuffer cmd_buff, VkImageLayout newLayout)
	{
		_proc->transitionImageLayout(_img, _format, _layout, newLayout, cmd_buff, _mipLevels);
		_layout = newLayout;
	}
}

#endif // !VAL_TEXTURE_2D_INLINE