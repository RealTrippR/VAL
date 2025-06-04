#include <iostream>
#include <string>
#include <chrono>

#ifdef NDEBUG
const bool enableValidationLayers = false;

#else
const bool enableValidationLayers = true;
#endif //!NDEBUG

#define FRAMES_IN_FLIGHT 2u

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/ext/gpu_vector.hpp>

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

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

void updateUniformBuffer(val::VAL_PROC& proc, val::UBO_Handle& hdl)
{	using namespace val;
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
{	using namespace val;

	// state infos
	static rasterizerState rasterizer;
	rasterizer.setCullMode(CULL_MODE::BACK);
	rasterizer.setTopologyMode(TOPOLOGY_MODE::FILL);
	pipeline.setRasterizer(&rasterizer);

	// the color blend state affects how the output of the fragmennt shader is 
	// blended into the existing content of the the framebuffer.
	static colorBlendStateAttachment colorBlendAttachment(false/*Disable blending*/);
	colorBlendAttachment.setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	/* A graphics pipeline can have as many color blend attachments as there are color attachments in the subpass it's associated with; no more, no less.*/
	static colorBlendState blendState; 
	blendState.bindBlendAttachment(&colorBlendAttachment);
	pipeline.setColorBlendState(&blendState);

	pipeline.setDynamicStates({ DYNAMIC_STATE::SCISSOR, DYNAMIC_STATE::VIEWPORT });
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

int main()
{
#ifndef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	namespace v = val;

	v::VAL_PROC proc;
	
	v::physicalDeviceRequirements deviceRequirements (v::DEVICE_TYPES::dedicated_GPU | v::DEVICE_TYPES::integrated_GPU);


	// Configure and create window
	v::windowProperties windowConfig;
	windowConfig.setProperty(v::WN_BOOL_PROPERTY::RESIZABLE, true);
	v::window window(windowConfig, 800, 800, "R_G_TEST", &proc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);


	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, &window);

	// VAL uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	VkFormat imageFormat = val::findSupportedImageFormat(proc._physicalDevice, formatReqs);

	val::UBO_Handle uboHdl(sizeof(uniformBufferObject));
	// load and configure vert shader
	val::shader vertShader("shaders-compiled/shadervert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription()});
	vertShader._UBO_Handles = { {&uboHdl,0} };

	// load and configure frag shader
	val::shader fragShader("shaders-compiled/colorshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
	//////////////////////////////////////////////////////////////

	val::graphicsPipelineCreateInfo pipeline;
	pipeline.shaders = { &vertShader,&fragShader };
	setGraphicsPipelineInfo(pipeline);

	val::renderPassManager renderPassMngr(proc);
	setRenderPass(renderPassMngr, imageFormat);
	pipeline.renderPass = &renderPassMngr;

	proc.create(&window, FRAMES_IN_FLIGHT, imageFormat, { &pipeline });
	



	// why is this still here? - for attachments?
	window.createSwapChainFrameBuffers(window._swapChainExtent, {}, 0u, pipeline.getVkRenderPass(), proc._device);




	val::gpu_vector<res::vertex> vertices(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		});

	val::gpu_vector<uint32_t> indices(proc, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		{ 0, 1, 2, 2, 3, 0 }
	);


	//////////////////////////////////////////////////////////////
	// create descriptor sets - this should be merged into the
	// pipeline creation function
	proc.createDescriptorSets(&pipeline);
	//////////////////////////////////////////////////////////////


	// configure the render target, setting vertex buffers, scissors, area, etc
	val::renderTarget renderTarget;
	renderTarget.setFormat(imageFormat);
	renderTarget.setRenderArea(window.getSize());
	renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
	// Note that simply setting the index and vertex buffers does not update them in current command buffer, they have to be binded using rt.updateBuffers() or rt.update()
	renderTarget.setIndexBuffer(indexBuffer, indices.size());
	renderTarget.setVertexBuffer(vertexBuffer, vertices.size());

	// config viewport, covers the entire size of the window
	VkViewport viewport{ 0,0, window.getSize().width, window.getSize().height, 0.f, 1.f };

	
	while (!window.shouldClose()) {
		glfwPollEvents();


		auto& graphicsQueue = proc._graphicsQueue;
		auto& presentQueue = window._presentQueue;
		auto& currentFrame = proc._currentFrame;

		VkCommandBuffer cmdBuffer = proc._graphicsQueue._commandBuffers[currentFrame];
		// Update view information, stored in a UBO
		updateUniformBuffer(proc, uboHdl);

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);
		renderTarget.begin(proc);

		renderTarget.beginPass(proc, pipeline.getVkRenderPass(), framebuffer);
		renderTarget.updateBuffers(proc);
		renderTarget.updatePipeline(proc, pipeline);
		renderTarget.updateViewport(proc, viewport, 0);
		renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window.getSize()});
		renderTarget.render(proc);
		renderTarget.endPass(proc);

		renderTarget.submit(proc, { presentQueue._semaphores[currentFrame] }, window.getPresentFence());
		window.display(imageFormat, { graphicsQueue._semaphores[currentFrame] });

		proc.nextFrame();
	}

	glfwTerminate();
#ifndef NDEBUG
	_CrtDumpMemoryLeaks();
#endif // !NDEBUG

	return EXIT_SUCCESS;
}