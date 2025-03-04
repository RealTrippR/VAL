#ifndef VAL_DEPTH_BUFFER_VAL
#define VAL_DEPTH_BUFFER_VAL

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	class depthBuffer
	{
	public:
		depthBuffer() = default;
		depthBuffer(VAL_PROC& proc, VkExtent2D extent, VkFormat depthFormat, size_t imgViewCount = 1u, uint8_t mipLevels = 1u, VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT) {
			create(proc, extent, depthFormat, imgViewCount, mipLevels, msaaSamples);
		}
	public:
		void create(VAL_PROC& proc, VkExtent2D extent, VkFormat depthFormat, size_t imgViewCount = 1u, uint8_t mipLevels = 1u, VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT);

		void destroy(VAL_PROC& proc);
	public:
		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		std::vector<VkImageView> imgViews;
	};
}
#endif // !VAL_DEPTH_BUFFER_VAL