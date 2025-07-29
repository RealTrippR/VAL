#include <iostream>
#include <string>
#include <chrono>

#ifdef NDEBUG
const bool enableValidationLayers = false;

#else
const bool enableValidationLayers = true;
#endif //!NDEBUG

#define FRAMES_IN_FLIGHT 2u

#define IMG_FORMAT VK_FORMAT_R8G8B8A8_SRGB

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/ext/gpu_vector.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vertex.hpp"

// it is important that this comes last
#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>

struct ViewMatrix {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

void updateViewMatrix(val::ValProc& proc, val::Window& window, val::UBO_Handle& hdl)
{
	using namespace val;
	const VkExtent2D& extent = window.getSize();
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	static ViewMatrix ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	hdl.update(proc, &ubo);
}

void setGraphicsPipelineInfo(val::GraphicsPipeline& pipeline)
{
	using namespace val;

	// state infos
	static rasterizerState rasterizer;
	rasterizer.setCullMode(CULL_MODE::BACK);
	rasterizer.setTopologyMode(TOPOLOGY_MODE::FILL);
	pipeline.setRasterizer(&rasterizer);

	// the color blend state affects how the output of the fragmennt shader is 
	// blended into the existing content of the the framebuffer.
	static ColorBlendStateAttachment colorBlendAttachment(false/*Disable blending*/);
	colorBlendAttachment.setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	/* A graphics pipeline can have as many color blend attachments as there are color attachments in the subpass it's associated with; no more, no less.*/
	static ColorBlendState blendState;
	blendState.bindBlendAttachment(&colorBlendAttachment);
	pipeline.setColorBlendState(&blendState);

	pipeline.setDynamicStates({ DYNAMIC_STATE::Scissor, DYNAMIC_STATE::Viewport });
}

void setRenderPass(val::RenderPassManager& renderPassMngr, VkFormat imgFormat) {
	using namespace val;
	static ColorAttachment colorAttach;
	colorAttach.setImgFormat(imgFormat);
	colorAttach.setLoadOperation(Clear);
	colorAttach.setStoreOperation(Store);
	colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	static Subpass subpass(renderPassMngr, Graphics);
	subpass.bindAttachment(&colorAttach);
}

int main()
{
#ifndef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	using namespace val;

	ValProc proc;

	PhysicalDeviceRequirements deviceRequirements(DEVICE_TYPES::dedicated_GPU | DEVICE_TYPES::integrated_GPU);

	// Configure and create window
	WindowProperties windowConfig;
	windowConfig.setProperty(WN_BOOL_PROPERTY::Resizable, true);
	Window window1(windowConfig, 800, 800, "Multiple Window Test - Window #1", proc);
	Window window2(windowConfig, 800, 800, "Multiple Window Test - Window #2", proc);


	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, QUEUE_FLAGS::Graphics);

	val::UBO_Handle viewUBO(sizeof(ViewMatrix));

	DescriptorSheet descSheet(
		{ {0, viewUBO, SHADER_STAGE::Vertex} },
		FRAMES_IN_FLIGHT
	);

	// load and configure vert shader
	val::Shader vertShader("shaders-compiled/shader.vert.spv", SHADER_STAGE::Vertex, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription() });

	// load and configure frag shader
	val::Shader fragShader("shaders-compiled/colorshader.frag.spv", SHADER_STAGE::Fragment, "main");
	//////////////////////////////////////////////////////////////


	val::GraphicsPipeline pipeline;
	pipeline.setDescriptorSheet(&descSheet);
	pipeline.setShaders({ &vertShader,&fragShader });
	setGraphicsPipelineInfo(pipeline);

	val::RenderPassManager renderPassMngr(proc);
	setRenderPass(renderPassMngr, IMG_FORMAT);
	pipeline.setRenderPassManager(&renderPassMngr);

	proc.create(FRAMES_IN_FLIGHT, {&pipeline});

	window1.create(IMG_FORMAT, pipeline.getVkRenderPass());
	window2.create(IMG_FORMAT, pipeline.getVkRenderPass());

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
	// create descriptor sets
	pipeline.allocateAndWriteDescriptorSets(proc);
	//////////////////////////////////////////////////////////////






	Queue graphicsQueue(proc, QUEUE_FLAGS::Graphics);



	// configure the render target, setting vertex buffers, scissors, area, etc
	val::renderTarget renderTarget;
	renderTarget.setQueue(graphicsQueue);
	renderTarget.setFormat(IMG_FORMAT);
	renderTarget.setRenderArea(window1.getSize());
	renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
	// Note that simply setting the index and vertex buffers does not update them in current command buffer, they have to be binded using rt.updateBuffers() or rt.update()
	renderTarget.setIndexBuffer(indices, indices.size());
	renderTarget.setVertexBuffer(vertices, vertices.size());
	// config viewport, covers the entire size of the window
	VkViewport viewport{ 0,0, window1.getSize().width, window1.getSize().height, 0.f, 1.f };

	Fence presentFence;
	presentFence.create(proc);

	while (!window1.shouldClose()) {
		window1.pollEvents();
		window2.pollEvents();

		// Update view information, stored in a UBO
		updateViewMatrix(proc, window1, viewUBO);

		VkFramebuffer framebuffer1 = window1.beginDraw(IMG_FORMAT);
		VkFramebuffer framebuffer2 = window2.beginDraw(IMG_FORMAT);

		renderTarget.begin(proc);

		renderTarget.beginPass(proc, pipeline.getVkRenderPass(), framebuffer1);
		renderTarget.updateBuffers(proc);
		renderTarget.updatePipeline(proc, pipeline);
		renderTarget.updateViewport(proc, viewport, 0);
		renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window1.getSize() });
		renderTarget.updateDescriptorSet(proc, pipeline, descSheet, proc.getCurrentFrame());
		renderTarget.render(proc);
		renderTarget.endPass(proc);

		renderTarget.beginPass(proc, pipeline.getVkRenderPass(), framebuffer2);
		renderTarget.updateBuffers(proc);
		renderTarget.updatePipeline(proc, pipeline);
		renderTarget.updateViewport(proc, viewport, 0);
		renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window2.getSize() });
		renderTarget.updateDescriptorSet(proc, pipeline, descSheet, proc.getCurrentFrame());
		renderTarget.render(proc);
		renderTarget.endPass(proc);

		renderTarget.submit(proc,
			{ window1.getPresentSemaphore(), window2.getPresentSemaphore()},
			presentFence);

		presentFence.wait(proc);
		window1.display(IMG_FORMAT, { graphicsQueue.getSemaphore() });
		window2.display(IMG_FORMAT, {});
		presentFence.reset(proc);

		proc.nextFrame();
	}

	glfwTerminate();
#ifndef NDEBUG
	_CrtDumpMemoryLeaks();
#endif // !NDEBUG
	presentFence.destroy(proc);
	return EXIT_SUCCESS;
}