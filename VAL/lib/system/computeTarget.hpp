#ifndef VAL_COMPUTE_TARGET_HPP
#define VAL_COMPUTE_TARGET_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/computePipelineCreateInfo.hpp>

namespace val {
	class queueManager; // forward declaration
	class computeTarget {
	public:
		void compute(ValProc& proc, const uint32_t& groupCountX, const uint32_t& groupCountY, const uint32_t& groupCountZ);

		void update(ValProc& proc, computePipelineCreateInfo& computePipeline);

		void begin(ValProc& proc);
		
		void submit(ValProc& proc, std::vector<VkSemaphore> waitSemaphores, VkFence fence = VK_NULL_HANDLE);
	public:

	};
}
#endif // !VAL_COMPUTE_TARGET_HPP
