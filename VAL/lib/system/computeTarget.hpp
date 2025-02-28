#ifndef VAL_COMPUTE_TARGET_HPP
#define VAL_COMPUTE_TARGET_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/computePipelineCreateInfo.hpp>

namespace val {
	class queueManager; // forward declaration
	class computeTarget {
	public:
		void compute(VAL_PROC& proc, queueManager& queue, const uint32_t& groupCountX, const uint32_t& groupCountY, const uint32_t& groupCountZ);

		void update(VAL_PROC& proc, queueManager& queue, computePipelineCreateInfo& computePipeline);

		
		void submit(VAL_PROC& proc, const std::vector<queueManager&>& signalQueues, const std::vector<queueManager&>& waitQueues, VkFence fence = VK_NULL_HANDLE);
	public:

	};
}
#endif // !VAL_COMPUTE_TARGET_HPP
