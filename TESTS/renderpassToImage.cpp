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
#include <VAL/lib/graphics/shader.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vertex.hpp"


// it is important that this comes last
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


void updateUniformBuffer2(int shaderIndex, val::VAL_PROC& proc) {
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

void setRenderPassInfo(val::renderPassInfo& renderPassInfo, VkFormat colorAttachmentImageFormat) {

	// attachments
	static VkAttachmentDescription colorAttachment{};
	colorAttachment.format = colorAttachmentImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	renderPassInfo.attachments = { colorAttachment };
	renderPassInfo.attachmentImageLayouts = { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL/*This second value is the layout of the corresponding VkAttachmentReference*/ };

	static VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// subpasses
	static VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	renderPassInfo.subpasses = { subpass };

	static VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassInfo.subpassDependencies = { dependency };
}

void setImageSamplerInfo(VkSamplerCreateInfo* info) {
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
	info->maxLod = 0.0f;
	info->anisotropyEnable = VK_FALSE;
	info->maxAnisotropy = 1.0f;
}

void setupRenderPass2(val::renderPassInfo& passInfo, const VkFormat imageFormat) {

	static VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = imageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	passInfo.attachmentImageLayouts = { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL/*This second value is the layout of the corresponding VkAttachmentReference*/ };


	static VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = colorAttachment.finalLayout;

	static VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	static VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	passInfo.attachments = { colorAttachment };
	passInfo.subpasses = { subpass };
	passInfo.subpassDependencies = { dependency };
}

int main() {
	val::VAL_PROC mainProc;

	/////////// consider moving this into the window class ///////////
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable
	//////////////////////////////////////////////////////////////////

	GLFWwindow* windowHDL_GLFW = glfwCreateWindow(800, 800, "Test", NULL, NULL);

	// The creation of the swapchain is handled in the window
	val::window window(windowHDL_GLFW, &mainProc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// FML uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	uniformBufferObject ubo;
	// load and configure vert shader
	val::shader vertShader("shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions().data(),
		res::vertex::getAttributeDescriptions().size());
	vertShader.setBindingDescription(res::vertex::getBindingDescription());
	vertShader.setUniformBufferData(&ubo, sizeof(ubo), 1);

	// load and configure frag shader
	// CONSIDER STORING IMAGE INFO INSIDE THE SHADER CLASS
	val::shader fragShaderImage("shaders/imagefrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	VkSamplerCreateInfo samplerInfo{};
	setImageSamplerInfo(&samplerInfo);
	fragShaderImage.setImageSamplers({ samplerInfo });

	VkImageView renderTargImgView;
	fragShaderImage.setImageView({ &renderTargImgView });
	// fragShader.setDescript
	// configure pipeline create info
	val::graphicsPipelineCreateInfo pipelineInfo1;
	pipelineInfo1.shaders = { &vertShader,&fragShaderImage };

	setGraphicsPipelineInfo(pipelineInfo1);

	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	mainProc.initDevices(deviceExtensions, validationLayers, enableValidationLayers, &window);

	VkFormat imageFormat = val::findSupportedImageFormat(mainProc._physicalDevice, formatReqs);

	val::renderPassInfo mainRenderPassInfo;
	setRenderPassInfo(mainRenderPassInfo, imageFormat);
	pipelineInfo1.renderPassInfo = &mainRenderPassInfo;

	//////////////////////////////////////////////////////////////////////////////

	val::graphicsPipelineCreateInfo pipelineInfo2;

	uniformBufferObject ubo2;
	val::shader vertShader2("shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader2.setVertexAttributes(res::vertex::getAttributeDescriptions().data(),
		res::vertex::getAttributeDescriptions().size());
	vertShader2.setBindingDescription(res::vertex::getBindingDescription());
	vertShader2.setUniformBufferData(&ubo, sizeof(ubo), 1);

	val::shader fragShaderColor("shaders/colorfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	pipelineInfo2.shaders = { &vertShader2,&fragShaderColor };

	setGraphicsPipelineInfo(pipelineInfo2);

	pipelineInfo2.renderPassInfo = &mainRenderPassInfo;



	// 1 renderPass per pipeline
	std::vector<VkRenderPass> renderPasses;

	//mainProc.create(windowHDL_GLFW, &window, 2u, imageFormat, { &pipelineInfo1/*, &pipelineInfo2*/}, &renderPasses);
	mainProc.create(windowHDL_GLFW, &window, 2u, imageFormat, { &pipelineInfo1, &pipelineInfo2 }, &renderPasses);

	window.createSwapChainFrameBuffers(window._swapChainExtent, {}, 0u, renderPasses[0], mainProc._device);


	const std::vector<res::vertex> vertices1 = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	VkBuffer vertexBuffer1 = NULL;
	VkDeviceMemory vertexBufferMem1 = NULL;
	mainProc.createVertexBuffer(vertices1.data(), vertices1.size(), sizeof(res::vertex), &vertexBuffer1, &vertexBufferMem1);


	std::vector<uint32_t> indices1 = {
		0, 1, 2, 2, 3, 0 };
	VkBuffer indexBuffer1 = NULL;
	VkDeviceMemory indexBufferMem1 = NULL;
	mainProc.createIndexBuffer(indices1.data(), indices1.size(), &indexBuffer1, &indexBufferMem1);
	//mainProc.render();
	//mainProc.display(window.getHandleGLFW());

	VkImage renderTargetImg;
	VkDeviceMemory renderTargImgMem;
	VkExtent2D secondaryRenderTargetExtent = { 800,800 };
	mainProc.createImage(secondaryRenderTargetExtent.width, secondaryRenderTargetExtent.height,imageFormat,VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderTargetImg, renderTargImgMem);
	mainProc.createImageView(renderTargetImg, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, &renderTargImgView);
	//mainProc.transitionImageLayout(renderTargetImg, imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	const std::vector<res::vertex> vertices2 = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	VkBuffer vertexBuffer2 = NULL;
	VkDeviceMemory vertexBufferMem2 = NULL;
	mainProc.createVertexBuffer(vertices2.data(), vertices2.size(), sizeof(res::vertex), &vertexBuffer2, &vertexBufferMem2);


	std::vector<uint16_t> indices2 = {
		0, 1, 2, 2, 3, 0 };
	VkBuffer indexBuffer2 = NULL;
	VkDeviceMemory indexBufferMem2 = NULL;
	mainProc.createIndexBuffer(indices2.data(), indices2.size(), &indexBuffer2, &indexBufferMem2);

	//////////////////////////////////////////////////////////////////
	///// SECONDARY RENDER PASS //////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	val::renderPassInfo renderPassInfo{};
	setupRenderPass2(renderPassInfo, imageFormat);

	VkRenderPass renderPass = mainProc.createRenderPass(&renderPassInfo);

	VkFramebufferCreateInfo framebufferInfo {};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPasses[1];
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &renderTargImgView;
	framebufferInfo.width = secondaryRenderTargetExtent.width;
	framebufferInfo.height = secondaryRenderTargetExtent.height;
	framebufferInfo.layers = 1;

	VkFramebuffer renderTargetFramebuffer;
	vkCreateFramebuffer(mainProc._device, &framebufferInfo, nullptr, &renderTargetFramebuffer);

	////////////////////////////////////////////////////////////

	fragShaderImage._imageViews[0] = &renderTargImgView;

	////////////////// CREATE DESCRIPTOR SETS //////////////////
	mainProc.createDescriptorSets(&pipelineInfo1,0u);
	mainProc.createDescriptorSets(&pipelineInfo2,1u);
	////////////////////////////////////////////////////////////



	VkClearValue clearValues[1];
	clearValues[0].color = { {0.0f, 0.2f, 1.0f, 1.0f} };

	VkClearValue clearValues2[1];
	clearValues2[0].color = { {1.0f, 1.0f, 0.0f, 1.0f} };


	uint32_t currentSC_ImageIdx = 0;

	mainProc.transitionImageLayout(renderTargetImg, imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

	while (!glfwWindowShouldClose(windowHDL_GLFW)) {
		glfwPollEvents();
		updateUniformBuffer(0 /*the vert shader is the first to be created, so it's index is 0*/, mainProc); // maybe updateDescriptorSet(s)?
		//updateUniformBuffer2(0/*the vert shader is the first to be created, so it's index is 0*/, mainProc); // maybe updateDescriptorSet(s)?
		////////////////////////////////////////////////////////////////////////////////////////////////////

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);
		window.waitForFences();
		mainProc.beginDraw(imageFormat);


		mainProc.drawFrameExperimental(1u, renderPasses[1], renderTargetFramebuffer,
			vertexBuffer2, indexBuffer2, indices2.data(), indices2.size(), imageFormat, clearValues2, 1u);

		// this needed because the render pass automatically transitions images into the final layout specified in its config
		// this is also needed to ensure that the renderTargetFramebuffer has finished rendering.

		mainProc.transitionImageLayout(renderTargetImg, imageFormat,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mainProc._commandBuffers[mainProc._currentFrame]);

		mainProc.drawFrameExperimental(0u, renderPasses[0], framebuffer, 
			vertexBuffer1, indexBuffer1, indices1.data(), indices1.size(),imageFormat, clearValues, 1u);


		mainProc.endDraw(imageFormat);
	}

	mainProc.cleanup(windowHDL_GLFW);

	return EXIT_SUCCESS;
}