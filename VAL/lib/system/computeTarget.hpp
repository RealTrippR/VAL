#ifndef VAL_COMPUTE_TARGET_HPP
#define VAL_COMPUTE_TARGET_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/computePipelineCreateInfo.hpp>

namespace val {
	class QueueManager; // forward declaration
	class computeTarget {
	public:
		void compute(Queue& computeQueue, const uint32_t& groupCountX, const uint32_t& groupCountY, const uint32_t& groupCountZ);

		void update(Queue& computeQueue, computePipelineCreateInfo& computePipeline);

		void begin(Queue& computeQueue);
		
		void submit(Queue& computeQueue, std::vector<VkSemaphore> waitSemaphores, VkFence fence = VK_NULL_HANDLE);
	public:

	};
}
#endif // !VAL_COMPUTE_TARGET_HPP
