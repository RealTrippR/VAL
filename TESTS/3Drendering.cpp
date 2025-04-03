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


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

void updateUniformBuffer(val::VAL_PROC& proc, val::UBO_Handle& hdl) {
	using namespace val;
	VkExtent2D& extent = proc._windowVAL->_swapChainExtent;
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	const float CAM_DIST = 3.0f;
	static uniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), .5f * time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(CAM_DIST, CAM_DIST, CAM_DIST), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	hdl.update(proc, &ubo);
}
void setGraphicsPipelineInfo(val::graphicsPipelineCreateInfo& pipeline, const VkSampleCountFlagBits& MSAAsamples)
{
	using namespace val;
	// state infos (MUST BE STATIC IN MEMORY!)
	static rasterizerState rasterizer;
	rasterizer.setCullMode(CULL_MODE::BACK);
	rasterizer.setTopologyMode(TOPOLOGY_MODE::FILL);
	pipeline.setRasterizer(&rasterizer);

	// the color blend state affects how the output of the fragment shader is 
	// blended into the existing content of the the framebuffer.
	static colorBlendStateAttachment colorBlendAttachment(false/*Disable blending*/);
	colorBlendAttachment.setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	static colorBlendState blendState;
	blendState.bindBlendAttachment(&colorBlendAttachment);
	pipeline.setColorBlendState(&blendState);

	pipeline.setDynamicStates({ DYNAMIC_STATE::SCISSOR, DYNAMIC_STATE::VIEWPORT });

	pipeline.setSampleCount(MSAAsamples); // required for multisampling
}

void setRenderPass(val::renderPassManager& renderPassMngr, VkFormat imgFormat, VkFormat depthFormat, VkSampleCountFlagBits MSAAsamples)
{ using namespace val;

	renderPassMngr.setMSAAsamples(MSAAsamples); // required for multisampling


	static subpass subpass(renderPassMngr, GRAPHICS);

	{
		static depthAttachment depthAttach;
		depthAttach.setImgFormat(depthFormat);
		depthAttach.setLoadOperation(CLEAR);
		depthAttach.setStoreOperation(DISCARD);
		depthAttach.setFinalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		subpass.bindAttachment(&depthAttach);
	}
	{
		static colorAttachment colorAttach;
		colorAttach.setImgFormat(imgFormat);
		colorAttach.setLoadOperation(CLEAR);
		colorAttach.setStoreOperation(STORE);
		colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		subpass.bindAttachment(&colorAttach);
	}
	{
		static resolveAttachment resolveAttach; // required for multisampling
		resolveAttach.setImgFormat(imgFormat);
		resolveAttach.setLoadOperation(DISCARD);
		resolveAttach.setStoreOperation(STORE);
		resolveAttach.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		subpass.bindAttachment(&resolveAttach);
	}
}

