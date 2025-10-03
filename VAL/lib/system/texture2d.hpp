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
#include <VAL/lib/system/queue.hpp>

#ifndef VAL_TEXTURE_2D_HPP
#define VAL_TEXTURE_2D_HPP

namespace val {

	class ImageView; // forward declaration

	class Texture2D {
	public:

		Texture2D() = default;
		
		Texture2D(const VkImageLayout layout) : _layout(layout) {};

		Texture2D(const IMAGE_LAYOUT layout) : _layout((VkImageLayout)layout) {};

		Texture2D(const VkImageLayout layout, const VkFormat format) : _layout(layout), _format(format) {};

		Texture2D(const IMAGE_LAYOUT layout, const VkFormat format) : _layout((VkImageLayout)layout), _format(format) {};

		Texture2D(Queue& q, void* memory, uint32_t memorySize, const VkFormat format,
			const VkImageUsageFlagBits usages, const VkImageLayout layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u)
		{
			createFromMemory(q, memory, memorySize, usages, layout, memspace, mipLevels);
		}

		Texture2D(Queue& q, std::filesystem::path srcpath, const VkFormat format,
			const VkImageUsageFlagBits usages, const VkImageLayout layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u
			, const uint16_t maxWidth = USE_SOURCE_DIMENSION, const uint16_t maxHeight = USE_SOURCE_DIMENSION)
		{
			createFromDisk(q, srcpath, usages, layout, memspace, mipLevels, maxWidth, maxHeight);
		}

		Texture2D(Queue& q, std::filesystem::path srcpath, const VkFormat format,
			const IMAGE_USAGE usages, const IMAGE_LAYOUT layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u
			, const uint16_t maxWidth = USE_SOURCE_DIMENSION, const uint16_t maxHeight = USE_SOURCE_DIMENSION)
		{
			createFromDisk(q, srcpath, usages, layout, memspace, mipLevels, maxWidth, maxHeight);
		}

		Texture2D(Queue& q, const uint16_t width, const uint16_t height, const VkFormat format,
			const VkImageUsageFlagBits usages, const VkImageLayout layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u)
		{
			create(q, width, height, format, usages, layout, memspace, mipLevels);
		}

		Texture2D(Queue& q, const uint16_t width, const uint16_t height, const VkFormat format,
			const IMAGE_USAGE usages, const IMAGE_LAYOUT layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u)
		{
			create(q, width, height, format, usages, layout, memspace, mipLevels);
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
		inline void discardPixels();

		inline stbi_uc* getPixels() const;

		inline uint32_t getWidth() const;

		inline uint32_t getHeight() const;

		inline VkFormat getVkFormat() const;

		inline VkImageLayout getImageLayout() const;

		inline BUFFER_SPACE getBufferSpace() const;

		inline VkDeviceMemory getDeviceMemory();

		inline VkImage getVkImage();

	public:

		inline void transitionLayout(Queue& q, VkCommandBuffer cmd_buff, VkImageLayout newLayout);

		void createFromMemory(Queue& q, const void* memory, const uint32_t memorySize, const VkImageUsageFlagBits usages,
			const VkImageLayout layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u);

		inline void createFromDisk(Queue& q, std::filesystem::path srcpath, const IMAGE_USAGE usages,
			const IMAGE_LAYOUT layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u, const uint16_t maxWidth = USE_SOURCE_DIMENSION, const uint16_t maxHeight = USE_SOURCE_DIMENSION)
		{
			createFromDisk(q, srcpath, (VkImageUsageFlagBits)usages, (VkImageLayout)layout, memspace, mipLevels, maxWidth, maxHeight);
		}



		void createFromDisk(Queue& q, std::filesystem::path srcpath, const VkImageUsageFlagBits usages,
			const VkImageLayout layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u, const uint16_t maxWidth = USE_SOURCE_DIMENSION, const uint16_t maxHeight = USE_SOURCE_DIMENSION);

		inline void create(Queue& q, const uint16_t width, const uint16_t height, VkFormat format, const IMAGE_USAGE usages,
			const IMAGE_LAYOUT layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u)
		{
			create(q, width, height, format, VkImageUsageFlagBits(usages), VkImageLayout(layout), memspace, mipLevels);
		}

		void create(Queue& q, const uint16_t width, const uint16_t height, VkFormat format, const VkImageUsageFlagBits usages,
			const VkImageLayout layout, const BUFFER_SPACE memspace = GPU_ONLY, const uint8_t mipLevels = 1u);

		void destroy(ValProc& proc);

	protected:

		void generateMipmaps(Queue& q, const uint8_t mipLevels);

	protected:

		friend ImageView;

		stbi_uc* _pixels = NULL;
		VkImage _img = VK_NULL_HANDLE;
		VkDeviceMemory _imgMemory = VK_NULL_HANDLE;

		VkImageLayout _layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkFormat _format = VK_FORMAT_R8G8B8A8_SRGB;

		uint16_t _width = 0u;
		uint16_t _height = 0u;
		
		uint8_t _channels = 0u;
		uint8_t _mipLevels = 0u;
	};
}

#endif // !VAL_TEXTURE_2D_HPP