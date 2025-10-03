#ifndef VAL_MULTISAMPLER_MANAGER_HPP
#define VAL_MULTISAMPLER_MANAGER_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	class MultisamplerConfig
	{
	public:
		MultisamplerConfig(ValProc& proc) { _procVAL = &proc; }
		MultisamplerConfig(ValProc& proc, const VkSampleCountFlagBits& sampleCount) { _procVAL = &proc; _sampleCount = sampleCount; }

	public:
		
		void create(Queue& q, const VkFormat& imgFormat, const uint32_t& width, const uint32_t& height, const VkImageAspectFlagBits& flagBits = VK_IMAGE_ASPECT_COLOR_BIT, const BUFFER_SPACE& space = GPU_ONLY, const uint8_t& mipMapLevel = 1u);
		void create(Queue& q, const VkSampleCountFlagBits MSAA_samples, const VkFormat& imgFormat, const uint32_t& width, const uint32_t& height, const VkImageAspectFlagBits& flagBits = VK_IMAGE_ASPECT_COLOR_BIT, const BUFFER_SPACE& space = GPU_ONLY, const uint8_t& mipMapLevel = 1u);

		void destroy();
	public:
		const VkImage& getVkImage();

		const VkDeviceMemory& getImageMemory();

		const VkImageView& getVkImageView();

		void setSampleCount(const VkSampleCountFlagBits& samples);

		const VkSampleCountFlagBits& getSampleCount();

		void setValProc(ValProc& proc);

		ValProc& getValProc();
	protected:
		ValProc* _procVAL;
		/********************************/
		VkImage _colorImage = VK_NULL_HANDLE;
		VkDeviceMemory _colorImageMemory = VK_NULL_HANDLE;
		VkImageView _colorImageView = VK_NULL_HANDLE;

		VkSampleCountFlagBits _sampleCount;
	};
}

#endif // !VAL_MULTISAMPLER_MANAGER_HPP