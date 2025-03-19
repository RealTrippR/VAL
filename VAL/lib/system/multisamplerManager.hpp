#ifndef VAL_MULTISAMPLER_MANAGER_HPP
#define VAL_MULTISAMPLER_MANAGER_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	class multisamplerManager
	{
	public:
		multisamplerManager(VAL_PROC& proc) { _procVAL = &proc; }
		multisamplerManager(VAL_PROC& proc, const VkSampleCountFlagBits& sampleCount) { _procVAL = &proc; _sampleCount = sampleCount; }

	public:
		void create(const VkFormat& imgFormat, const uint32_t& width, const uint32_t& height, const VkImageAspectFlagBits& flagBits = VK_IMAGE_ASPECT_COLOR_BIT, const bufferSpace& space = GPU_ONLY, const uint8_t& mipMapLevel = 1u);

		void destroy();
	public:
		const VkImage& getVkImage();

		const VkDeviceMemory& getImageMemory();

		const VkImageView& getVkImageView();

		void setSampleCount(const VkSampleCountFlagBits& samples);

		const VkSampleCountFlagBits& getSampleCount();

		void setVAL_PROC(VAL_PROC& proc);

		VAL_PROC& getVAL_PROC();
	protected:
		VAL_PROC* _procVAL;
		/********************************/
		VkImage _colorImage = VK_NULL_HANDLE;
		VkDeviceMemory _colorImageMemory = VK_NULL_HANDLE;
		VkImageView _colorImageView = VK_NULL_HANDLE;

		VkSampleCountFlagBits _sampleCount;
	};
}

#endif // !VAL_MULTISAMPLER_MANAGER_HPP