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

#ifndef VAL_VAL_PROC_HPP
#define VAL_VAL_PROC_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <ExternalLibraries/stb_image.h>;
#include <VAL/lib/system/system_utils.hpp>

#include <VAL/lib/system/window.hpp>
#include <VAL/lib/graphics/shader.hpp>

#include <VAL/lib/system/image.hpp>
#include <VAL/lib/system/imageView.hpp>
#include <VAL/lib/system/depthBuffer.hpp>

#include <VAL/lib/system/graphicsPipelineCreateInfo.hpp>
#include <VAL/lib/system/computePipelineCreateInfo.hpp>

#include <VAL/lib/system/queueManager.hpp>

#include <VAL/lib/system/renderTarget.hpp>
#include <VAL/lib/system/computeTarget.hpp>

#include <VAL/lib/system/buffer.hpp>

#include <VAL/lib/system/renderPass.hpp>

#include <VAL/lib/system/multisamplerManager.hpp>

#include <VAL/lib/system/UBO_arr_manager.hpp>

#include <VAL/lib/pipelineStateInfos/stateInfos.hpp>


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
		~VAL_PROC() {
			cleanup();
		}
	public:
		/// initDevices() - Creates the Vulkan Instance along with the Physical and Logical devices. If a window is passed in as an argument, it's surface will be created.
		/// @name initDevices
		/// @brief Creates vulkan instances, devices, and window surface
		/// @param val::physical_Device_Requirements physical_Device_Requirements - The Physical Device Requirements
		/// @param const std::vector<const char*>& validationLayers - Requested validation layers
		/// @param const bool enableValidationLayers - Toggle for validation layers
		/// @param val::window* windowVAL - An optional handle to to the VAL window
		void initDevices(val::physicalDeviceRequirements& physical_Device_Requirements, const std::vector<const char*>& validationLayers, const bool enableValidationLayers, val::window* windowVAL);

		void create(val::window* windowVAL, const uint8_t maxFramesInFlight, const VkFormat swapchainFormat,
			std::vector<graphicsPipelineCreateInfo*> pipelineCreateInfos, std::vector<computePipelineCreateInfo*> computePipelineCreateInfos = {});

		void cleanup();

		inline void nextFrame() {
			_currentFrame = (_currentFrame + 1) % _MAX_FRAMES_IN_FLIGHT;
		}
		//void drawFrameExperimental(const uint16_t& pipelineIdx, VkRenderPass renderPass, VkFramebuffer framebuffer, const VkBuffer& vertexBuffer, const VkBuffer& indexBuffer, const uint32_t* indices,
		//	const uint_fast32_t& vertexCount, const uint_fast32_t& indicesCount, const VkFormat& imageFormat, const VkClearValue* clearValues, const uint16_t& clearValueCount);

		VkDevice getVkLogicalDevice();

		VkPhysicalDevice getVkPhysicalDevice();

		const uint32_t& getCurrentFrame();

	public:

		void setupDebugMessenger(const bool& enableValidationLayers);

		void createVK_Instance(std::vector<const char*> validationLayers, const bool& enableValidationLayers);

		void pickPhysicalDevice(val::physicalDeviceRequirements& requirements);

		void createLogicalDevice(std::vector<const char*>& deviceExtensions, const std::vector<const char*>& validationLayers,
			const bool& enableValidationLayers, VkPhysicalDeviceFeatures* deviceFeatures, val::window* windowVAL);
		
		void createSyncObjects();

		void createRenderPass(val::renderPassManager* renderPassManager);

		void createDescriptorSetLayout(pipelineCreateInfo* createInfo, const uint32_t pipelineIdx);

		void createPushDescriptorSetLayouts(pipelineCreateInfo* createInfo);

		void createDescriptorPool(pipelineCreateInfo* pipelineInfo, const uint32_t pipelineIdx);

		void createDescriptorSets(pipelineCreateInfo* createInfo);

		void createGraphicsPipelines(std::vector<graphicsPipelineCreateInfo*>& createInfos);

		void createComputePipelines(std::vector<computePipelineCreateInfo*>& createInfos);

		void createCommandPool();

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

		void createUniformBuffers(std::vector<pipelineCreateInfo*>);

		void createSSBOs(std::vector<pipelineCreateInfo*>);

		//void resizeSSBO(SSBO_Handle* SSBO_HDL);

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		void createImage(const uint32_t& width, const uint32_t& height, const VkFormat& format, const VkImageTiling& tiling, const VkImageUsageFlags& usage, const VkMemoryPropertyFlags& properties, VkImage& image,
			VkDeviceMemory& imageMemory, const uint8_t& mipMapLevels = 1u, const VkSampleCountFlagBits& NumSamples = VK_SAMPLE_COUNT_1_BIT, const VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED);

		void transitionImageLayout(VkImage image, const VkFormat& format, const VkImageLayout& oldLayout, const VkImageLayout& newLayout, VkCommandBuffer CommandBuffer = VK_NULL_HANDLE,
			const uint32_t& mipLevels = 1U);

		void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* imageView, const uint32_t& mipMapLevels = 1U);

		void copyBufferToImage(VkBuffer buffer, VkImage image, const uint32_t& width, const uint32_t& height);

		void generateMipMaps(VkImage image, const VkFormat imageFormat, const int32_t texWidth, const int32_t texHeight, const uint32_t mipMapLevels);

		// copies the srcbuffer into the dstBuffer
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, const VkDeviceSize srcOffset = 0, const VkDeviceSize dstOffset = 0);

		VkCommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		VkShaderModule createShaderModule(const char* bytecode, const size_t bytecodeSize);

		void copyImage(VkImage& src, VkImage& dst, const VkFormat& imageFormat, const VkImageLayout& srcLayout, const VkImageLayout dstLayout, const uint32_t& width, const uint32_t& height);

		uint32_t getMaxMipmapLevel();

		VkSampleCountFlagBits getMaxSampleCount();

	public:
		void createVertexBuffer(const void* vertices, const size_t vertexCount, const size_t sizeOfVertex, VkBuffer* vertexBufferOut, VkDeviceMemory* vertexBufferMemoryOut, VkBufferUsageFlags additionalUsages = VkBufferUsageFlags(0));

		void createIndexBuffer(uint32_t* indices, const uint32_t& indexCount, VkBuffer* indexBufferOut, VkDeviceMemory* indexBufferMemoryOut);

	private:

		void cleanupTmpDeviceExtensions();

	public:

		friend texture;
		friend window;
		friend shader;

		bool _frameBufferResized = false;

		val::window* _windowVAL = NULL;

		VkInstance _instance = VK_NULL_HANDLE;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties _physicalDeviceProperties{};
		VkDevice _device = VK_NULL_HANDLE; // logical device