int main() {
	using namespace val;
	val::VAL_PROC proc;

	val::physicalDeviceRequirements deviceRequirements(DEVICE_TYPES::dedicated_GPU | DEVICE_TYPES::integrated_GPU);
	deviceRequirements.deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	val::meshTextured mesh(proc);
	val::sampler imgSampler(proc, val::combinedImage);
	imgSampler.bindImageView(mesh._textureImageView);
	/////////// consider moving this into the window class ///////////
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable
	//////////////////////////////////////////////////////////////////

	VkExtent2D windowSize{ 1000,800 }; // in pixels
	GLFWwindow* windowHDL_GLFW = glfwCreateWindow(windowSize.width, windowSize.height, "Test", NULL, NULL);

	val::window window(windowHDL_GLFW, &proc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

	val::UBO_Handle uboHdl(sizeof(uniformBufferObject));

	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, &window);

	// FML uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements renderImageFormatReqs;
	renderImageFormatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	renderImageFormatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	renderImageFormatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	renderImageFormatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	VkFormat imageFormat = val::findSupportedImageFormat(proc._physicalDevice, renderImageFormatReqs);

	VkSampleCountFlagBits msaaSamples = proc.getMaxSampleCount();
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(proc._physicalDevice, &deviceProperties);

	// required for multisampling
	multisamplerManager multisamplerMngr(proc, msaaSamples);
	multisamplerMngr.create(imageFormat, windowSize.width, windowSize.height);

	// load and configure shaders
	val::shader vertShader("shaders-compiled/shader3Dimagevert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(val::vertex3D::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ val::vertex3D::getBindingDescription() });
	vertShader._UBO_Handles = { { &uboHdl, 0 } };

	// load and configure frag shader
	val::shader fragShader("shaders-compiled/imageshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
	fragShader.setImageSamplers({ { &imgSampler, 1 } });
	
	// config grahics pipeline
	val::graphicsPipelineCreateInfo pipeline;
	pipeline.shaders = { &vertShader,&fragShader };
	setGraphicsPipelineInfo(pipeline, msaaSamples);

	val::imageFormatRequirements depthFormatReqs;
	depthFormatReqs.acceptedFormats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
	depthFormatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthFormatReqs.features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthFormatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	VkFormat depthFormat = val::findSupportedImageFormat(proc._physicalDevice, depthFormatReqs);


	renderPassManager renderPassMngr(proc);
	setRenderPass(renderPassMngr, imageFormat, depthFormat, msaaSamples);
	pipeline.renderPass = &renderPassMngr;

	proc.create(windowHDL_GLFW, &window, 2u, imageFormat, { &pipeline });

	// Create depth buffer
	val::depthBuffer depthBuffer;
	depthBuffer.create(proc, window._swapChainExtent, depthFormat, 1u, 1u, msaaSamples);
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<VkImageView> attachments = { depthBuffer.imgViews.front(), multisamplerMngr.getVkImageView() };
	window.createSwapChainFrameBuffers(window._swapChainExtent, attachments.data(), attachments.size(), pipeline.getVkRenderPass(), proc._device);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// AFTER FML_PROC INIT //////////////////////////////////////////////////
	//////////////////////////////////////////////////
	// Create mesh and texture, load object and apply texture to the image
	val::image textureImg(proc, TEXTURE_PATH, imageFormat);
	mesh.loadFromDiskObj(proc, MODEL_PATH, true);
	mesh.setTexture(proc, &textureImg);

	//////////////////////////////////////////////////////////////////
	proc.createDescriptorSets(&pipeline);

	val::renderTarget renderTarget;
	renderTarget.setFormat(imageFormat);
	renderTarget.setRenderArea(window._swapChainExtent);
	renderTarget.setClearValues({
		{.depthStencil { 1.0f, 0 } },
		{.color { 0.0f, 0.0f, 0.0f, 1.0f } }
		});
	renderTarget.setIndexBuffer(mesh._indexBuffer, mesh._indices.size());
	renderTarget.setVertexBuffers({ mesh._vertexBuffer }, mesh._vertices.size());

	// config viewport, covers the entire size of the window
	VkViewport viewport{ 0,0, window._swapChainExtent.width, window._swapChainExtent.height, 0.f, 1.f };

	while (!glfwWindowShouldClose(windowHDL_GLFW)) {
		glfwPollEvents();
		// INSTEAD OF UPDATING HERE, ADD A METHOD TO UPDATE UBOS VIA THE SHADER
		// ALSO, THERE IS EXCESS COPYING IN THIS FUNCTION

		auto& graphicsQueue = proc._graphicsQueue;
		auto& presentQueue = window._presentQueue;
		auto& currentFrame = proc._currentFrame;

		VkCommandBuffer cmdBuffer = proc._graphicsQueue._commandBuffers[currentFrame];
		glfwPollEvents();
		updateUniformBuffer(proc, uboHdl);


		VkFramebuffer framebuffer = window.beginDraw(imageFormat);
		renderTarget.beginPass(proc, pipeline.getVkRenderPass(), framebuffer);
		renderTarget.update(proc, pipeline, { viewport });
		renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window._swapChainExtent });
		renderTarget.render(proc);
		renderTarget.endPass(proc);

		renderTarget.submit(proc, { presentQueue._semaphores[currentFrame] }, presentQueue._fences[currentFrame]);
		window.display(imageFormat, { graphicsQueue._semaphores[currentFrame] });
		
		proc.nextFrame();
	}

	return EXIT_SUCCESS;
}