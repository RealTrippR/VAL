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

#include <VAL/lib/system/image.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val
{
	void Image::create(ValProc& proc, const IMAGE_TILING tiling, const IMAGE_ASPECT aspect)
	{
#ifndef NDEBUG
		if (_img != NULL) {
			dbg::printWarning("Image::create: This function should never be called on an image (@ %p) that has already been created; it will result in a memory leak.", this);
			throw std::runtime_error("BAD FUNCTION CALL");
		}
#endif // !NDEBUG

		
		proc.createImage(_width, _height, _format, (VkImageTiling)tiling, _usages, GPU_ONLY, _img, _imgMemory, _mipMapLevel, VK_SAMPLE_COUNT_1_BIT, _layout, (VkImageAspectFlags)aspect);
		dbg::recordVkObjectCreation(proc, _img);
		dbg::recordVkObjectCreation(proc, _imgMemory);
	}

	void Image::resize(ValProc& proc, uint16_t newWidth, uint16_t newHeight)
	{
		if (_width == newWidth && _height == newHeight) {
			return;
		}
		VkImage tmpImg;
		VkDeviceMemory tmpDeviceMem;

		proc.createImage(newWidth, newHeight, _format, VK_IMAGE_TILING_LINEAR, _usages, GPU_ONLY, tmpImg, tmpDeviceMem,_mipMapLevel, VK_SAMPLE_COUNT_1_BIT, _layout);

		proc.copyImage(_img, tmpImg, _format, _layout, _layout, _width, _height);
		
		_width = newWidth;
		_height = newHeight;

		if (_img) {
			vkDestroyImage(proc, _img, NULL);
			vkFreeMemory(proc, _imgMemory, NULL);
			dbg::recordVkObjectDestruction(proc, _img);
			dbg::recordVkObjectDestruction(proc, _imgMemory);
		}
		_img = tmpImg;
		_imgMemory = tmpDeviceMem;
		dbg::recordVkObjectCreation(proc, _img);
		dbg::recordVkObjectCreation(proc, _imgMemory);
	}

	void Image::destroy(ValProc& proc)
	{
		if (_img) {
			vkDestroyImage(proc, _img, NULL);
			vkFreeMemory(proc, _imgMemory, NULL);
			_img = NULL;
			dbg::recordVkObjectDestruction(proc, _img);
			dbg::recordVkObjectDestruction(proc, _imgMemory);
		}
	}

	void Image::copyToOther(ValProc& proc, Image* other)
	{
		other->destroy(proc);
		other->_width = _width;
		other->_height = _height;
		other->_layout = _layout;
		other->_format = _format;
		other->_mipMapLevel = _mipMapLevel;
		other->_img = VK_NULL_HANDLE;
		other->_usages = _usages;

		other->create(proc);

		proc.copyImage(_img, other->_img, _format, _layout, _layout, _width, _height);
	}


	void Image::setWidth(ValProc& proc, const uint16_t width)
	{
		resize(proc, width, _height);
	}


	void Image::setHeight(ValProc& proc, const uint16_t height)
	{
		resize(proc, _width, height);
	}

	void Image::setUsages(ValProc& proc, VkImageUsageFlags usages)
	{
		if (_usages == usages) {
			return;
		}

		_usages = usages;
		VkImage tmpImg;
		VkDeviceMemory tmpDeviceMem;

		proc.createImage(_width, _height, _format, VK_IMAGE_TILING_LINEAR, _usages, GPU_ONLY, tmpImg, tmpDeviceMem, _mipMapLevel, VK_SAMPLE_COUNT_1_BIT, _layout);

		proc.copyImage(_img, tmpImg, _format, _layout, _layout, _width, _height);

		if (_img) {
			vkDestroyImage(proc, _img, NULL);
			vkFreeMemory(proc, _imgMemory, NULL);
			dbg::recordVkObjectDestruction(proc, _img);
			dbg::recordVkObjectDestruction(proc, _imgMemory);
		}
		_img = tmpImg;
		_imgMemory = tmpDeviceMem;
		dbg::recordVkObjectCreation(proc, _img);
		dbg::recordVkObjectCreation(proc, _imgMemory);


	}
	void Image::setMipmapLevel(ValProc& proc, const uint8_t mipmaplevel)
	{
		if (_mipMapLevel == mipmaplevel) {
			return;
		}

		_mipMapLevel = mipmaplevel;

		VkImage tmpImg;
		VkDeviceMemory tmpDeviceMem;

		proc.createImage(_width, _height, _format, VK_IMAGE_TILING_LINEAR, _usages, GPU_ONLY, tmpImg, tmpDeviceMem, _mipMapLevel, VK_SAMPLE_COUNT_1_BIT, _layout);

		proc.copyImage(_img, tmpImg, _format, _layout, _layout, _width, _height);

		if (_img) {
			vkDestroyImage(proc, _img, NULL);
			vkFreeMemory(proc, _imgMemory, NULL);
			dbg::recordVkObjectDestruction(proc, _img);
			dbg::recordVkObjectDestruction(proc, _imgMemory);
		}
		_img = tmpImg;
		_imgMemory = tmpDeviceMem;
		dbg::recordVkObjectCreation(proc, _img);
		dbg::recordVkObjectCreation(proc, _imgMemory);
	}

}