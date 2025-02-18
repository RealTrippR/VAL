#ifndef VAL_VAL_PROC_HPP
#define VAL_VAL_PROC_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <ExternalLibraries/stb_image.h>;
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/graphics/shader.hpp>

#include <VAL/lib/system/image.hpp>

#include <VAL/lib/system/graphicsPipelineCreateInfo.hpp>
#include <VAL/lib/system/computePipelineCreateInfo.hpp>

#include <VAL/lib/system/queueManager.hpp>

#include <cstdint>
#include <vector>
#include <array>
#include <stdexcept>
#include <optional>
#include <unordered_map>
#include <unordered_set>


namespace val {
	class texture; // forward declaration
	class shader; // forward declaration
	class window; // forward declaration

	class VAL_PROC {
	public:
		////////////////////////////////////////////////
		VAL_PROC() = default;
		VAL_PROC(const VAL_PROC& other) = delete;
		////////////////////////////////////////////////
	public:

		void beginDraw(const VkFormat& imageFormat);

		void endDraw(const VkFormat& imageFormat);

		void initDevices(std::vector<const char*>& deviceExtensions, const std::vector<const char*>& validationLayers, const bool& enableValidationLayers, val::window* windowVAL);

		void create(GLFWwindow* windowHDL_GLFW, val::window* windowVAL, const uint8_t maxFramesInFlight, const VkFormat swapchainFormat,
			std::vector<graphicsPipelineCreateInfo*> pipelineCreateInfos, std::vector<VkRenderPass>* renderPassesOut, std::vector<computePipelineCreateInfo*> computePipelineCreateInfos = {});

		void cleanup(GLFWwindow* window /*optional*/);

		void drawFrameExperimental(const uint16_t& pipelineIdx, VkRenderPass renderPass, VkFramebuffer framebuffer, const VkBuffer& vertexBuffer, const VkBuffer& indexBuffer, const uint32_t* indices,
			const uint32_t& indicesCount, const VkFormat& imageFormat, const VkClearValue* clearValues, const uint16_t& clearValueCount);

		VkDevice getVkLogicalDevice();

		VkPhysicalDevice getVkPhysicalDevice();

		const uint32_t& getCurrentFrame();

	public:

		void setupDebugMessenger(const bool& enableValidationLayers);

		void createVK_Instance(std::vector<const char*> validationLayers, const bool& enableValidationLayers);

		void pickPhysicalDevice(std::vector<const char*>& deviceExtensions);

		void createLogicalDevice(std::vector<const char*>& deviceExtensions,
			const std::vector<const char*>& validationLayers, const bool& enableValidationLayers, val::window* windowVAL);
		
		void createSyncObjects();

		VkRenderPass createRenderPass(const val::renderPassInfo* renderPassCreateInfo);

		void createDescriptorSetLayout(pipelineCreateInfo* createInfo, const uint32_t pipelineIdx);

		void createDescriptorPool(pipelineCreateInfo* pipelineInfo, const uint32_t pipelineIdx);

		void createDescriptorSets(pipelineCreateInfo* createInfo, const uint32_t pipelineIdx);

		void createGraphicsPipelines(std::vector<graphicsPipelineCreateInfo*>& createInfos, std::vector<VkRenderPass>* renderPasses);

		void createComputePipelines(std::vector<computePipelineCreateInfo*>& createInfos);

		void createCommandPool();

		void createCommandBuffers();

		void recordRenderPassCommands(const uint16_t& pipelineIdx, VkRenderPass renderPass, const VkBuffer& vertexBuffer, const VkBuffer& indexBuffer, const uint32_t& indicesCount,
			VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, const VkClearValue* clearValues, const uint16_t& clearValueCount);

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

		void createUniformBuffers(pipelineCreateInfo* createInfo);

		void createSSBOs(pipelineCreateInfo* createInfo);

		//void resizeSSBO(SSBO_Handle* SSBO_HDL);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void createImage(const uint32_t& width, const uint32_t& height, const VkFormat& format, const VkImageTiling& tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkImage& image,
			VkDeviceMemory& imageMemory, const uint8_t& mipMapLevels = 1u, const VkSampleCountFlagBits& NumSamples = VK_SAMPLE_COUNT_1_BIT);

