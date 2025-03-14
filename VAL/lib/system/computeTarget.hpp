#ifndef VAL_COMPUTE_TARGET_HPP
#define VAL_COMPUTE_TARGET_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/computePipelineCreateInfo.hpp>

namespace val {
	class queueManager; // forward declaration
	class computeTarget {
	public:
		void compute(VAL_PROC& proc, const uint32_t& groupCountX, const uint32_t& groupCountY, const uint32_t& groupCountZ);

		void update(VAL_PROC& proc, computePipelineCreateInfo& computePipeline);

		void begin(VAL_PROC& proc);
		
		void submit(VAL_PROC& proc, std::vector<VkSemaphore> waitSemaphores, VkFence fence = VK_NULL_HANDLE);
	public:

	};
}
#endif // !VAL_COMPUTE_TARGET_HPP
