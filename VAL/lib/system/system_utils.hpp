/*
Copyright � 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the �Software�), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef VAL_SYSTEM_UTILS_HPP
#define VAL_SYSTEM_UTILS_HPP


// for C++ class enums

#define DEF_ENUM_BITWISE_OR(TYPE) inline TYPE operator|(TYPE a, TYPE b) { return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) | static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_AND(TYPE) inline TYPE operator&(TYPE a, TYPE b) { return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) & static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_XOR(TYPE) inline TYPE operator^(TYPE a, TYPE b) { return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) ^ static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_NOT(TYPE) inline TYPE operator~(TYPE a) { return static_cast<TYPE>(~static_cast<std::underlying_type_t<TYPE>>(a));}

#define DEF_ENUM_BITWISE_OR_ASSIGN(TYPE) inline TYPE operator|=(TYPE& a, TYPE& b) { return a = static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) | static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_AND_ASSIGN(TYPE) inline TYPE operator&=(TYPE& a, TYPE& b) { return a = static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) & static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_XOR_ASSIGN(TYPE) inline TYPE operator^=(TYPE& a, TYPE& b) { return a = static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) ^ static_cast<std::underlying_type_t<TYPE>>(b));}

#define DEF_ENUM_BITWISE_OPERATORS(TYPE)\
DEF_ENUM_BITWISE_OR(TYPE)\
DEF_ENUM_BITWISE_AND(TYPE)\
DEF_ENUM_BITWISE_XOR(TYPE)\
DEF_ENUM_BITWISE_NOT(TYPE)\
DEF_ENUM_BITWISE_OR_ASSIGN(TYPE)\
DEF_ENUM_BITWISE_AND_ASSIGN(TYPE)\
DEF_ENUM_BITWISE_XOR_ASSIGN(TYPE)


#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>

#include <ExternalLibraries/stb_image.h>;
#include <optional>
#include <array>
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

#include <VAL/lib/debugReporting/debugCallbacks.hpp>

#include <VAL/lib/ext/tiny_vector.hpp>

#include <VAL/lib/system/descriptorBinding.hpp>
#include <VAL/lib/system/pipelineType.hpp>

#include <VAL/lib/system/physicalDeviceRequirements.hpp>

#include <VAL/lib/pipelineStateInfos/stateInfoEnums.hpp>


namespace val {

	// VAL::DYNAMIC_STATE maps directly to VkDynamicState
	enum class DYNAMIC_STATE
	{ // https://registry.khronos.org/vulkan/specs/latest/man/html/VkDynamicState.html
		SCISSOR = VK_DYNAMIC_STATE_SCISSOR,
		VIEWPORT = VK_DYNAMIC_STATE_VIEWPORT,
		LINE_WIDTH = VK_DYNAMIC_STATE_LINE_WIDTH,
		DEPTH_BIAS = VK_DYNAMIC_STATE_DEPTH_BIAS,
		DEPTH_BOUNDS = VK_DYNAMIC_STATE_DEPTH_BOUNDS,
		BLEND_CONSTANTS = VK_DYNAMIC_STATE_BLEND_CONSTANTS,
		/*Vk 1.3 and up:*/
		CULL_MODE = VK_DYNAMIC_STATE_CULL_MODE, 
		TOPOLOGY = VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY
	};

	enum bufferSpace : uint8_t {
		GPU_ONLY = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		CPU_GPU = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
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

	//bool isDeviceSuitable(VkPhysicalDevice device, std::vector<const char*>& deviceExtensions, VkSurfaceKHR surface);

	//VkSurfaceFormatKHR findSurfaceImageFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkFormat findSupportedImageFormat(VAL_PROC& proc, imageFormatRequirements& requirements);

	VkFormat findSupportedImageFormat(VkPhysicalDevice physicalDevice, imageFormatRequirements& requirements);

	VkPhysicalDevice findOptimalPhysicalDevice(VAL_PROC& proc, physicalDeviceRequirements& requirements, VkSurfaceKHR surface);

	VkPhysicalDevice findOptimalPhysicalDevice(VkInstance vkInstance, physicalDeviceRequirements& requirements, VkSurfaceKHR surface);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(GLFWwindow* windowHDL, const VkSurfaceCapabilitiesKHR& capabilities);

	VkImageView createImageView(VkDevice device, VkImage image, const VkFormat& format, const uint32_t& mipLevels = 1U);

	VkImage createTextureImage(VAL_PROC* proc, fs::path imgFilepath, stbi_uc** pixelsOut, VkFormat format, VkDeviceMemory& textureImageMemory,
		const VkImageUsageFlagBits& additionalUsageFlagBits = VkImageUsageFlagBits(0), const uint32_t& mipLevels = 1U,
		int* texWidthOut = NULL, int* texHeightOut = NULL, uint8_t* texChannelsOut = NULL, const bufferSpace& bufferSpace = GPU_ONLY);
	
	// returns false if the file cannot be read
	bool readByteFile(const std::string& filename, std::vector<char>* dst);

	bool readByteFile(const std::string& filename, tiny_vector<char>* dst);

	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkMemoryPropertyFlags bufferSpaceToVkMemoryProperty(const bufferSpace& bufferSpace);
}

#endif // !VAl_SYSTEM_UTILS