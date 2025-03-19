// https://snorristurluson.github.io/TextRenderingWithFreetype/ - FreeType con Vulkan

#include <iostream>
#include <string>
#include <chrono>

#ifdef NDEBUG
const bool enableValidationLayers = false;

#else
const bool enableValidationLayers = true;
#endif //!NDEBUG

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/UBO_Handle.hpp>
#include <VAL/lib/graphics/shader.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vertex.hpp"


// it is important that this comes last
#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>

#include <random>

//////////////////////////////////////////////
// vars for the particle sim //
const uint32_t PARTICLE_COUNT = 8192;
float lastFrameTime = 0.0f;
double lastTime = 0.0f;
//////////////////////////////////////////////

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

struct UniformBufferObject {
	float deltaTime = 1.0f;
};

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Particle);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() {
		static std::vector<VkVertexInputAttributeDescription> attributeDescriptions{ {},{} };

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Particle, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Particle, color);

        return attributeDescriptions;
    }
};

void updateUniformBuffer(val::VAL_PROC& proc, val::UBO_Handle& UBO_HDL) {
	static UniformBufferObject ubo{};
	ubo.deltaTime = lastFrameTime * 2.0f;
	UBO_HDL.update(proc, &ubo);
}

void setGraphicsPipelineInfo(val::graphicsPipelineCreateInfo& info) {
	VkPipelineRasterizationStateCreateInfo& rasterizer = info.rasterizer;
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo& multisampling = info.multisampling;
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState& colorBlendAttachment = info.colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	VkPipelineColorBlendStateCreateInfo& colorBlending = info.colorBlending;
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;


	info.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
}

void setRenderPass(val::renderPassManager& renderPassMngr, VkFormat imgFormat) {
	using namespace val;
	static colorAttachment colorAttach;
	colorAttach.setImgFormat(imgFormat);
	colorAttach.setLoadOperation(CLEAR);
	colorAttach.setStoreOperation(STORE);
	colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	static subpass subpass(renderPassMngr, GRAPHICS);
	subpass.bindAttachment(&colorAttach);
}

void initParticles(std::vector<Particle>& particles, VkExtent2D displayExtent) {
	// Initialize particles
	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

	// Initial particle positions on a circle
	particles.resize(PARTICLE_COUNT);

	for (auto& particle : particles) {
		float r = 0.25f * sqrt(rndDist(rndEngine));
		float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
		float x = r * cos(theta) * displayExtent.width / displayExtent.height;
		float y = r * sin(theta);
		particle.position = glm::vec2(x, y);
		particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
		particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
	}
}

