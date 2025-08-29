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
	VAL_RETURN_CODE Image::create(ValProc& proc, const IMAGE_TILING tiling, const IMAGE_ASPECT aspect)
	{
#ifndef NDEBUG
		if (_img != NULL) {
			dbg::printWarning("Image::create: This function should never be called on an image (@ %p) that has already been created; it will result in a memory leak.", this);
			throw std::runtime_error("BAD FUNCTION CALL");
		}
#endif // !NDEBUG
		if (_width == 0 || _height == 0 || _usages == 0x0)
			return VAL_FAILURE;

		
		proc.createImage(_width, _height, _format, (VkImageTiling)tiling, _usages, GPU_ONLY, _img, _imgMemory, _mipMapLevel, VK_SAMPLE_COUNT_1_BIT, _layout, (VkImageAspectFlags)aspect);
		dbg::recordVkObjectCreation(proc, _img);
		dbg::recordVkObjectCreation(proc, _imgMemory);

		if (!_img)
			return VAL_FAILURE;
		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE Image::overwriteFromPixels(ValProc& proc, const uint8_t* pixels, uint16_t width, uint16_t height, uint8_t channelCount, int32_t dstOffsetX, int32_t dstOffsetY)
	{	
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VkDeviceSize imgSize = width * height * channelCount;
		VkImageLayout oldLayout = _layout;
		proc.transitionImageLayout(_img, _format, _layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		proc.createBuffer(imgSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		if (!stagingBufferMemory)
			return VAL_FAILURE;

		void* data;
		vkMapMemory(proc, stagingBufferMemory, 0, imgSize, 0, &data);
		memcpy(data, pixels, imgSize);
		vkUnmapMemory(proc, stagingBufferMemory);

		proc.copyBufferToImage(stagingBuffer, _img, width,height, dstOffsetX,dstOffsetY);

		proc.transitionImageLayout(_img, _format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _layout);

		vkFreeMemory(proc, stagingBufferMemory, nullptr);
		vkDestroyBuffer(proc, stagingBuffer, nullptr);

		return VAL_SUCCESS;
	}

	VAL_RETURN_CODE Image::overwriteFromImage(ValProc& proc, const VkImage srcImg, const VkFormat srcFormat, VkImageLayout srcLayout, uint16_t width, uint16_t height, uint8_t channelCount, int32_t dstOffsetX, int32_t dstOffsetY, int32_t srcOffsetX, int32_t srcOffsetY)
	{
		proc.copyImage(srcImg, _img, _format, srcFormat, _layout, srcLayout, width, height, dstOffsetX, dstOffsetY, srcOffsetX, srcOffsetY);

		return VAL_SUCCESS;
	}

	void Image::resize(ValProc& proc, uint16_t newWidth, uint16_t newHeight)
	{
		if (_width == newWidth && _height == newHeight) {
			return;
		}
		if (!_img) {
			_width = newWidth;
			_height = newHeight;
			return;
		}
		VkImage tmpImg;
		VkDeviceMemory tmpDeviceMem;

		proc.createImage(newWidth, newHeight, _format, VK_IMAGE_TILING_LINEAR, _usages, GPU_ONLY, tmpImg, tmpDeviceMem,_mipMapLevel, VK_SAMPLE_COUNT_1_BIT, _layout);

		proc.copyImage(_img, tmpImg, _format, _format, _layout, _layout, _width, _height);
		
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

		proc.copyImage(_img, other->_img, _format, _format, _layout, _layout, _width, _height);
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
		if (!_img)
			return;

		VkImage tmpImg;
		VkDeviceMemory tmpDeviceMem;

		proc.createImage(_width, _height, _format, VK_IMAGE_TILING_LINEAR, _usages, GPU_ONLY, tmpImg, tmpDeviceMem, _mipMapLevel, VK_SAMPLE_COUNT_1_BIT, _layout);

		proc.copyImage(_img, tmpImg, _format, _format, _layout, _layout, _width, _height);

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

		proc.copyImage(_img, tmpImg, _format, _format, _layout, _layout, _width, _height);

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


	void Image::setFormat(ValProc& proc, VkFormat f)
	{
		if (_format == f) {
			return;
		}

		_format = f;
		if (_img)
		{
			VkImage tmpImg;
			VkDeviceMemory tmpDeviceMem;

			proc.createImage(_width, _height, _format, VK_IMAGE_TILING_LINEAR, _usages, GPU_ONLY, tmpImg, tmpDeviceMem, _mipMapLevel, VK_SAMPLE_COUNT_1_BIT, _layout);

			proc.copyImage(_img, tmpImg, _format, f, _layout, _layout, _width, _height);

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

	VkFormat Image::getFormat() const
	{
		return _format;
	}

	VAL_RETURN_CODE Image::setLayout(ValProc& proc, VkImageLayout l, VkCommandBuffer buffer)
	{
		if (_layout = l)
			return VAL_SUCCESS;
		if (_img) {
			proc.transitionImageLayout(_img, _format, _layout, l, buffer, _mipMapLevel);
		}
		_layout = l;
		return VAL_SUCCESS;
	}

	VkImageLayout Image::getLayout() const
	{
		return _layout;
	}
}