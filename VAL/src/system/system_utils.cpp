#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <ExternalLibraries/stb_image.h>;

namespace val {
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::cerr << "Validation layers: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != NULL) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	queueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
	{
		queueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			// Check for graphics queue support
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			/*if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
				indices.computeFamily = i;
			}

			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.computeAndGraphicsFamily = i;
			}*/

			// Check for presentation queue support only if surface is provided
			if (surface != NULL) {
				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

				if (presentSupport) {
					indices.presentFamily = i;
				}
			}

			// Exit early if all required queues are found
			if (surface != NULL) {
				if (indices.graphicsFamily.has_value() && indices.presentFamily.has_value()) {
					break;
				}
			}
			else
			{
				if (indices.graphicsFamily.has_value()) {
					break;
				}
			}

			i++;
		}
		return indices;
	}

	std::vector<const char*> getRequiredExtensions(const bool& enableValidationLayers)
	{
		unsigned int glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	VkSurfaceKHR createSurfaceKHR(VkInstance instance, GLFWwindow* window) {
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO CREATE A VULKAN WINDOW SURFACE!");
		}
		return surface;
	}

	bool checkValidationLayerSupport(std::vector<const char*>& validationLayers)
	{
		unsigned int layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}
		return true;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*>& deviceExtensions)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		swapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	bool isDeviceSuitable(VkPhysicalDevice device, std::vector<const char*>& deviceExtensions, VkSurfaceKHR surface) {
		queueFamilyIndices indices = findQueueFamilies(device, surface);

		bool extensionsSupported = checkDeviceExtensionSupport(device, deviceExtensions);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			if (surface) {
				swapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
				swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			}
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.graphicsFamily.has_value() && extensionsSupported &&
			((swapChainAdequate && indices.presentFamily.has_value()) || !surface);
	}

	/*VkSurfaceFormatKHR findSurfaceImageFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}*/

	VkFormat findSupportedImageFormat(VkPhysicalDevice physicalDevice, imageFormatRequirements& requirements) {
#ifndef NDEBUG
		if (!physicalDevice) {
			printf("VAL: Could not find a supported image format, because the devices have not yet been initialized!\n");
			throw std::runtime_error("VAL: Could not find a supported image format, because the devices have not yet been initialized!");
		}
#endif // !NDEBUG

		for (VkFormat format : requirements.acceptedFormats) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (requirements.tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & requirements.features) == requirements.features) {
				return format;
			}
			else if (requirements.tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & requirements.features) == requirements.features) {
				return format;
			}
		}
		throw std::runtime_error("FAILED TO FIND A SUPPORTED IMAGE FORMAT");
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(GLFWwindow* windowHDL, const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(windowHDL, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	VkImageView createImageView(VkDevice device, VkImage image, const VkFormat& format, const uint32_t& mipLevels /*DEFAULT=1U*/) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("FML: FAILED TO CREATE TEXTURE VIEW");
		}

		return imageView;
	}

	VkImage createTextureImage(VAL_PROC* proc, fs::path imgFilepath, stbi_uc* pixelsOut, VkFormat format,
		VkDeviceMemory& textureImageMemory, const VkImageUsageFlagBits& additionalUsageFlagBits, const uint32_t& mipLevels,
		int* texWidthOut /*NULL BY DEFAULT*/, int* texHeightOut /*NULL BY DEFAULT*/, uint8_t* texChannelsOut /*NULL BY DEFAULT*/) {
		VkImage textureImage = NULL;
		int texWidth, texHeight, texChannels;
		pixelsOut = stbi_load(imgFilepath.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * (texChannels + 1);

		if (!pixelsOut) {
			printf("VAL: FAILED TO LOAD TEXTURE IMAGE, IMAGE AT FILEPATH: %s IS OF AN UNSUPPORTED TYPE OR DOES NOT EXIST!\n", imgFilepath.c_str());
			throw std::runtime_error("FML: FAILED TO LOAD TEXTURE IMAGE");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		proc->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(proc->_device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixelsOut, static_cast<size_t>(imageSize));
		vkUnmapMemory(proc->_device, stagingBufferMemory);

		stbi_image_free(pixelsOut);

		proc->createImage(texWidth, texHeight, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			VK_IMAGE_USAGE_SAMPLED_BIT | additionalUsageFlagBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory, mipLevels);

		proc->transitionImageLayout(textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, NULL, mipLevels);
		proc->copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

		vkDestroyBuffer(proc->_device, stagingBuffer, nullptr);
		vkFreeMemory(proc->_device, stagingBufferMemory, nullptr);

#ifndef NDEBUG
		if ((texWidthOut || texHeightOut) && !(texWidthOut && texHeightOut)) {
			printf("VAL: If either texWidthOut or texHeightOut is a valid pointer, both must be valid, but only 1 was passed as an input to createTextureImage.\n");
			throw std::logic_error("VAL: If either texWidthOut or texHeightOut is a valid pointer, both must be valid, but only 1 was passed as an input to createTextureImage.");
		}
#endif // !NDEBUG

		if (texWidthOut) {
			*texWidthOut = texWidth;
			*texHeightOut = texHeight;
		}
		if (texChannelsOut) {
			*texChannelsOut = texChannels;
		}
		return textureImage;
	}

	bool readByteFile(const std::string& filename, std::vector<char>* dst) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			return false;
		}

		size_t fileSize = (size_t)file.tellg();
		dst->resize(fileSize);

		file.seekg(0);
		file.read(dst->data(), fileSize);

		file.close();

		return true;
	}

	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("FAILED TO FIND SUITABLE MEMORY TYPE!");
	}

	VkMemoryPropertyFlags bufferSpaceToVkMemoryProperty(const bufferSpace& bufferSpace) {
		switch (bufferSpace) {
		case GPU_ONLY:
			return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		case CPU_GPU:
			return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			// this can be optimized, some GPUs support VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			// it would be smart to use this memory property whenever supported.
		}
	}
}