/*
Copyright � 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the �Software�), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


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

void setGraphicsPipelineInfo(val::graphicsPipelineCreateInfo& pipeline, const VkSampleCountFlagBits& MSAAsamples)
{	using namespace val;

	pipeline.setSampleCount(MSAAsamples);  // required for multisampling

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
}

void setRenderPass(val::renderPassManager& renderPassMngr, VkFormat imgFormat, uint8_t MSAAsamples) {
	using namespace val;

	renderPassMngr.setMSAAsamples(MSAAsamples);  // required for multisampling

	static subpass subpass(renderPassMngr, GRAPHICS);
	{
		static colorAttachment colorAttach;
		colorAttach.setImgFormat(imgFormat);
		colorAttach.setLoadOperation(CLEAR);
		colorAttach.setStoreOperation(STORE);
		colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		subpass.bindAttachment(&colorAttach);
	}
	{
		static resolveAttachment resolveAttach;
		resolveAttach.setImgFormat(imgFormat);
		resolveAttach.setLoadOperation(DISCARD);
		resolveAttach.setStoreOperation(STORE);
		resolveAttach.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		subpass.bindAttachment(&resolveAttach);
	}
}

int main() {
	using namespace val;
	VAL_PROC proc;
	graphicsPipelineCreateInfo pipeline;

	physicalDeviceRequirements deviceRequirements(DEVICE_TYPES::dedicated_GPU | DEVICE_TYPES::integrated_GPU);
	deviceRequirements.deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	imageView imgView(proc);

	sampler imgSampler(proc, combinedImage);
	imgSampler.setMaxAnisotropy(0.f);

	VkFormat imageFormat;

	/////////// consider moving this into the window class ///////////
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable
	//////////////////////////////////////////////////////////////////

	VkExtent2D windowSize{ 800,800 }; // in pixels
	GLFWwindow* windowHDL_GLFW = glfwCreateWindow(windowSize.width, windowSize.height, "Test", NULL, NULL);
	val::window window(windowHDL_GLFW, &proc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, &window);

	// VAL uses image format requirements to pick the best image format out of a list of the formats input
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	imageFormat = val::findSupportedImageFormat(proc._physicalDevice, formatReqs);
	VkSampleCountFlagBits MSAAsamples = proc.getMaxSampleCount();
	
	// required for multisampling
	multisamplerManager multisamplerMngr(proc, MSAAsamples);
	multisamplerMngr.create(imageFormat, windowSize.width, windowSize.height);
	multisamplerMngr.setSampleCount(MSAAsamples);


	UBO_Handle uboHdl(sizeof(uniformBufferObject));
	// load and configure vert shader
	shader vertShader("shaders-compiled/shadervert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription() });
	vertShader._UBO_Handles = { {&uboHdl,0} };

	shader fragShader("shaders-compiled/imageshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
	fragShader.setImageSamplers({ {&imgSampler, 1} });


	// config grahics pipeline
	pipeline.shaders = { &vertShader, &fragShader };
	setGraphicsPipelineInfo(pipeline, MSAAsamples);

	renderPassManager renderPassMngr(proc);
	setRenderPass(renderPassMngr, imageFormat, MSAAsamples);
	pipeline.renderPass = &renderPassMngr;

	proc.create(windowHDL_GLFW, &window, 2u, imageFormat, { &pipeline });

	std::vector<VkImageView> attachments = { multisamplerMngr.getVkImageView()};  // required for multisampling
	window.createSwapChainFrameBuffers(window._swapChainExtent, attachments.data(), attachments.size(), pipeline.getVkRenderPass(), proc._device);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// AFTER VAL_PROC INIT //////////////////////////////////////////////////

	// CREATE IMAGE & IMAGE VIEW //
	val::image img(proc, "testImage.jpg", imageFormat);
	imgView.create(img, VK_IMAGE_ASPECT_COLOR_BIT);

	imgSampler.bindImageView(imgView); // it is imperative that this is called AFTER the image view is created

	const std::vector<res::vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};
	// buffer wrapper for vertex Buffer
	buffer vertexBuffer(proc, vertices.size() * sizeof(res::vertex), CPU_GPU, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	memcpy(vertexBuffer.getDataMapped(), (void*)vertices.data(), vertices.size() * sizeof(res::vertex));

	std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0 };
	buffer indexBuffer(proc, indices.size() * sizeof(uint32_t), CPU_GPU, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	memcpy(indexBuffer.getDataMapped(), (void*)indices.data(), indices.size() * sizeof(uint32_t));


	// this needs to be called only after the frag shader's image view has been set
	proc.createDescriptorSets(&pipeline);

	// configure the render target, setting vertex buffers, scissors, area, etc
	renderTarget renderTarget;
	renderTarget.setFormat(imageFormat);
	renderTarget.setRenderArea(window._swapChainExtent);
	renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
	renderTarget.setIndexBuffer(indexBuffer.getVkBuffer(), indices.size());
	renderTarget.setVertexBuffers({ vertexBuffer.getVkBuffer() }, vertices.size());
	// config viewport, covers the entire size of the window
	VkViewport viewport{ 0,0, window._swapChainExtent.width, window._swapChainExtent.height, 0.f, 1.f };


	while (!glfwWindowShouldClose(windowHDL_GLFW)) {
		auto& graphicsQueue = proc._graphicsQueue;
		auto& presentQueue = window._presentQueue;
		auto& currentFrame = proc._currentFrame;

		VkCommandBuffer cmdBuffer = proc._graphicsQueue._commandBuffers[currentFrame];
		glfwPollEvents();
		updateUniformBuffer(proc, uboHdl);

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);
		renderTarget.beginPass(proc,pipeline.getVkRenderPass(), framebuffer);
		renderTarget.update(proc, pipeline, { viewport });
		renderTarget.updateScissor(proc, { VkRect2D{ {0,0}, window._swapChainExtent } });
		renderTarget.render(proc);
		renderTarget.endPass(proc);

		renderTarget.submit(proc, { presentQueue._semaphores[currentFrame] }, presentQueue._fences[currentFrame]);
		window.display(imageFormat, { graphicsQueue._semaphores[currentFrame] });

		proc.nextFrame();
	}

	return EXIT_SUCCESS;
}