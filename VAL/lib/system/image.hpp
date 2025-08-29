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

#include <VAL/lib/system/system_utils.hpp>

#ifndef VAL_STORAGE_IMAGE_HPP
#define VAL_STORAGE_IMAGE_HPP

namespace val
{
	class Image
	{
	public:
		Image() = default;

		Image(ValProc& proc, uint16_t width, uint16_t height, VkFormat format, VkImageLayout imgLayout, VkImageUsageFlags usages, VkImageTiling tiling, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT)
		{
			_width = width;
			_height = height;
			_format = format;
			_layout = imgLayout;
			_usages = usages;
			create(proc, (IMAGE_TILING)tiling, (IMAGE_ASPECT)aspect);
		}
		Image(ValProc& proc, uint16_t width, uint16_t height, VkFormat format, IMAGE_LAYOUT imgLayout, IMAGE_USAGE usages, IMAGE_TILING tiling, IMAGE_ASPECT aspect = IMAGE_ASPECT::Color)
		{
			_width = width;
			_height = height;
			_format = format;
			_layout = (VkImageLayout)imgLayout;
			_usages = (VkImageUsageFlags)usages;
			create(proc,tiling, aspect);
		}

		~Image()
		{
#ifndef NDEBUG
			if (_img != NULL) {
				dbg::printError("Image::~Image: Image @ %p was not properly destroyed.", this);
				//throw std::runtime_error("Image::~Image");
			}
#endif // !NDEBUG
		}

		static inline VkDescriptorType getVkDescriptorType() {
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		}


		inline ImageViewBindInfo toImageViewBindInfo()
		{
			ImageViewBindInfo bindInfo;
			bindInfo.format = _format;
			bindInfo.layout = &_layout;
			bindInfo.image = _img;
			return bindInfo;
		}

		operator ImageViewBindInfo()
		{
			return toImageViewBindInfo();
		}
		

		inline ImageLayoutTransitionInfo toImageLayoutTransitionInfo()
		{
			ImageLayoutTransitionInfo layoutInfo = {
				.pImgLayout = &_layout,
				.img = _img,
			};
			return layoutInfo;
		}

		operator ImageLayoutTransitionInfo() {
			return toImageLayoutTransitionInfo();
		}

	public:
		VAL_RETURN_CODE create(ValProc& proc, const IMAGE_TILING tiling = IMAGE_TILING::Linear, const IMAGE_ASPECT = IMAGE_ASPECT::Color);

		VAL_RETURN_CODE overwriteFromPixels(ValProc& proc, const uint8_t* pixels, uint16_t width, uint16_t height, uint8_t channelCount, int32_t dstOffsetX = 0u, int32_t dstOffsetY = 0u);

		VAL_RETURN_CODE overwriteFromImage(ValProc& proc, const VkImage srcImg, const VkFormat srcFormat, VkImageLayout srcLayout, uint16_t width, uint16_t height, uint8_t channelCount, int32_t dstOffsetX = 0u, int32_t dstOffsetY = 0u, int32_t srcOffsetX = 0u, int32_t srcOffsetY = 0u);

		void resize(ValProc& proc, uint16_t newWidth, uint16_t newHeight);

		void destroy(ValProc& proc);

		void copyToOther(ValProc& proc, Image* other);

		inline VkImage getVkImage() const { return _img; };

		inline VkDeviceMemory getDeviceMemory() const { return _imgMemory; };

		inline uint16_t getWidth() const { return _width; };

		void setWidth(ValProc& proc, const uint16_t width);

		inline uint16_t getHeight() const { return _height; };

		void setHeight(ValProc& proc, const uint16_t height);

		inline VkImageUsageFlags getUsages() const { return _usages; };

		void setUsages(ValProc& proc, VkImageUsageFlags usages);

		inline uint8_t getMipmapLevel() const { return _mipMapLevel; };

		void setMipmapLevel(ValProc& proc, const uint8_t mipmaplevel);

		void setFormat(ValProc&, VkFormat);

		VkFormat getFormat() const;

		VAL_RETURN_CODE setLayout(ValProc&, VkImageLayout, VkCommandBuffer buffer = VK_NULL_HANDLE);

		VkImageLayout getLayout() const;

	private:
		VkImage _img = NULL;
		VkDeviceMemory _imgMemory;

		VkImageLayout _layout;
		VkFormat _format;
		VkImageUsageFlags _usages;

		uint16_t _width;
		uint16_t _height;
		uint8_t _mipMapLevel = 1u;
	};
}

#endif // !VAL_STORAGE_IMAGE_HPP
