#ifndef VAL_QUEUE_HANDLER_HPP
#define VAL_QUEUE_HANDLER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>

namespace val
{

	class VAL_PROC; // forward declaration

	class queueManager {
	public:
		uint32_t findQueueFamilyFromQueueFlags(VkPhysicalDevice physicalDevice,
			bool isPresentQueue=false, VkSurfaceKHR surface=VK_NULL_HANDLE);

		void create(VAL_PROC& proc, bool semaphoresNeeded, bool fencesNeeded);

		VkDeviceQueueCreateInfo getQueueCreateInfo();

		void destroy(VAL_PROC& proc);

	public:
		VkQueue _queue;
		VkQueueFlags _queueFlags;
		uint32_t _queueFamily;

		std::vector<VkCommandBuffer> _commandBuffers;

		std::vector<VkSemaphore> _semaphores;
		std::vector<VkFence> _fences;

	};
}
#endif // !VAL_QUEUE_HANDLER_HPP