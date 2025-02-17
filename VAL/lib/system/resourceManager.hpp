#ifndef FML_RESOURCE_MANAGER_HPP
#define FML_RESOURCE_MANAGER_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>

namespace val {
	class resourceManager {
	public:
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		std::vector<VkBuffer> _uniformBuffers;
		std::vector<VkDeviceMemory> _uniformBuffersMemory;
		std::vector<void*> _uniformBuffersMapped;
	};
}

#endif // !FML_RESOURCE_MANAGER_HPP