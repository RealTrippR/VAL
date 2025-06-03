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
	class imageView; // forward declaration

	class texture2d {
	public:
		texture2d(VAL_PROC& proc) : _proc(proc) {};

		texture2d(VAL_PROC& proc, const uint16_t width, const uint16_t height, const VkFormat format,
			const VkImageUsageFlagBits usages, const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 1u) : _proc(proc)
		{
			create(width, height, format, usages, layout, memspace, mipLevels);
		}

		texture2d(VAL_PROC& proc, std::filesystem::path srcpath, const VkFormat format,
			const VkImageUsageFlagBits usages, const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 1u) : _proc(proc)
		{
			create(srcpath, format, usages, layout, memspace, mipLevels);
		}
	public:

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

		void create(const uint16_t width, const uint16_t height, const VkFormat format, const VkImageUsageFlagBits usages,
			const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 0u);

		void create(std::filesystem::path srcpath, const VkFormat format, const VkImageUsageFlagBits usages,
			const VkImageLayout layout, const bufferSpace memspace = GPU_ONLY, const uint8_t mipLevels = 0u);

		void destroy();

	protected:

		void generateMipmaps(const uint8_t mipLevels);

	protected:

		friend imageView;

		stbi_uc* _pixels = NULL;
		VkImage _img = VK_NULL_HANDLE;
		VkDeviceMemory _imgMemory = VK_NULL_HANDLE;
		VAL_PROC& _proc;

		VkImageLayout _layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		VkFormat _format = VK_FORMAT_UNDEFINED;

		uint16_t _width = 0u;
		uint16_t _height = 0u;
		
		uint8_t _channels = 0u;
		uint8_t _mipLevels = 0u;
	};
}

#endif // !VAL_TEXTURE_2D_HPP