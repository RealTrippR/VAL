/*
Copyright © 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef VAL_SYSTEM_UTILS_HPP
#define VAL_SYSTEM_UTILS_HPP


#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>

#include <ExternalLibraries/stb_image.h>
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

#include <VAL/lib/system/ImageLayoutTransitionInfo.hpp>
#include <VAL/lib/debugReporting/debugCallbacks.hpp>

#include <VAL/lib/ext/tiny_vector.hpp>

#include <VAL/lib/system/descriptorBinding.hpp>
#include <VAL/lib/system/pipelineType.hpp>

#include <VAL/lib/system/physicalDeviceRequirements.hpp>

#include <VAL/lib/pipelineStateInfos/stateInfoEnums.hpp>

#include <VAL/lib/system/bufferUsageEnum.hpp>

#include <VAL/lib/system/shaderStageEnum.hpp>

#include <VAL/lib/debugUtils/dbgUtils.hpp>

#include <VAL/lib/descriptorSheets/objectDescriptorInfo.h>

#include <VAL/lib/system/imageViewBindInfo.hpp>

#include <VAL/lib/system/pipelineStages.hpp>

namespace val {

	constexpr uint16_t USE_SOURCE_DIMENSION = 0;
	constexpr VkFormat TEXTURE_FORMAT_AUTO = VK_FORMAT_MAX_ENUM;

	class Queue; // forward declaration
	class Window; // forward declaration
	class ValProc; // forward declaration
	class DescriptorSheet; // forward declaration

	// VAL::DYNAMIC_STATE maps directly to VkDynamicState
	enum class DYNAMIC_STATE
	{ // https://registry.khronos.org/vulkan/specs/latest/man/html/VkDynamicState.html
		Scissor = VK_DYNAMIC_STATE_SCISSOR,
		Viewport = VK_DYNAMIC_STATE_VIEWPORT,
		LineWidth = VK_DYNAMIC_STATE_LINE_WIDTH,
		DepthBias = VK_DYNAMIC_STATE_DEPTH_BIAS,
		DepthBounds = VK_DYNAMIC_STATE_DEPTH_BOUNDS,
		BlendConstants = VK_DYNAMIC_STATE_BLEND_CONSTANTS,
		/*Vk 1.3 and up:*/
		CullMode = VK_DYNAMIC_STATE_CULL_MODE, 
		Topology = VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY
	};

	enum class DEPENDENCY_FLAGS {
		None = 0x0,
		ByRegion = VK_DEPENDENCY_BY_REGION_BIT,
		DeviceGroup = VK_DEPENDENCY_DEVICE_GROUP_BIT,
		ViewLocal = VK_DEPENDENCY_VIEW_LOCAL_BIT,
		FeedbackLoop = VK_DEPENDENCY_FEEDBACK_LOOP_BIT_EXT,
		QueueFamilyOwnershipTransferUseAllStages = VK_DEPENDENCY_QUEUE_FAMILY_OWNERSHIP_TRANSFER_USE_ALL_STAGES_BIT_KHR
	};
	#ifndef DEPENDENCY_FLAGS_DEF_ENUM_BITWISE_OPERATORS
	#define DEPENDENCY_FLAGS_DEF_ENUM_BITWISE_OPERATORS
		DEF_ENUM_BITWISE_OPERATORS(DEPENDENCY_FLAGS);
	#endif

	enum class IMAGE_ASPECT 
	{
		None = VK_IMAGE_ASPECT_NONE,
		Color = VK_IMAGE_ASPECT_COLOR_BIT,
		Depth = VK_IMAGE_ASPECT_DEPTH_BIT,
		Stencil = VK_IMAGE_ASPECT_STENCIL_BIT,
		Metadata = VK_IMAGE_ASPECT_METADATA_BIT,
		Plane0 = VK_IMAGE_ASPECT_PLANE_0_BIT,
		Plane1 = VK_IMAGE_ASPECT_PLANE_1_BIT,
		Plane2 = VK_IMAGE_ASPECT_PLANE_2_BIT,
		MemoryPlane0 = VK_IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT,
		MemoryPlane1 = VK_IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT,
		MemoryPlane2 = VK_IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT,
		MaxEnum = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM
	};

	#ifndef IMAGE_ASPECT_DEF_ENUM_BITWISE_OPERATORS
	#define IMAGE_ASPECT_DEF_ENUM_BITWISE_OPERATORS
		DEF_ENUM_BITWISE_OPERATORS(IMAGE_ASPECT);
	#endif
	enum class ACCESS_FLAGS 
	{
		None = VK_ACCESS_NONE,
		IndirectCommandRead = VK_ACCESS_INDIRECT_COMMAND_READ_BIT,
		IndexRead = VK_ACCESS_INDEX_READ_BIT,
		VertexAttributeRead = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
		UniformRead = VK_ACCESS_UNIFORM_READ_BIT,
		InputAttachmentRead = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
		ShaderRead = VK_ACCESS_SHADER_READ_BIT,
		ShaderWrite = VK_ACCESS_SHADER_WRITE_BIT,
		ColorAttachmentRead = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
		ColorAttachmentWrite = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		DepthStencilAttachmentRead = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
		DepthStencilAttachmentWrite = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		TransferRead = VK_ACCESS_TRANSFER_READ_BIT,
		TransferWrite = VK_ACCESS_TRANSFER_WRITE_BIT,
		HostRead = VK_ACCESS_HOST_READ_BIT,
		HostWrite = VK_ACCESS_HOST_WRITE_BIT,
		MemoryRead = VK_ACCESS_MEMORY_READ_BIT,
		MemoryWrite = VK_ACCESS_MEMORY_WRITE_BIT,
		TransformFeedbackWrite = VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT,
		TransformFeedbackCounterRead = VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT,
		TransformFeedbackCounterWrite = VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT,
		ConditionalRenderingRead = VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT,
		ColorAttachmentReadNonCoherent = VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT,
		AccelerationStructureRead = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR,
		AccelerationStructureWrite = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR,
		FragmentDensityMapRead = VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT,
		CommandPreprocessRead = VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV,
		CommandPreprocessWrite = VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV
	};
	#ifndef ACCESS_FLAGS_DEF_ENUM_BITWISE_OPERATORS
	#define ACCESS_FLAGS_DEF_ENUM_BITWISE_OPERATORS
		DEF_ENUM_BITWISE_OPERATORS(ACCESS_FLAGS);
	#endif

	
	enum class IMAGE_USAGE 
	{
		TransferSrc = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		TransferDst = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		Sampled = VK_IMAGE_USAGE_SAMPLED_BIT,
		Storage = VK_IMAGE_USAGE_STORAGE_BIT,
		ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		DepthStencilAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		TransientAttachment = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
		InputAttachment = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
		HostTransfer =VK_IMAGE_USAGE_HOST_TRANSFER_BIT,
		VideoDecodeDst = VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR,
		VideoDecodeSrc = VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR,
		VideoDecodeDpb = VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR,
		FragmentDensityMap = VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT,
		FragmentShadingRate = VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
		VideoEncodeDst = VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR,
		VideoEncodeSrc = VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR,
		VideoEncodeDpb = VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR,
		AttachmentFeedbackLoop = VK_IMAGE_USAGE_ATTACHMENT_FEEDBACK_LOOP_BIT_EXT,
		InvocationMaskHuawei = VK_IMAGE_USAGE_INVOCATION_MASK_BIT_HUAWEI,
		SampleWeightQCOM = VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM,
		SampleBlockMatchQCOM = VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM,
		QuantizationDeltaMap = VK_IMAGE_USAGE_VIDEO_ENCODE_QUANTIZATION_DELTA_MAP_BIT_KHR,
		VideoEncodeEmphasisMap = VK_IMAGE_USAGE_VIDEO_ENCODE_EMPHASIS_MAP_BIT_KHR,
		ShadingRateNV = VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV,
		HostTransferEXT = VK_IMAGE_USAGE_HOST_TRANSFER_BIT_EXT,
		MaxEnum = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM
	};
	#ifndef IMAGE_USAGE_DEF_ENUM_BITWISE_OPERATORS
	#define IMAGE_USAGE_DEF_ENUM_BITWISE_OPERATORS
		DEF_ENUM_BITWISE_OPERATORS(IMAGE_USAGE);
	#endif

	enum class IMAGE_TILING 
	{
		Optimal =					VK_IMAGE_TILING_OPTIMAL,
		Linear =					VK_IMAGE_TILING_LINEAR,
		DrmFormatModifier =			VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
		MaxEnum =					VK_IMAGE_TILING_MAX_ENUM
	};
	#ifndef IMAGE_TILING_DEF_ENUM_BITWISE_OPERATORS
	#define IMAGE_TILING_DEF_ENUM_BITWISE_OPERATORS
		DEF_ENUM_BITWISE_OPERATORS(IMAGE_TILING);
	#endif


	enum class IMAGE_LAYOUT 
	{
		Undefined = VK_IMAGE_LAYOUT_UNDEFINED,
		General = VK_IMAGE_LAYOUT_GENERAL,
		ColorAttachment = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		DepthStencilAttachment = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		DepthStencilReadOnly = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
		ShaderReadOnly = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		TransferSRC = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		TransferDST = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		Preinitialized = VK_IMAGE_LAYOUT_PREINITIALIZED,
		// Provided by VK_VERSION_1_1
		DepthReadOnlyStencilAttachment = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
		DepthAttachmentStencilReadOnly = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
		// Provided by VK_VERSION_1_2
		DepthAttachment = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		DepthReadOnly = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
		StencilAttachment = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
		StencilReadOnly = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
		// Provided by VK_VERSION_1_3
		ReadOnly = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
		Attachment = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
		// Provided by VK_VERSION_1_4
		RenderingLocalRead = VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ,
		// Provided by VK_KHR_swapchain
		PresentSrc = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		// Provided by VK_KHR_video_decode_queue
		VideoDecodeDST = VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR,
		VideoDecodeSRC = VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR,
		VideoDecodeDPB = VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR,
		// Provided by VK_KHR_shared_presentable_image
		SharedPresent =	VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR,
		// Provided by VK_KHR_fragment_shading_rate
		FragmentDensityMap = VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT,
		// Provided by VK_KHR_video_encode_queue
		VideoEncodeDST = VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR,
		VideoEncodeSRC = VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR,
		VideoEncodeDPB = VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR,
		// Provided by VK_EXT_attachment_feedback_loop_layout
		FeedbackLoop = VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT,
		// Provided by VK_KHR_video_encode_quantization_map
		VideoEncodeQuantizationMAP = VK_IMAGE_LAYOUT_VIDEO_ENCODE_QUANTIZATION_MAP_KHR,
		// Provided by VK_EXT_zero_initialize_device_memory
		ZeroInitialized = 1000620000,
		MaxEnum = 0x7FFFFFFF
	};
	#ifndef IMAGE_LAYOUT_DEF_ENUM_BITWISE_OPERATORS
	#define IMAGE_LAYOUT_DEF_ENUM_BITWISE_OPERATORS
		DEF_ENUM_BITWISE_OPERATORS(IMAGE_LAYOUT);
	#endif

	enum BUFFER_SPACE : uint8_t {
		GPU_ONLY = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		CPU_GPU = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	};

	namespace fs = std::filesystem;

	struct ImageFormatRequirements
	{
		tiny_vector<VkFormat> acceptedFormats;
		tiny_vector<VkColorSpaceKHR> acceptedColorSpaces;
		VkImageTiling tiling;
		VkFormatFeatureFlags features;
	};

	struct QueueFamilyIndices
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
	void __VAL_DEBUG_ValidateBufferCopy(uint64_t dstBufferSize, uint64_t dataSize, VkDeviceSize srcOffset, VkDeviceSize dstOffset);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void destroyDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);

	//////////////////////////////////////////////////////////////////////////////////////////////////


	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	std::vector<const char*> getRequiredExtensions(const bool& enableValidationLayers);

	bool checkValidationLayerSupport(std::vector<const char*>& validationLayers);

	bool checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);

	swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

	//bool isDeviceSuitable(VkPhysicalDevice device, std::vector<const char*>& deviceExtensions, VkSurfaceKHR surface);

	//VkSurfaceFormatKHR findSurfaceImageFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VAL_RETURN_CODE allocateCommandBuffers(ValProc& proc, VkCommandBuffer* cmdBuffers, uint32_t cmdBufferCount, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VAL_RETURN_CODE freeCommandBuffers(ValProc& proc, VkCommandBuffer* cmdBuffers, uint32_t cmdBufferCount);

	VAL_RETURN_CODE createSemaphores(ValProc& proc, VkSemaphore* semaphores, uint32_t semaphoreCount);
	
	VAL_RETURN_CODE resetCommandBuffers(VkCommandBuffer* cmdBuffers, uint32_t cmdBufferCount);

	VAL_RETURN_CODE resetCommandBuffer(VkCommandBuffer cmdBuffer);

	VAL_RETURN_CODE beginCommandBuffer(VkCommandBuffer cmdBuffer, VkCommandBufferUsageFlags usageFlags = 0x0);

	VAL_RETURN_CODE beginCommandBuffers(VkCommandBuffer* cmdBuffers, uint32_t cmdBufferCount, VkCommandBufferUsageFlags usageFlags = 0x0);

	VAL_RETURN_CODE endCommandBuffer(VkCommandBuffer cmdBuffer);

	VAL_RETURN_CODE endCommandBuffers(VkCommandBuffer* cmdBuffers, uint32_t cmdBufferCount);


	VAL_RETURN_CODE destroySemaphores(ValProc& proc, VkSemaphore* semaphores, uint32_t semaphoreCount);

	VkFormat findSupportedImageFormat(ValProc& proc, ImageFormatRequirements& requirements);

	VkFormat findSupportedImageFormat(VkPhysicalDevice physicalDevice, ImageFormatRequirements& requirements);

	VkPhysicalDevice findOptimalPhysicalDevice(ValProc& proc, const PhysicalDeviceRequirements& requirements, VkSurfaceKHR surface);

	VkPhysicalDevice findOptimalPhysicalDevice(VkInstance vkInstance, const PhysicalDeviceRequirements& requirements, VkSurfaceKHR surface, uint32_t* deviceCountOut=NULL);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(GLFWwindow* windowHDL, const VkSurfaceCapabilitiesKHR& capabilities);

	VkImageView createImageView(VkDevice device, VkImage image, const VkFormat& format, const uint32_t& mipLevels = 1U);


	VkImage createTextureImage16BitFromDisk(ValProc* proc, fs::path imgFilepath, VkFormat* autoFormatOut, stbi_us** pixelsOut,
		VkDeviceMemory* textureImageMemory, const VkImageLayout layout, const VkFormat format, const VkImageUsageFlagBits additionalUsageFlagBits, const uint32_t mipLevels,
		int* texWidthOut = NULL, int* texHeightOut = NULL, uint8_t* texChannelsOut = NULL,
		const BUFFER_SPACE& buffSpace = GPU_ONLY);


	VkImage createTextureImage8BitFromMemory(ValProc* proc, const void* memory, const uint32_t memSize, stbi_uc** pixelsOut,
		VkDeviceMemory* textureImageMemory, const VkImageLayout layout, const VkFormat format, const VkImageUsageFlagBits additionalUsageFlagBits, const uint32_t mipLevels,
		int* texWidthOut = NULL, int* texHeightOut = NULL, uint8_t* texChannelsOut = NULL,
		const BUFFER_SPACE& buffSpace = GPU_ONLY);

	VkImage createTextureImage8BitFromDisk(ValProc* proc, fs::path imgFilepath, stbi_uc** pixelsOut,
		VkDeviceMemory* textureImageMemory, const VkImageLayout layout, const VkFormat format, const VkImageUsageFlagBits additionalUsageFlagBits, const uint32_t mipLevels,
		int* texWidthOut = NULL, int* texHeightOut = NULL, uint8_t* texChannelsOut = NULL,
		const BUFFER_SPACE& buffSpace = GPU_ONLY);
	


	// returns false if the file cannot be read
	bool readByteFile(const std::string& filename, std::vector<char>* dst);

	bool readByteFile(const std::string& filename, tiny_vector<char>* dst);

	uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkMemoryPropertyFlags bufferSpaceToVkMemoryProperty(const BUFFER_SPACE& bufferSpace);

	void transitionImageLayout(VkDevice Device, VkImage img, const IMAGE_LAYOUT oldLayout,
		const IMAGE_LAYOUT newLayout, const IMAGE_ASPECT aspectMask, const PIPELINE_STAGE srcStageMask,
		const PIPELINE_STAGE dstStageMask, const ACCESS_FLAGS srcAccessMask, const ACCESS_FLAGS dstAccessMask,
		const DEPENDENCY_FLAGS dependencyFlags, const VkCommandBuffer& commandBuffer);

}

#endif // !VAl_SYSTEM_UTILS