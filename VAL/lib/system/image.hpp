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

#ifndef VAL_IMAGE_HPP
#define VAL_IMAGE_HPP

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <ExternalLibraries/stb_image.h>;

#include <VAL/lib/system/VAL_PROC.hpp>
#include <filesystem>
#include <stdexcept>
#include <stdlib.h>

namespace val {
	class image {
	public:
		image() = default;
		image(VAL_PROC& proc, const std::filesystem::path path, const VkFormat& format, uint8_t mipLevels = 1U, VkSampleCountFlagBits MSAA_samples = VK_SAMPLE_COUNT_1_BIT) : image() {
			create(proc, path, format, mipLevels, MSAA_samples);
		}
		image(VAL_PROC& proc, const std::filesystem::path path, const VkFormat& format, VkImageLayout imgLayout, uint8_t mipLevels = 1U, VkSampleCountFlagBits MSAA_samples = VK_SAMPLE_COUNT_1_BIT) : image() {
			_imgLayout = imgLayout;
			create(proc, path, format, mipLevels, MSAA_samples);
		}
		~image() {
			destroy();
		}

		void recreate(VAL_PROC& proc, const std::filesystem::path path, const VkFormat& format, const uint8_t& mipLevels = 1U);

		inline const stbi_uc* pixels() {
			return _pixels;
		}

		inline VkImage& getImage() {
			return _image;
		}

		inline const int32_t& getWidth() {
			return _width;
		}

		inline const int32_t& getHeight() {
			return _height;
		}

		inline const uint8_t& getChannels() {
			return _channels;
		}

		inline const VkFormat& getFormat() {
			return _format;
		}

		inline VkDeviceMemory& getMemory() {
			return _img_memory;
		}

		inline const uint8_t getMipLevels() {
			return _mipLevels;
		}

		inline const void destroy() {
			if (_img_memory) {
				vkDestroyImage(_device, _image, NULL);
				vkFreeMemory(_device, _img_memory, NULL);
				_img_memory = NULL;
			}
			if (_pixels) {
				stbi_image_free(_pixels);
				_pixels = NULL;
			}
		}

		void transitionImgLayout(VAL_PROC& proc, VkCommandBuffer cmdbuff, VkImageLayout newLayout);

		inline const VkImageLayout getLayout() {
			return _imgLayout;
		}
	public:

		void create(VAL_PROC& proc, const std::filesystem::path path, const VkFormat& format, const uint8_t& mipLevels = 1U, const VkSampleCountFlagBits& MSAA_samples = VK_SAMPLE_COUNT_1_BIT);

		void generateMipmaps(VAL_PROC& proc, const uint8_t mipLevels);

	protected:
		stbi_uc* _pixels = NULL;
		VkImage _image{};
		VkImageLayout _imgLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		VkFormat _format{};
		int32_t _width{}; // source image width
		int32_t _height{}; // source image height
		uint8_t _channels{};
		uint8_t _mipLevels{};
		VkDeviceMemory _img_memory{};
		VkDevice _device{};
	};
}
#endif // !VAL_IMAGE_HPP