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
#include <VAL/lib/system/image.hpp>
#include <VAL/lib/graphics/shader.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vertex.hpp"

// it is important that this is the last include
#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>

struct uniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};



void updateUniformBuffer(int shaderIndex, val::VAL_PROC& proc) {
	using namespace val;
	VkExtent2D& extent = proc._windowVAL->_swapChainExtent;
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	static uniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	memcpy(proc._uniformBuffersMapped[proc._currentFrame][shaderIndex], &ubo, sizeof(ubo));
}

void setGraphicsPipelineInfo(val::pipelineCreateInfo& info, const VkSampleCountFlagBits& msaaSamples) {
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

	// for sampleShadingEnable to be valid, set: deviceFeatures.sampleRateShading = VK_TRUE; // enable sample shading feature for the device
	//multisampling.sampleShadingEnable = VK_TRUE;
	//multisampling.minSampleShading = .75f; // min fraction for sample shading; closer to one is smoother

	multisampling.rasterizationSamples = msaaSamples;

	VkPipelineColorBlendAttachmentState& colorBlendAttachment = info.colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

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
}

void setImageSamplerInfo(VkSamplerCreateInfo* info, const uint32_t mipmapLevels) {
	info->sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	info->magFilter = VK_FILTER_LINEAR;
	info->minFilter = VK_FILTER_LINEAR;
	info->addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info->addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info->addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	info->anisotropyEnable = VK_TRUE;

	info->maxAnisotropy = 8; // this value will be clamped if it is greater than what is supported by the graphics card
	info->borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	info->unnormalizedCoordinates = VK_FALSE;
	info->compareEnable = VK_FALSE;
	info->compareOp = VK_COMPARE_OP_ALWAYS;
	info->mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	info->mipLodBias = 0.0f;
	info->minLod = 0.0f;
	info->maxLod = VK_LOD_CLAMP_NONE;
	info->anisotropyEnable = VK_FALSE;
	info->maxAnisotropy = 1.0f;
}

void setRenderPassInfo(val::renderPassInfo& renderPassInfo, VkFormat imageFormat, VkSampleCountFlagBits msaaSamples) {
	// !ALL VARIABLES in this function MUST BE STATIC! //

	// attachments
	static VkAttachmentDescription colorAttachment{};
	colorAttachment.format = imageFormat;
	colorAttachment.samples = msaaSamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	static VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = imageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


	static VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// THE FINAL COLOR ATTACHMENT REFERENCE MUST ALWAYS BE THE LAST ATTACHMENT
	static VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 1;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	renderPassInfo.attachments = {colorAttachment,colorAttachmentResolve };


	// subpasses
	static VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;


	renderPassInfo.subpasses = { subpass };

	static VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	renderPassInfo.subpassDependencies = { dependency };
}

