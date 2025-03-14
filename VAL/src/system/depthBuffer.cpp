#include <VAL/lib/system/depthBuffer.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void depthBuffer::create(VAL_PROC& proc, VkExtent2D extent, VkFormat depthFormat, size_t imgViewCount, uint8_t mipLevels, VkSampleCountFlagBits msaaSamples)
	{
		proc.createImage(extent.width, extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory, mipLevels, msaaSamples);
		imgViews.resize(imgViewCount);
		for (size_t i = 0; i < imgViewCount; ++i) {
			proc.createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &imgViews[i]);
		}
	}

	void depthBuffer::destroy(VAL_PROC& proc) {
		vkDestroyImage(proc._device, depthImage, VK_NULL_HANDLE);
		for (size_t i = 0; i < imgViews.size(); ++i) {
			vkDestroyImageView(proc._device, imgViews[i], VK_NULL_HANDLE);
		}
		vkFreeMemory(proc._device, depthImageMemory, VK_NULL_HANDLE);
	}
}