#ifndef VAL_SYSTEM_UTILS_HPP
#define VAL_SYSTEM_UTILS_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ExternalLibraries/stb_image.h>;
#include <optional>
#include <set>
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <math.h>

#include <VAL/lib/system/descriptorBinding.hpp>
#include <VAL/lib/system/pipelineType.hpp>

#include <VAL/lib/system/physicalDeviceRequirements.hpp>

namespace val {

	enum bufferSpace {
		GPU_ONLY,
		CPU_GPU
	};

	class VAL_PROC; // forward declaration

	namespace fs = std::filesystem;

	struct imageFormatRequirements
	{
		std::vector<VkFormat> acceptedFormats;
		std::vector<VkColorSpaceKHR> acceptedColorSpaces;
		VkImageTiling tiling;
		VkFormatFeatureFlags features;
	};

	struct queueFamilyIndices
	{
		std::optional<unsigned int> graphicsFamily;
		std::optional<unsigned int> presentFamily;
		/*std::optional<unsigned int> computeFamily;
		std::optional<unsigned int> computeAndGraphicsFamily;*/
		/*bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}*/
	};

	struct swapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef NDEBUG
	void __VAL_DEBUG_ValidateBufferCopy(uint64_t dstBufferSize, uint64_t dataSize, VkDeviceSize srcOffset, VkDeviceSize dstOffset);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void destroyDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);

#endif // !NDEBUG
	//////////////////////////////////////////////////////////////////////////////////////////////////


	queueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	std::vector<const char*> getRequiredExtensions(const bool& enableValidationLayers);

	bool checkValidationLayerSupport(std::vector<const char*>& validationLayers);

	bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*>& deviceExtensions);

	swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

	bool isDeviceSuitable(VkPhysicalDevice device, std::vector<const char*>& deviceExtensions, VkSurfaceKHR surface);

	//VkSurfaceFormatKHR findSurfaceImageFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkFormat findSupportedImageFormat(VAL_PROC& proc, imageFormatRequirements& requirements);

	VkFormat findSupportedImageFormat(VkPhysicalDevice physicalDevice, imageFormatRequirements& requirements);

	VkPhysicalDevice findOptimalPhysicalDevice(VAL_PROC& proc, physicalDeviceRequirements& requirements);

	VkPhysicalDevice findOptimalPhysicalDevice(VkInstance* vkInstance, physicalDeviceRequirements& requirements);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(GLFWwindow* windowHDL, const VkSurfaceCapabilitiesKHR& capabilities);

	VkImageView createImageView(VkDevice device, VkImage image, const VkFormat& format, const uint32_t& mipLevels = 1U);

	VkImage createTextureImage(VAL_PROC* proc, fs::path imgFilepath, stbi_uc** pixelsOut, VkFormat format, VkDeviceMemory& textureImageMemory,
		const VkImageUsageFlagBits& additionalUsageFlagBits = VkImageUsageFlagBits(0), const uint32_t& mipLevels = 1U,
		int* texWidthOut = NULL, int* texHeightOut = NULL, uint8_t* texChannelsOut = NULL);
	// returns false if the file cannot be read
	bool readByteFile(const std::string& filename, std::vector<char>* dst);

	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkMemoryPropertyFlags bufferSpaceToVkMemoryProperty(const bufferSpace& bufferSpace);
}

#endif // !VAl_SYSTEM_UTILS