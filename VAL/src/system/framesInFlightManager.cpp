#include <VAL/lib/system/framesInFlightManager.hpp>

namespace val {
	void framesInFlightManager::create(VkDevice deviceIN, const uint32_t maxFrames) {
		device = deviceIN;
		maxFramesInFlight = maxFrames;

	}

	void framesInFlightManager::incrementCurrentFrame() {
		currentFrame = (currentFrame + 1) % maxFramesInFlight;

	}

}