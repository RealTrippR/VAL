#ifndef VAL_QUEUE_HANDLER_HPP
#define VAL_QUEUE_HANDLER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>
#include <VAL/lib/ext/tiny_vector.hpp>

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

		tiny_vector<VkCommandBuffer, uint32_t> _commandBuffers;

		tiny_vector<VkSemaphore, uint32_t> _semaphores;
		tiny_vector<VkFence, uint32_t> _fences;

	};
}
#endif // !VAL_QUEUE_HANDLER_HPP