#ifndef NDEBUG
		VkDebugUtilsMessengerEXT _debugMessenger{};
#endif


		VkCommandPool _commandPool{};


		queueManager _graphicsQueue;
		queueManager _computeQueue;
		queueManager _transferQueue;


		std::vector<VkPipeline> _graphicsPipelines;
		std::vector<VkPipeline> _computePipelines;

		std::vector<VkPipelineLayout> _pipelineLayouts;
		std::vector<VkPipelineLayout> _computePipelineLayouts;

		// includes both graphics and compute shaders: [1st order: Graphics Shaders, 2nd order: Compute shaders]
		std::vector<VkDescriptorSetLayout> _descriptorSetLayouts; // one for every pipeline

		// includes both graphics and compute shaders: [1st order: Graphics Shaders, 2nd order: Compute shaders]
		std::vector<VkDescriptorPool> _descriptorPools; // one for every pipeline

		// note that push descriptor set layouts don't require descriptor pools [1st order: Graphics Shaders, 2nd order: Compute shaders]
		std::vector<VkDescriptorSetLayout> _pushDescriptorSetLayouts; // one for every pipeline that has push descriptors

		// includes both graphics and compute shaders: [1st order: Graphics Shaders, 2nd order: Compute shaders]
		std::vector<std::vector<VkDescriptorSet>> _descriptorSets; // the outer vector is for every pipeline respective to it's index, the inner vector is each frame in flight.



		uint32_t _currentFrame = 0;

		// PLACEHOLDERS: For optimization purposes, the memory should be packed into only one buffer
		// Consider creating a bufferManager class for this purpose
		//std::vector<std::vector<VkBuffer>> _uniformBuffers; // the outer vector is for each frame, and the inner vector is for each shader
		//std::vector<std::vector<VkDeviceMemory>> _uniformBuffersMemory;
		//std::vector<std::vector<void*>> _uniformBuffersMapped;
		uboArray _uboArr;

		std::vector<std::vector<VkBuffer>> _SSBO_Buffers;
		std::vector<std::vector<VkDeviceMemory>> _SSBO_Memory;
		std::vector<std::vector<void*>> _SSBO_DataMapped;

		uint8_t _MAX_FRAMES_IN_FLIGHT;
	
	private:
		// this array will be discarded after creation of the VAL_PROC
		char** _deviceExtensions = NULL;
		uint_fast8_t _deviceExtCount = 0u;
	};
}


#include <VAL/lib/system/graphicsPipelineCreateInfo.inl>
#include <VAL/lib/system/window.inl>
#include <VAL/lib/system/texture2d.inl>
#endif // !VAL_VAL_PROC_HPP
