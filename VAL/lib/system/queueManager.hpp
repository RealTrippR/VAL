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

		inline void submit(const uint32_t frameidx, const VkCommandBuffer& cmdBuff, VkFence& fence, queueManager& waitFor);

		inline VkQueue getVkQueue();

		inline VkQueueFlags getVkQueueFlags() const;

		inline uint32_t getQueueFamily() const;

		inline VkCommandBuffer& getCommandBuffer(const uint32_t& frameIdx);

		inline VkSemaphore& getSemaphore(const uint32_t& frameIdx);
	public:
		VkQueue _queue;
		VkQueueFlags _queueFlags;
		uint32_t _queueFamily;

		tiny_vector<VkCommandBuffer> _commandBuffers;

		tiny_vector<VkSemaphore> _semaphores;
		tiny_vector<VkFence> _fences;

	};
}
#endif // !VAL_QUEUE_HANDLER_HPP



#include <VAL/lib/system/queueManager.inl>