int main() {
	val::VAL_PROC proc;

	/////////// consider moving this into the window class ///////////
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable
	//////////////////////////////////////////////////////////////////
	
	VkExtent2D windowSize = { 800, 600 };
	GLFWwindow* windowHDL_GLFW = glfwCreateWindow(windowSize.width, windowSize.height, "Test", NULL, NULL);

	// The creation of the swapchain is handled in the window
	val::window window(windowHDL_GLFW, &proc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// VAL uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	std::vector<Particle> particles;
	initParticles(particles, { windowSize.width,windowSize.height });

	val::SSBO_Handle ssboHdl(particles.size()*sizeof(Particle), GPU_ONLY);
	ssboHdl._additionalUsageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	val::SSBO_Handle debugHdl(sizeof(glm::vec2), CPU_GPU);
	debugHdl._additionalUsageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	val::UBO_Handle uboHDL(sizeof(UniformBufferObject));

	// load and configure shaders
	val::shader vertShader("shaders-compiled/particleShadervert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setBindingDescriptions({ Particle::getBindingDescription() });
	vertShader.setVertexAttributes(Particle::getAttributeDescriptions());

	val::shader fragShader("shaders-compiled/particleShaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	val::shader computeShader("shaders-compiled/particleShadercomp.spv", VK_SHADER_STAGE_COMPUTE_BIT, "main");
	computeShader._UBO_Handles = { {&uboHDL,0} };
	computeShader._SSBO_Handles = { {&ssboHdl,1}, {&ssboHdl,2}, {&debugHdl,3} };


	//////////////////////////////////////////////////////////////

	val::graphicsPipelineCreateInfo pipeline;
	pipeline.shaders = { &vertShader, &fragShader };

	val::computePipelineCreateInfo computePipeline;
	computePipeline.shaders = { &computeShader };

	setGraphicsPipelineInfo(pipeline);

	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	proc.initDevices(deviceExtensions, validationLayers, enableValidationLayers, &window);

	VkFormat imageFormat = val::findSupportedImageFormat(proc._physicalDevice, formatReqs);

	val::renderPassManager renderPassMNGR(proc);
	setRenderPass(renderPassMNGR, imageFormat);
	pipeline.renderPass = &renderPassMNGR;
	// 1 renderPass per pipeline

	proc.create(windowHDL_GLFW, &window, 2u, imageFormat, { &pipeline }, { &computePipeline });

	window.createSwapChainFrameBuffers(window._swapChainExtent, {}, 0u, pipeline.getVkRenderPass(), proc._device);
	//////////////////////////////////////////////////////////////
	// create descriptor sets - this should be merged into the
	// pipeline creation function
	proc.createDescriptorSets(&pipeline);
	proc.createDescriptorSets(&computePipeline);

	//////////////////////////////////////////////////////////////
	
	// overwrite some of the previous descriptors
	// it would be nice if there was a simpler way to do this...
	for (size_t i = 0; i < proc._MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo storageBufferInfoLastFrame{};
		storageBufferInfoLastFrame.buffer = ssboHdl.getBuffers(proc)[(i - 1) % proc._MAX_FRAMES_IN_FLIGHT];
		storageBufferInfoLastFrame.offset = 0;
		storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = computePipeline.getDescriptorSets(proc)[i];
		descriptorWrites[0].dstBinding = 1;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &storageBufferInfoLastFrame;

		VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
		storageBufferInfoCurrentFrame.buffer = ssboHdl.getBuffers(proc)[i];
		storageBufferInfoCurrentFrame.offset = 0;
		storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = computePipeline.getDescriptorSets(proc)[i];
		descriptorWrites[1].dstBinding = 2;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &storageBufferInfoCurrentFrame;

		vkUpdateDescriptorSets(proc._device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }

	ssboHdl.updateFromTempStagingBuffer(proc, particles.data());

	VkClearValue clearValues[1];
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };

	uint32_t currentSC_ImageIdx = 0;

	// create & configure the render target, setting vertex buffers, scissors, area, etc
	val::renderTarget renderTarget;
	renderTarget.setFormat(imageFormat);
	renderTarget.setArea(window._swapChainExtent);
	renderTarget.setScissorExtent(window._swapChainExtent);
	renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });

	// config viewport, covers the entire size of the window
	VkViewport viewport{ 0,0, window._swapChainExtent.width, window._swapChainExtent.height, 0.f, 1.f };

	// create & configure the compute target
	val::computeTarget computeTarget;

	while (!glfwWindowShouldClose(windowHDL_GLFW)) {
		glfwPollEvents();
		lastTime = glfwGetTime();

		val::queueManager& computeQueue = proc._computeQueue;
		auto& graphicsQueue = proc._graphicsQueue;
		auto& presentQueue = window._presentQueue;
		auto& currentFrame = proc._currentFrame;
		

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		computeTarget.begin(proc);
		computeTarget.update(proc, computePipeline);
		computeTarget.compute(proc, PARTICLE_COUNT / 256, 1, 1);
		computeTarget.submit(proc, {}, computeQueue._fences[currentFrame]);

		//////////////////////////////////////////////////////////////
		updateUniformBuffer(proc, uboHDL);


		// note: the getBuffers function is very ineffecient, it should be omptimized.
		renderTarget.setVertexBuffers({ ssboHdl.getBuffers(proc)[proc._currentFrame] }, PARTICLE_COUNT);
		// the renderTarget must be updated after any changes are made 
		VkFramebuffer framebuffer = window.beginDraw(imageFormat);

		renderTarget.beginPass(proc, pipeline.getVkRenderPass(), framebuffer);
		renderTarget.update(proc, pipeline, { viewport });
		renderTarget.render(proc);
		renderTarget.endPass(proc);

		renderTarget.submit(proc, { presentQueue._semaphores[currentFrame], computeQueue._semaphores[currentFrame]}, presentQueue._fences[currentFrame]);
		window.display(imageFormat, { graphicsQueue._semaphores[currentFrame] });
		proc.nextFrame();

		double currentTime = glfwGetTime();
		lastFrameTime = (currentTime - lastTime) * 1000.0;

		//glm::vec2* debugData = (glm::vec2*)(proc._SSBO_DataMapped[currentFrame][debugHdl._index]);
		//printf("DEBUG BUFFER: x: %f y: %f\n", debugData->x, debugData->y);
	}

	window.cleanupSwapChain();
	proc.cleanup();

	return EXIT_SUCCESS;
}