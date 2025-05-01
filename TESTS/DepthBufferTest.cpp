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


#include <iostream>
#include <string>
#include <chrono>

#ifdef NDEBUG
const bool enableValidationLayers = false;

#else
const bool enableValidationLayers = true;
#endif //!NDEBUG

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

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

#include <VAL\lib\meshes&vertices\vertex3Dtextured.hpp>;

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

void setGraphicsPipelineInfo(val::graphicsPipelineCreateInfo& pipeline)
{
	using namespace val;

	// state infos
	static rasterizerState rasterizer;
	rasterizer.setCullMode(CULL_MODE::BACK);
	rasterizer.setTopologyMode(TOPOLOGY_MODE::FILL);
	pipeline.setRasterizer(&rasterizer);

	// the color blend state affects how the output of the fragmennt shader is 
	// blended into the existing content of the the framebuffer.
	static colorBlendStateAttachment colorBlendAttachment(false/*Disable blending*/);
	colorBlendAttachment.setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	static depthStencilState depthState;
	depthState.enableDepthTesting(true);
	depthState.setCompareOp(VK_COMPARE_OP_LESS);
	pipeline.setDepthStencilState(&depthState);

	static colorBlendState blendState;
	blendState.bindBlendAttachment(&colorBlendAttachment);
	pipeline.setColorBlendState(&blendState);



	pipeline.setDynamicStates({ DYNAMIC_STATE::SCISSOR, DYNAMIC_STATE::VIEWPORT });

}


void setRenderPass(val::renderPassManager& renderPassMngr, VkFormat imgFormat, VkFormat depthFormat) {
	using namespace val;

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
}

int main()
{
	namespace v = val;

	v::VAL_PROC proc;

	val::physicalDeviceRequirements deviceRequirements(v::DEVICE_TYPES::dedicated_GPU | v::DEVICE_TYPES::integrated_GPU);
	deviceRequirements.deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	val::imageView imgView(proc);


	/////////// consider moving this into the window class ///////////
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable
	//////////////////////////////////////////////////////////////////

	GLFWwindow* windowHDL_GLFW = glfwCreateWindow(800, 800, "Test", NULL, NULL);

	v::window window(windowHDL_GLFW, &proc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);


	// VAL uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	v::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };


	// load and configure vert shader
	v::shader vertShader("shaders-compiled/shader3Dimagevert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(val::vertex3D::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ val::vertex3D::getBindingDescription() });

	v::UBO_Handle uboHdl(sizeof(uniformBufferObject));
	vertShader._UBO_Handles = { {&uboHdl,0} };

	// load and configure frag shader
	// CONSIDER STORING IMAGE INFO INSIDE THE SHADER CLASS
	v::shader fragShader("shaders-compiled/imageshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	v::sampler imgSampler(proc, v::combinedImage);
	fragShader.setImageSamplers({ { &imgSampler, 1 } });

	// config grahics pipeline
	v::graphicsPipelineCreateInfo pipeline;
	pipeline.shaders.push_back(&vertShader); // consolidate into a single function
	pipeline.shaders.push_back(&fragShader); // consolidate into a single function
	setGraphicsPipelineInfo(pipeline);

	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, &window);

	VkFormat imageFormat = val::findSupportedImageFormat(proc._physicalDevice, formatReqs);

	val::imageFormatRequirements depthFormatReqs;
	depthFormatReqs.acceptedFormats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
	depthFormatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthFormatReqs.features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthFormatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	VkFormat depthFormat = val::findSupportedImageFormat(proc._physicalDevice, depthFormatReqs);

	val::renderPassManager renderPassMngr(proc);
	setRenderPass(renderPassMngr, imageFormat, depthFormat);
	pipeline.renderPass = &renderPassMngr;


	proc.create(windowHDL_GLFW, &window, 2u, imageFormat, { &pipeline });


	// Create depth buffer
	val::depthBuffer depthBuffer(proc, window._swapChainExtent, depthFormat, 1u);
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<VkImageView> attachments = { depthBuffer.imgViews.front()};
	window.createSwapChainFrameBuffers(window._swapChainExtent, attachments.data(), attachments.size(), pipeline.getVkRenderPass(), proc._device);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// AFTER FML_PROC INIT //////////////////////////////////////////////////
	//////////////////////////////////////////////////

	// CREATE IMAGE & IMAGE VIEW //
	val::image img(proc, "testImage.jpg", imageFormat);
	imgView.create(img, VK_IMAGE_ASPECT_COLOR_BIT);
	imgSampler.bindImageView(imgView);


	std::vector<val::vertex3D> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	};

	VkBuffer vertexBuffer = NULL;
	VkDeviceMemory vertexBufferMem = NULL;
	proc.createVertexBuffer(vertices.data(), vertices.size(), sizeof(val::vertex3D), &vertexBuffer, &vertexBufferMem);

	std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	VkBuffer indexBuffer = NULL;
	VkDeviceMemory indexBufferMem = NULL;
	proc.createIndexBuffer(indices.data(), indices.size(), &indexBuffer, &indexBufferMem);

	proc.createDescriptorSets(&pipeline);

	val::renderTarget renderTarget;
	renderTarget.setFormat(imageFormat);
	renderTarget.setRenderArea(window._swapChainExtent);
	renderTarget.setClearValues({ 
		{.depthStencil { 1.0f, 0 } },
		{.color { 0.0f, 0.0f, 0.0f, 1.0f } }
	});
	renderTarget.setIndexBuffer(indexBuffer, indices.size());
	renderTarget.setVertexBuffers({ vertexBuffer }, vertices.size());

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