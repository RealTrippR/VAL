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

void updateImages(val::VAL_PROC& proc, VkSampler* imageSampler, VkImageView* newImageView, VkDescriptorSet descriptorSet) {
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageView = *newImageView;
	imageInfo.sampler = *imageSampler;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 1; // THIS MUST MATCH THE BINDING
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(proc._device, 1, &descriptorWrite, 0, nullptr);
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
	//renderPassInfo.attachmentImageLayouts = { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL/*This second value is the layout of the corresponding VkAttachmentReference*/ };

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

int main() {
	val::VAL_PROC mainProc;

	/////////// consider moving this into the window class ///////////
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable
	//////////////////////////////////////////////////////////////////

	GLFWwindow* windowHDL_GLFW = glfwCreateWindow(800, 800, "Test", NULL, NULL);

	val::window window(windowHDL_GLFW, &mainProc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	uniformBufferObject ubo;

	// VAL uses image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	// The shader class is poorly optimized and fucking retarded at the moment
	// load and configure vert shader
	val::shader vertShader("shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions().data(),
	res::vertex::getAttributeDescriptions().size());
	vertShader.setBindingDescription(res::vertex::getBindingDescription());
	vertShader.setUniformBufferData(&ubo, sizeof(ubo), 1);


	// load and configure frag shader
	// CONSIDER STORING IMAGE INFO INSIDE THE SHADER CLASS
	val::shader fragShader("shaders/imagefrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	VkSamplerCreateInfo samplerInfo{};
	setImageSamplerInfo(&samplerInfo);
	fragShader.setImageSamplers({ samplerInfo });

	VkImageView currentImgView;
	fragShader.setImageView({&currentImgView});

	// config grahics pipeline
	val::graphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.shaders.push_back(&vertShader); // consolidate into a single function

	pipelineInfo.shaders.push_back(&fragShader); // consolidate into a single function

	setGraphicsPipelineInfo(pipelineInfo);

	mainProc.initDevices(deviceExtensions, validationLayers, enableValidationLayers, &window);

	VkFormat imageFormat = val::findSupportedImageFormat(mainProc._physicalDevice, formatReqs);

	val::renderPassInfo renderPassInfo;
	setRenderPassInfo(renderPassInfo, imageFormat);
	pipelineInfo.renderPassInfo = &renderPassInfo;
	// 1 renderPass per pipeline
	std::vector<VkRenderPass> renderPasses;

	mainProc.create(windowHDL_GLFW, &window, 2U, imageFormat, { &pipelineInfo }, &renderPasses);

	window.createSwapChainFrameBuffers(window._swapChainExtent, {}, 0u, renderPasses[0], mainProc._device);



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// AFTER VAL_PROC INIT //////////////////////////////////////////////////
	//////////////////////////////////////////////////

	VkImageView imgView1;
	VkImageView imgView2;



	val::image img1(mainProc, "testImage.jpg", imageFormat);
	mainProc.createImageView(img1.getImage(), imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, &imgView1);

	val::image img2(mainProc, "testImage2.png", imageFormat);
	mainProc.createImageView(img2.getImage(), imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, &imgView2);
	


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



	currentImgView = imgView1;

	// sets the image view for the frag shader.
	fragShader._imageViews[0] = &imgView1;

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

		timer++;
		if (timer > 200) {
			timer = 0;
			imgNum = !imgNum;

			if (imgNum) {
				currentImgView = imgView2;
			}
			else {
				currentImgView = imgView1;
			}

			//std::cout << "IMAGE SWAPPED!\n";
		}

		// Wait idle for the GPU to finish executing the command buffer
		vkDeviceWaitIdle(mainProc._device);

		mainProc.endDraw(imageFormat);

		window.waitForFences(); // this is required because the update images function modifies the descriptor sets,
								// and in Vulkan we have to wait for the current command buffer to finish before we can modify or destroy them.

		const auto& currentFrame = mainProc._currentFrame;
		updateImages(mainProc, &fragShader._imageSamplers[0], &currentImgView, mainProc._descriptorSets[0][currentFrame]);
	}

	mainProc.cleanup(windowHDL_GLFW);

	vkDestroyImageView(mainProc._device, imgView1, NULL);
	vkDestroyImageView(mainProc._device, imgView2, NULL);


	return EXIT_SUCCESS;
}