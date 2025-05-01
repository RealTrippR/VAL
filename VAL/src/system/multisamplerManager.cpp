#include <VAL/lib/system/multisamplerManager.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val
{
	void multisamplerManager::create(const VkFormat& imgFormat, const uint32_t& width, const uint32_t& height, const VkImageAspectFlagBits& flagBits, const bufferSpace& space, const uint8_t& mipMapLevel) {
		_procVAL->createImage(width, height, imgFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			bufferSpaceToVkMemoryProperty(space), _colorImage, _colorImageMemory, mipMapLevel, _sampleCount);
		_procVAL->createImageView(_colorImage, imgFormat, VK_IMAGE_ASPECT_COLOR_BIT, &_colorImageView, mipMapLevel);
	}

	void multisamplerManager::create(const VkSampleCountFlagBits MSAA_samples, const VkFormat& imgFormat, const uint32_t& width, const uint32_t& height, const VkImageAspectFlagBits& flagBits, const bufferSpace& space, const uint8_t& mipMapLevel) {
		
		setSampleCount(MSAA_samples);

		_procVAL->createImage(width, height, imgFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			bufferSpaceToVkMemoryProperty(space), _colorImage, _colorImageMemory, mipMapLevel, _sampleCount);
		_procVAL->createImageView(_colorImage, imgFormat, VK_IMAGE_ASPECT_COLOR_BIT, &_colorImageView, mipMapLevel);
	}


	void multisamplerManager::destroy() {
		const auto& device = _procVAL->_device;
		if (_colorImage) {
			vkDestroyImage(device, _colorImage, VK_NULL_HANDLE);
			vkFreeMemory(device, _colorImageMemory, VK_NULL_HANDLE);
			vkDestroyImageView(device, _colorImageView, VK_NULL_HANDLE);

			_colorImage = VK_NULL_HANDLE;
			_colorImageMemory = VK_NULL_HANDLE;
			_colorImageView = VK_NULL_HANDLE;
		}
	}

	const VkImage& multisamplerManager::getVkImage() {
		return _colorImage;
	}

	const VkDeviceMemory& multisamplerManager::getImageMemory() {
		return _colorImageMemory;
	}

	const VkImageView& multisamplerManager::getVkImageView() {
		return _colorImageView;
	}

	void multisamplerManager::setSampleCount(const VkSampleCountFlagBits& samples) {
		_sampleCount = samples;
	}

	const VkSampleCountFlagBits& multisamplerManager::getSampleCount() {
		return _sampleCount;
	}

	void multisamplerManager::setVAL_PROC(VAL_PROC& proc) {
		_procVAL = &proc;
	}

	VAL_PROC& multisamplerManager::getVAL_PROC() {
		return *_procVAL;
	}
}