int main() {
	val::VAL_PROC mainProc;

	/////////// consider moving this into the window class ///////////
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable
	//////////////////////////////////////////////////////////////////

	VkExtent2D windowSize{ 800,800 }; // in pixels
	GLFWwindow* windowHDL_GLFW = glfwCreateWindow(windowSize.width, windowSize.height, "Test", NULL, NULL);

	val::window window(windowHDL_GLFW, &mainProc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	mainProc.initDevices(deviceExtensions, validationLayers, enableValidationLayers, &window);

	// VAL uses image format requirements to pick the best image format out of a list of the formats input
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	VkFormat imageFormat = val::findSupportedImageFormat(mainProc._physicalDevice, formatReqs);


	// MIPMAP LEVEL
	// I have to clamp this to 8, because the image format I'm using has a maximum value of 15.
	const uint32_t mipLevels = std::clamp(uint32_t(mainProc.getMaxMipmapLevel()), 1U, 8U);


	VkSampleCountFlagBits msaaSamples = mainProc.getMaxSampleCount();

	printf("\n>-- MSAA Sample count: %d\n\n", msaaSamples);


	// THIS PROCESS MUST BE SIMPLIFIED
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	mainProc.createImage(windowSize.width, windowSize.height, imageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory, 1u, msaaSamples);
	// this function should be changed to image colorImageView in the front
	mainProc.createImageView(colorImage, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, &colorImageView, 1u);


	uniformBufferObject ubo;

	// load and configure vert shader
	val::shader vertShader("shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions().data(),
		res::vertex::getAttributeDescriptions().size());
	vertShader.setBindingDescription(res::vertex::getBindingDescription());
	vertShader.setUniformBufferData(&ubo, sizeof(ubo), 1);


	// load and configure frag shader
	val::shader fragShader("shaders/imagefrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	VkSamplerCreateInfo samplerInfo{};
	setImageSamplerInfo(&samplerInfo, mipLevels);
	fragShader.setImageSamplers({ samplerInfo });

	VkImageView imgView;
	fragShader.setImageView({ &imgView });

	// config grahics pipeline
	val::pipelineCreateInfo pipelineInfo;
	pipelineInfo.shaders.push_back(&vertShader); // consolidate into a single function
	pipelineInfo.shaders.push_back(&fragShader); // consolidate into a single function

	setGraphicsPipelineInfo(pipelineInfo, msaaSamples);

	val::renderPassInfo renderPassInfo;
	setRenderPassInfo(renderPassInfo, imageFormat, msaaSamples);
	pipelineInfo.renderPassInfo = &renderPassInfo;
	// 1 renderPass per pipeline
	std::vector<VkRenderPass> renderPasses;

	mainProc.create(windowHDL_GLFW, &window, 2u, imageFormat, { &pipelineInfo }, &renderPasses);

	std::vector<VkImageView> attachemnts = { colorImageView };
	window.createSwapChainFrameBuffers(window._swapChainExtent, attachemnts.data(), attachemnts.size(), renderPasses[0], mainProc._device);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// AFTER VAL_PROC INIT //////////////////////////////////////////////////
	//////////////////////////////////////////////////


	// CREATE IMAGE & IMAGE VIEW //
	val::image img(mainProc, "testImage.jpg", imageFormat, mipLevels);
	mainProc.createImageView(img.getImage(), imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, &imgView, mipLevels);

	const std::vector<res::vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	VkBuffer vertexBuffer = NULL;
	VkDeviceMemory vertexBufferMem = NULL;
	mainProc.createVertexBuffer(vertices.data(), vertices.size(), sizeof(res::vertex), &vertexBuffer, &vertexBufferMem);

	std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0 };
	VkBuffer indexBuffer = NULL;
	VkDeviceMemory indexBufferMem = NULL;

	mainProc.createIndexBuffer(indices.data(), indices.size(), &indexBuffer, &indexBufferMem);



	// sets the image view for the frag shader.
	fragShader._imageViews[0] = &imgView;

	int timer = 0;
	bool imgNum = 0;

	VkClearValue clearValues[1];
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };



	// this needs to be called only after the frag shader's image view has been set
	mainProc.createDescriptorSets(&pipelineInfo, 0u);

	while (!glfwWindowShouldClose(windowHDL_GLFW)) {
		glfwPollEvents();
		// INSTEAD OF UPDATING HERE, ADD A METHOD TO UPDATE UBOS VIA THE SHADER
		// ALSO, THERE IS EXCESS COPYING IN THIS FUNCTION
		updateUniformBuffer(0/*the vert shader is the first to be created, so it's index is 0*/, mainProc); // maybe updateDescriptorSet(s)?

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);
		mainProc.beginDraw(imageFormat);

		mainProc.drawFrameExperimental(0u, renderPasses[0], framebuffer, vertexBuffer, indexBuffer, indices.data(), indices.size(), imageFormat, clearValues, uint16_t(sizeof(clearValues) / sizeof(VkClearValue)));

		// Wait idle for the GPU to finish executing the command buffer
		vkDeviceWaitIdle(mainProc._device);

		mainProc.endDraw(imageFormat);

		window.waitForFences();
	}

	vkDestroyImageView(mainProc._device, imgView, NULL);

	img.~image();

	mainProc.cleanup(windowHDL_GLFW);

	return EXIT_SUCCESS;
}