		void transitionImageLayout(VkImage image, const VkFormat& format, const VkImageLayout& oldLayout, const VkImageLayout& newLayout, VkCommandBuffer CommandBuffer = VK_NULL_HANDLE,
			const uint32_t& mipLevels = 1U);

		void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView, const uint32_t& mipMapLevels = 1U);

		void copyBufferToImage(VkBuffer buffer, VkImage image, const uint32_t& width, const uint32_t& height);

		void generateMipMaps(VkImage image, const VkFormat imageFormat, const int32_t texWidth, const int32_t texHeight, const uint32_t mipMapLevels);

		// copies the srcbuffer into the dstBuffer
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkDeviceSize srcOffset = 0, const VkDeviceSize dstOffset = 0);

		VkCommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void createFrameBuffer(VkDevice physicalDevice, const VkExtent2D& extent, VkRenderPass renderPass);

		VkShaderModule createShaderModule(const char* bytecode, const size_t bytecodeSize);

		void copyImage(VkImage& src, VkImage& dst, const VkFormat& imageFormat, const VkImageLayout& srcLayout, const VkImageLayout dstLayout, const uint32_t& width, const uint32_t& height);

		uint32_t getMaxMipmapLevel();

		VkSampleCountFlagBits getMaxSampleCount();

	public:
		void createVertexBuffer(const void* vertices, const size_t vertexCount, const size_t sizeOfVertex, VkBuffer* vertexBufferOut, VkDeviceMemory* vertexBufferMemoryOut, VkBufferUsageFlags additionalUsages = VkBufferUsageFlags(0));

		void createIndexBuffer(uint32_t* indices, const uint32_t& indexCount, VkBuffer* indexBufferOut, VkDeviceMemory* indexBufferMemoryOut);

	public:

		friend texture;
		friend window;
		friend shader;

		bool _frameBufferResized = false;

		val::window* _windowVAL = NULL;

		VkInstance _instance = VK_NULL_HANDLE;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE; // logical device
		VkDebugUtilsMessengerEXT _debugMessenger;

		//VkSurfaceKHR _surface = VK_NULL_HANDLE;

		VkCommandPool _commandPool;


		queueManager _graphicsQueue;
		queueManager _computeQueue;
		queueManager _transferQueue;

		//VkQueue _graphicsQueue;
		//std::vector<VkCommandBuffer> _commandBuffers;

		std::vector<VkPipeline> _graphicsPipelines;
		std::vector<VkPipeline> _computePipelines;

		std::vector<VkPipelineLayout> _pipelineLayouts;
		std::vector<VkPipelineLayout> _computePipelineLayouts;

		// includes both graphics and compute shaders: [1st order: Graphics Shaders, 2nd order: Compute shaders]
		std::vector<VkDescriptorSetLayout> _descriptorSetLayouts; // one for every pipeline
		// includes both graphics and compute shaders: [1st order: Graphics Shaders, 2nd order: Compute shaders]
		std::vector<VkDescriptorPool> _descriptorPools; // one for every pipeline
		// includes both graphics and compute shaders: [1st order: Graphics Shaders, 2nd order: Compute shaders]
		std::vector</*1 per pipeline*/std::vector<VkDescriptorSet>> _descriptorSets;



		uint32_t _currentFrame = 0;

		// PLACEHOLDERS: For optimization purposes, the memory should be packed into only one buffer
		// Consider creating a bufferManager class for this purpose
		std::vector<std::vector<VkBuffer>> _uniformBuffers; // the outer vector is for each frame, and the inner vector is for each shader
		std::vector<std::vector<VkDeviceMemory>> _uniformBuffersMemory;
		std::vector<std::vector<void*>> _uniformBuffersMapped;

		void* _pushConstantData;

		std::vector<std::vector<VkBuffer>> _SSBO_Buffers;
		std::vector<std::vector<VkDeviceMemory>> _SSBO_Memory;
		std::vector<std::vector<void*>> _SSBO_DataMapped;


		unsigned char _MAX_FRAMES_IN_FLIGHT;
	};
}

#endif // !VAL_VAL_PROC_HPP