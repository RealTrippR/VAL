#ifndef VAL_FRAMES_IN_FLIGHT_MANAGER_HPP
#define VAL_FRAMES_IN_FLIGHT_MANAGER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <VAL/lib/system/system_utils.hpp>
#include <vector>
namespace val {
	class framesInFlightManager {
        void create(VkDevice deviceIN, const uint32_t maxFrames);
        void incrementCurrentFrame();
    /////////////////////////////////////////////////////////////
        VkDevice device;
        uint32_t maxFramesInFlight;
        uint32_t currentFrame = 0; // the index of the current frame

        std::vector<VkFence> inFlightFences;
	};
}
#endif // !VAL_FRAMES_IN_FLIGHT_MANAGER_HPP