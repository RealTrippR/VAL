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

#include <stdint.h>
#include <VAL/lib/system/system_utils.hpp>

#ifndef VAL_TEXTURE_2D_HPP
#define VAL_TEXTURE_2D_HPP

namespace val {

	class ImageView; // forward declaration

	class Texture2D {
	public:

		Texture2D() = default;

		Texture2D(ValProc& proc) : _proc(&proc) {};
		
		Texture2D(ValProc& proc, const VkImageLayout layout) : _proc(&proc), _layout(layout) {};

		Texture2D(ValProc& proc, const IMAGE_LAYOUT layout) : _proc(&proc), _layout((VkImageLayout)layout) {};

		Texture2D(ValProc& proc, const VkImageLayout layout, const VkFormat format) : _proc(&proc), _layout(layout), _format(format) {};

		Texture2D(ValProc& proc, const IMAGE_LAYOUT layout, const VkFormat format) : _proc(&proc), _layout((VkImageLayout)layout), _format(format) {};

		Texture2D(ValProc& proc, void* memory, uint32_t memorySize, const VkFormat format,
			const VkImageUsageFlagBits usages, const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 1u) : _proc(&proc)
		{
			createFromMemory(memory, memorySize, usages, layout, memspace, mipLevels);
		}

		Texture2D(ValProc& proc, std::filesystem::path srcpath, const VkFormat format,
			const VkImageUsageFlagBits usages, const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 1u
			, const uint16_t maxWidth = USE_SOURCE_DIMENSION, const uint16_t maxHeight = USE_SOURCE_DIMENSION) : _proc(&proc)
		{
			createFromDisk(srcpath, usages, layout, memspace, mipLevels, maxWidth, maxHeight);
		}

		Texture2D(ValProc& proc, const uint16_t width, const uint16_t height, const VkFormat format,
			const VkImageUsageFlagBits usages, const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 1u) : _proc(&proc)
		{
			create(width, height, format, usages, layout, memspace, mipLevels);
		}

		~Texture2D() {
			destroy();
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
		
		inline void setValProc(ValProc* proc);

		inline ValProc* getValProc();

		inline void discardPixels();

		inline stbi_uc* getPixels() const;

		inline uint32_t getWidth() const;

		inline uint32_t getHeight() const;

		inline VkFormat getVkFormat() const;

		inline VkImageLayout getImageLayout() const;

		inline bufferSpace getBufferSpace() const;

		inline VkDeviceMemory getDeviceMemory();

		inline VkImage getVkImage();

	public:

		inline void transitionLayout(VkCommandBuffer cmd_buff, VkImageLayout newLayout);

		void createFromMemory(const void* memory, const uint32_t memorySize, const VkImageUsageFlagBits usages,
			const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 1u);

		inline void createFromDisk(std::filesystem::path srcpath, const IMAGE_USAGE usages,
			const IMAGE_LAYOUT layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 1u, const uint16_t maxWidth = USE_SOURCE_DIMENSION, const uint16_t maxHeight = USE_SOURCE_DIMENSION)
		{
			createFromDisk(srcpath, (VkImageUsageFlagBits)usages, (VkImageLayout)layout, memspace, mipLevels, maxWidth, maxHeight);
		}



		void createFromDisk(std::filesystem::path srcpath, const VkImageUsageFlagBits usages,
			const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 1u, const uint16_t maxWidth = USE_SOURCE_DIMENSION, const uint16_t maxHeight = USE_SOURCE_DIMENSION);

		void create(const uint16_t width, const uint16_t height, VkFormat format, const VkImageUsageFlagBits usages,
			const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 1u);

		void destroy();

	protected:

		void generateMipmaps(const uint8_t mipLevels);

	protected:

		friend ImageView;

		stbi_uc* _pixels = NULL;
		VkImage _img = VK_NULL_HANDLE;
		VkDeviceMemory _imgMemory = VK_NULL_HANDLE;
		ValProc* _proc;

		VkImageLayout _layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkFormat _format = VK_FORMAT_UNDEFINED;

		uint16_t _width = 0u;
		uint16_t _height = 0u;
		
		uint8_t _channels = 0u;
		uint8_t _mipLevels = 0u;
	};
}

#endif // !VAL_TEXTURE_2D_HPP