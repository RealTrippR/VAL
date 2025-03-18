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

void updateUniformBuffer(val::VAL_PROC& proc, val::UBO_Handle& hdl) {
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

	hdl.update(proc, &ubo);
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
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

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

	val::UBO_Handle uboHdl(sizeof(uniformBufferObject));
	// load and configure vert shader
	val::shader vertShader("shaders-compiled/shadervert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription() });
	vertShader._UBO_Handles = { {&uboHdl,0} };

	// load and configure frag shader
	// CONSIDER STORING IMAGE INFO INSIDE THE SHADER CLASS
	val::shader fragShaderImage("shaders-compiled/imageshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	VkSamplerCreateInfo samplerInfo{};
	setImageSamplerInfo(&samplerInfo);
	fragShaderImage.setImageSamplers({ { samplerInfo,1 } });

	VkImageView renderTargImgView;
	fragShaderImage._imageViews = { &renderTargImgView };
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

	val::UBO_Handle uboHdl2(sizeof(uniformBufferObject));
	val::shader vertShader2("shaders-compiled/shadervert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader2.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader2.setBindingDescriptions({ res::vertex::getBindingDescription() });
	vertShader2._UBO_Handles = {{ &uboHdl2,0 }};

	val::shader fragShaderColor("shaders-compiled/colorshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	pipelineInfo2.shaders = { &vertShader2,&fragShaderColor };


	//////////////////////////////////////////////////////////////////
	///// SECONDARY RENDER PASS //////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	val::renderPassInfo secondaryRenderPassInfo{};
	setupRenderPass2(secondaryRenderPassInfo, imageFormat);
	setGraphicsPipelineInfo(pipelineInfo2);
	pipelineInfo2.renderPassInfo = &secondaryRenderPassInfo;

	// 1 renderPass per pipeline
	std::vector<VkRenderPass> renderPasses;

	mainProc.create(windowHDL_GLFW, &window, 2u, imageFormat, { &pipelineInfo1, &pipelineInfo2 }, &renderPasses);
	window.createSwapChainFrameBuffers(window._swapChainExtent, {}, 0u, renderPasses[0], mainProc._device);

	VkImage renderTargetImg;
	VkDeviceMemory renderTargImgMem;
	VkExtent2D secondaryRenderTargetExtent = { 800,800 };
	mainProc.createImage(secondaryRenderTargetExtent.width, secondaryRenderTargetExtent.height,imageFormat,VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderTargetImg, renderTargImgMem);
	mainProc.createImageView(renderTargetImg, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, &renderTargImgView);

	const std::vector<res::vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	// buffer wrapper for vertex Buffer
	val::buffer vertexBuffer(mainProc, vertices.size() * sizeof(res::vertex), CPU_GPU, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	memcpy(vertexBuffer.getDataMapped(), (void*)vertices.data(), vertices.size() * sizeof(res::vertex));

	std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0 };
	val::buffer indexBuffer(mainProc, indices.size() * sizeof(uint32_t), CPU_GPU, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	memcpy(indexBuffer.getDataMapped(), (void*)indices.data(), indices.size() * sizeof(uint32_t));

	//////////////////////////////////////////////////////////////////
	// create secondary vertex and index buffers
	// buffer wrapper for vertex Buffer
	val::buffer vertexBuffer2(mainProc, vertices.size() * sizeof(res::vertex), CPU_GPU, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	memcpy(vertexBuffer2.getDataMapped(), (void*)vertices.data(), vertices.size() * sizeof(res::vertex));

	val::buffer indexBuffer2(mainProc, indices.size() * sizeof(uint32_t), CPU_GPU, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	memcpy(indexBuffer2.getDataMapped(), (void*)indices.data(), indices.size() * sizeof(uint32_t));

	VkFramebufferCreateInfo framebufferInfo{};
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
	mainProc.createDescriptorSets(&pipelineInfo1);
	mainProc.createDescriptorSets(&pipelineInfo2);
	////////////////////////////////////////////////////////////

	// configure the render target, setting vertex buffers, scissors, area, etc
	val::renderTarget renderTarget;
	renderTarget.setFormat(imageFormat);
	renderTarget.setArea(window._swapChainExtent);
	renderTarget.setScissorExtent(window._swapChainExtent);
	renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
	renderTarget.setIndexBuffer(indexBuffer.getVkBuffer(), indices.size());
	renderTarget.setVertexBuffers({ vertexBuffer.getVkBuffer() }, vertices.size());

	// config viewport, covers the entire size of the window
	VkViewport viewport{ 0,0, window._swapChainExtent.width, window._swapChainExtent.height, 0.f, 1.f };

	mainProc.transitionImageLayout(renderTargetImg, imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	while (!glfwWindowShouldClose(windowHDL_GLFW)) {
		auto& graphicsQueue = mainProc._graphicsQueue;
		auto& presentQueue = window._presentQueue;
		auto& currentFrame = mainProc._currentFrame;

		VkCommandBuffer cmdBuffer = mainProc._graphicsQueue._commandBuffers[currentFrame];
		glfwPollEvents();
		updateUniformBuffer(mainProc, uboHdl);
		updateUniformBuffer(mainProc, uboHdl2);

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);

		renderTarget.beginPass(mainProc);
		renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
		renderTarget.update(mainProc, pipelineInfo1.pipelineIdx);
		renderTarget.render(mainProc, { viewport }, renderPasses[pipelineInfo1.pipelineIdx], framebuffer);

		renderTarget.setIndexBuffer(indexBuffer2.getVkBuffer(), indices.size());
		renderTarget.setVertexBuffers({ vertexBuffer2.getVkBuffer() }, vertices.size());
		renderTarget.setClearValues({ { 0.0f, 0.2f, 0.5f, 1.0f } });
		renderTarget.update(mainProc, pipelineInfo2.pipelineIdx);
		renderTarget.render(mainProc, { viewport }, renderPasses[pipelineInfo2.pipelineIdx], renderTargetFramebuffer);
	
		mainProc.transitionImageLayout(renderTargetImg, imageFormat,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL/*this should match the colorAttachment.finalLayout of the 2nd render pass*/, 
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			graphicsQueue._commandBuffers[mainProc._currentFrame]);

		renderTarget.submit(mainProc, { presentQueue._semaphores[currentFrame] }, presentQueue._fences[currentFrame]);
		window.display(imageFormat, { graphicsQueue._semaphores[currentFrame] });

		mainProc.nextFrame();
	}

	vkDestroyImageView(mainProc._device, renderTargImgView, NULL);
	vertexBuffer.destroy();
	indexBuffer.destroy();
	mainProc.cleanup();

	return EXIT_SUCCESS;
}