#include <VAL/lib/system/multisamplerConfig.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val
{
	void MultisamplerConfig::create(const VkFormat& imgFormat, const uint32_t& width, const uint32_t& height, const VkImageAspectFlagBits& flagBits, const BUFFER_SPACE& space, const uint8_t& mipMapLevel) {
		_procVAL->createImage(width, height, imgFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			bufferSpaceToVkMemoryProperty(space), _colorImage, _colorImageMemory, mipMapLevel, _sampleCount);
		_procVAL->createImageView(_colorImage, imgFormat, VK_IMAGE_ASPECT_COLOR_BIT, &_colorImageView, mipMapLevel);
	}

	void MultisamplerConfig::create(const VkSampleCountFlagBits MSAA_samples, const VkFormat& imgFormat, const uint32_t& width, const uint32_t& height, const VkImageAspectFlagBits& flagBits, const BUFFER_SPACE& space, const uint8_t& mipMapLevel) {
		
		setSampleCount(MSAA_samples);

		_procVAL->createImage(width, height, imgFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			bufferSpaceToVkMemoryProperty(space), _colorImage, _colorImageMemory, mipMapLevel, _sampleCount);
		_procVAL->createImageView(_colorImage, imgFormat, VK_IMAGE_ASPECT_COLOR_BIT, &_colorImageView, mipMapLevel);
	}


	void MultisamplerConfig::destroy() {
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

	const VkImage& MultisamplerConfig::getVkImage() {
		return _colorImage;
	}

	const VkDeviceMemory& MultisamplerConfig::getImageMemory() {
		return _colorImageMemory;
	}

	const VkImageView& MultisamplerConfig::getVkImageView() {
		return _colorImageView;
	}

	void MultisamplerConfig::setSampleCount(const VkSampleCountFlagBits& samples) {
		_sampleCount = samples;
	}

	const VkSampleCountFlagBits& MultisamplerConfig::getSampleCount() {
		return _sampleCount;
	}

	void MultisamplerConfig::setValProc(ValProc& proc) {
		_procVAL = &proc;
	}

	ValProc& MultisamplerConfig::getValProc() {
		return *_procVAL;
	}
}