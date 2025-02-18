#ifndef VAL_QUEUE_HANDLER_HPP
#define VAL_QUEUE_HANDLER_HPP


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>

namespace val
{
	class queueManager {
	public:
		uint32_t findQueueFamilyFromQueueFlags(VkPhysicalDevice physicalDevice);
		void create(const uint32_t queueFamily, const uint16_t framesInFlight, bool semaphoresNeeded, bool fencesNeeded);
	public:
		VkQueue _queue;
		VkQueueFlags _queueFlags;
		uint32_t _queueFamily;
		std::optional<std::vector<VkSemaphore>> _semaphores;
		std::optional<std::vector<VkSemaphore>> _inFlightFences;

	};
}
#endif // !VAL_QUEUE_HANDLER_HPP