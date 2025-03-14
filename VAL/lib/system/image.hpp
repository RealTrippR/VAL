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

		~image() {
			if (_img_memory) {
				vkDestroyImage(_device, _image, NULL);
				vkFreeMemory(_device, _img_memory, NULL);
				_img_memory = NULL;
			}
			if (_pixels) {
				stbi_image_free(_pixels);
			}
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
			vkDestroyImage(_device, _image, NULL);
			vkFreeMemory(_device, _img_memory, NULL);
		}

	public:

		void create(VAL_PROC& proc, const std::filesystem::path path, const VkFormat& format, const uint8_t& mipLevels = 1U, const VkSampleCountFlagBits& MSAA_samples = VK_SAMPLE_COUNT_1_BIT);

		void generateMipmaps(VAL_PROC& proc, const uint8_t mipLevels);

	protected:
		stbi_uc* _pixels = NULL;
		VkImage _image{};
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