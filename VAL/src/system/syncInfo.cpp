#include <VAL/lib/system/syncInfo.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void syncInfo::init(const VAL_PROC& proc, const std::vector<queueManager*>& signal, const std::vector<queueManager*>& waitFor, pipelineCreateInfo* waitPipelineInfo) {

		waitStages = waitPipelineInfo->getShaderStages();


		waitSemaphores.clear();
		waitSemaphores.resize(proc._MAX_FRAMES_IN_FLIGHT);
		waitFences.clear();
		waitFences.resize(proc._MAX_FRAMES_IN_FLIGHT);
		signalSemaphores.clear();
		signalSemaphores.resize(proc._MAX_FRAMES_IN_FLIGHT);
		signalFences.clear();
		signalFences.resize(proc._MAX_FRAMES_IN_FLIGHT);

		// init signal semaphores
		for (size_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; ++i) {
			for (size_t j = 0; j < signal.size(); ++j) {
				if (signal[j]->_semaphores.size() > 0) {
					signalSemaphores[i].push_back(signal[j]->_semaphores[i]);
				}
			}
		}
		// init signal fences
		for (size_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; ++i) {
			for (size_t j = 0; j < signal.size(); ++j) {
				if (signal[j]->_fences.size() > 0) {
					signalFences[i].push_back(signal[j]->_fences[i]);
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////
		// init wait seamphores
		for (size_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; ++i) {
			for (size_t j = 0; j < waitFor.size(); ++j) {
				if (waitFor[j]->_fences.size() > 0) {
					waitSemaphores[i].push_back(waitFor[j]->_semaphores[i]);
				}
			}
		}
		// init wait fences
		for (size_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; ++i) {
			for (size_t j = 0; j < waitFor.size(); ++j) {
				if (waitFor[j]->_fences.size() > 0) {
					waitFences[i].push_back(waitFor[j]->_fences[i]);
				}
			}
		}
	}
}