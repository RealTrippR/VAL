#ifndef VAL_SYNC_INFO_HPP
#define VAL_SYNC_INFO_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/pipelineCreateInfo.hpp>
#include <VAL/lib/system/queueManager.hpp>
#include <vector>

namespace val {
	class queueManager; // forward definition

	class syncInfo {
	public:
		syncInfo() = default;

		syncInfo(const VAL_PROC& proc, const std::vector<queueManager*>& signal, const std::vector<queueManager*>& waitFor, pipelineCreateInfo* waitPipelineInfo) {
			init(proc, signal, waitFor, waitPipelineInfo);
		}

	public:

		void init(const VAL_PROC& proc, const std::vector<queueManager*>& signal, const std::vector<queueManager*>& waitFor, pipelineCreateInfo* pipelineInfo);

		// outer vector is per frame, inner vector is the semaphores for that frame
		std::vector<std::vector<VkSemaphore>> waitSemaphores;
		std::vector<std::vector<VkFence>> waitFences;
		std::vector<std::vector<VkSemaphore>> signalSemaphores;
		std::vector<std::vector<VkFence>> signalFences;
		std::vector<VkPipelineStageFlags> waitStages;
	};
}

#endif // !VAL_SYNC_INFO_HPP