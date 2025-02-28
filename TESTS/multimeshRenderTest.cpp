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
#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/graphics/shader.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VAL/lib/meshes&vertices/vertex3Dtextured.hpp"
#include "VAL/lib/meshes&vertices/meshTextured.hpp"

#include "testUtils.hpp"

// it is critical that Implementations are the last include

#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <ExternalLibraries/tiny_obj_loader.h>

const std::string MODEL_PATH = "res/viking_room.obj";
const std::string TEXTURE_PATH = "res/viking_room.png";

struct uniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct pushConstant {
	alignas(16) glm::vec3 translation;
};


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

void updateUniformBuffer(val::VAL_PROC& proc, val::UBO_Handle* uboHDL) {
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

	uboHDL->update(proc, &ubo);
}

void updatePushConstant(val::VAL_PROC& proc, val::pushConstantHandle* PC_HDL) {
	static pushConstant pc;

	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	time = sin(time);

	pc.translation = glm::vec3(0,0, time*.5);

	PC_HDL->update(proc, &pc, 0u);
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

void setGraphicsPipelineInfo(val::pipelineCreateInfo& info, VkSampleCountFlagBits msaaSamples) {
	// RASTERIZER
	VkPipelineRasterizationStateCreateInfo& rasterizer = info.rasterizer;
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;


	// MULTISAMPLING
	VkPipelineMultisampleStateCreateInfo& multisampling = info.multisampling;
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = msaaSamples;

	VkPipelineColorBlendAttachmentState& colorBlendAttachment = info.colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;


	// COLOR BLENDING
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


	// DEPTH STENCIL
	static VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;

	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;

	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;

	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	info.depthStencil = &depthStencil;
}



void setRenderPassInfo(val::renderPassInfo& renderPassInfo, VkFormat colorAttachmentFormat, VkFormat depthAttachmentFormat, VkSampleCountFlagBits msaaSamples) {

	// attachments - ALL MUST BE STATIC IN MEMORY
	static VkAttachmentDescription colorAttachment{};
	colorAttachment.format = colorAttachmentFormat;
	colorAttachment.samples = msaaSamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// MUST BE STATIC IN MEMORY
	static VkAttachmentDescription depthAttachment{};
	depthAttachment.format = depthAttachmentFormat;
	depthAttachment.samples = msaaSamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	static VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = colorAttachmentFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// MUST BE STATIC IN MEMORY
	static VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// MUST BE STATIC IN MEMORY
	static VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// THE FINAL COLOR ATTACHMENT REFERENCE MUST ALWAYS BE THE LAST ATTACHMENT 	// MUST BE STATIC IN MEMORY
	static VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// subpasses
	// MUST BE STATIC IN MEMORY
	static VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	// MUST BE STATIC IN MEMORY
	static VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


	renderPassInfo.subpasses = { subpass };
	renderPassInfo.subpassDependencies = { dependency };
	renderPassInfo.attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
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

	// FML uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements renderImageFormatReqs;
	renderImageFormatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	renderImageFormatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	renderImageFormatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	renderImageFormatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	mainProc.initDevices(deviceExtensions, validationLayers, enableValidationLayers, &window);

	VkFormat imageFormat = val::findSupportedImageFormat(mainProc._physicalDevice, renderImageFormatReqs);


	VkSampleCountFlagBits msaaSamples = mainProc.getMaxSampleCount();

	printf("\n>-- MSAA Sample count: %d\n\n", msaaSamples);



	val::UBO_Handle uboHDL(sizeof(uniformBufferObject));

	val::pushConstantHandle PC_HDL(sizeof(pushConstant));



	// THIS PROCESS MUST BE SIMPLIFIED
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	mainProc.createImage(windowSize.width, windowSize.height, imageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory, 1u, msaaSamples);
	// this function should be changed to image colorImageView in the front
	mainProc.createImageView(colorImage, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, &colorImageView, 1u);

	// The shader class is poorly optimized and fucking retarded at the moment
	// load and configure vert shader
	val::shader vertShader("shaders-compiled/shaderPushConstantTestvert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(val::vertex3D::getAttributeDescriptions().data(),
		val::vertex3D::getAttributeDescriptions().size());
	vertShader.setBindingDescription(val::vertex3D::getBindingDescription());
	vertShader.setUniformBuffers({&uboHDL});
	vertShader.setPushConstants({&PC_HDL});

	// load and configure frag shader
	// CONSIDER STORING IMAGE INFO INSIDE THE SHADER CLASS
	val::shader fragShader("shaders-compiled/imageshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	VkSamplerCreateInfo samplerInfo{};
	setImageSamplerInfo(&samplerInfo);
	fragShader.setImageSamplers({ samplerInfo });

	VkImageView imgView;
	fragShader.setImageView({ &imgView });

	// config grahics pipeline
	val::pipelineCreateInfo pipelineInfo;

	pipelineInfo.shaders.push_back(&vertShader); // consolidate into a single function
	pipelineInfo.shaders.push_back(&fragShader); // consolidate into a single function

	setGraphicsPipelineInfo(pipelineInfo, msaaSamples);

	// FML uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	val::imageFormatRequirements depthFormatReqs;
	depthFormatReqs.acceptedFormats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
	depthFormatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthFormatReqs.features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthFormatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	VkFormat depthFormat = val::findSupportedImageFormat(mainProc._physicalDevice, depthFormatReqs);


	val::renderPassInfo renderPassInfo;
	setRenderPassInfo(renderPassInfo, imageFormat, depthFormat, msaaSamples);
	pipelineInfo.renderPassInfo = &renderPassInfo;
	// 1 renderPass per pipeline
	std::vector<VkRenderPass> renderPasses;


	mainProc.create(windowHDL_GLFW, &window, 2u, imageFormat, { &pipelineInfo }, &renderPasses);

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
	mainProc.createImage(window._swapChainExtent.width, window._swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory, 1u, msaaSamples);
	mainProc.createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &depthImageView);
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<VkImageView> attachments = { colorImageView, depthImageView };
	window.createSwapChainFrameBuffers(window._swapChainExtent, attachments.data(), attachments.size(), renderPasses[0], mainProc._device);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// AFTER FML_PROC INIT //////////////////////////////////////////////////
	//////////////////////////////////////////////////

	// Create mesh and texture, load object and apply texture to the image
	val::image textureImg(mainProc, TEXTURE_PATH, imageFormat);
	val::meshTextured mesh;
	mesh.loadFromDiskObj(mainProc, MODEL_PATH, true);
	mesh.setTexture(mainProc, &textureImg);
	fragShader._imageViews[0] = &mesh._textureImageView;


	//////////////////////////////////////////////////////////////////

	mainProc.createDescriptorSets(&pipelineInfo, 0u);

	VkClearValue clearValues[2];

	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	while (!glfwWindowShouldClose(windowHDL_GLFW)) {
		glfwPollEvents();

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);
		mainProc.beginDraw(imageFormat);

		updateUniformBuffer(mainProc, &uboHDL);
		updatePushConstant(mainProc, &PC_HDL);

		mainProc.drawFrameExperimental(0u, renderPasses[0], framebuffer, mesh._vertexBuffer, mesh._indexBuffer, mesh._indices.data(),
			mesh._indices.size(), imageFormat, clearValues, uint16_t(sizeof(clearValues) / sizeof(VkClearValue)));
		mainProc.submit(imageFormat);

		window.waitForFences();
	}

	mainProc.cleanup(windowHDL_GLFW);

	vkDestroyImageView( mainProc._device, imgView, NULL);

	mainProc.cleanup(windowHDL_GLFW);

	return EXIT_SUCCESS;
}