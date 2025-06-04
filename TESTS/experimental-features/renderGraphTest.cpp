#include <iostream>
#include <string>
#include <chrono>

#define VAL_ENABLE_EXPIREMENTAL // for render graphs and gpu_vector

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

#include "../vertex.hpp"

// it is important that this comes last
#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>

//#define VAL_RENDER_PASS_COMPILE_MODE
#include <VAL/lib/renderGraph/renderGraph.hpp>
#include <VAL/lib/renderGraph/passFunctionDefinitions.hpp>

/************************************************/
#include GRAPH_FILE(renderGraph_Draft);
/************************************************/

struct uniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
void updateUniformBuffer(val::VAL_PROC& proc, val::UBO_Handle& hdl)
{
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

	static colorBlendState blendState;
	blendState.bindBlendAttachment(&colorBlendAttachment);
	pipeline.setColorBlendState(&blendState);

	pipeline.setDynamicStates({ DYNAMIC_STATE::SCISSOR, DYNAMIC_STATE::VIEWPORT });
}


val::subpass& setRenderPass(val::renderPassManager& renderPassMngr, VkFormat imgFormat) {
	namespace v = val;
	static v::colorAttachment colorAttach;
	colorAttach.setImgFormat(imgFormat);
	colorAttach.setLoadOperation(v::CLEAR);
	colorAttach.setStoreOperation(v::STORE);
	colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	static v::subpass subpass(renderPassMngr, v::GRAPHICS);
	subpass.bindAttachment(&colorAttach);
	return subpass;
}

void intercept(const char* msg, bool* block) {
	printf("bar, bar: ");
	printf(msg);
	printf("\n");
	*block = false;
}


void interceptNo2(const char* msg, bool* block) {
	printf("foo, foo");
	printf(msg);
	printf("\n");

	*block = false;
}
int main()
{
#ifndef NDEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	val::dbg::addNoteIntercept(intercept);
	val::dbg::addNoteIntercept(interceptNo2);

	using namespace val;

	VAL_PROC proc;

	physicalDeviceRequirements deviceRequirements(DEVICE_TYPES::dedicated_GPU | DEVICE_TYPES::integrated_GPU);

	// Configure and create window
	windowProperties windowConfig;
	windowConfig.setProperty(WN_BOOL_PROPERTY::RESIZABLE, true);
	window window(windowConfig, 800,800, "R_G_TEST", &proc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

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
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription() });
	vertShader._UBO_Handles = { {&uboHdl,0} };

	// load and configure frag shader
	val::shader fragShader("shaders-compiled/colorshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
	//////////////////////////////////////////////////////////////

	val::graphicsPipelineCreateInfo pipeline;
	pipeline.shaders = { &vertShader,&fragShader };
	setGraphicsPipelineInfo(pipeline);

	/* * * * * * * * * * * * * * * * * * * */
	// configure render pass and subpass
	val::renderPassManager renderPassMngr(proc);
	val::subpass subpass = setRenderPass(renderPassMngr, imageFormat);
	/* * * * * * * * * * * * * * * * * * * */

	pipeline.renderPass = &renderPassMngr;

	proc.create(&window, FRAMES_IN_FLIGHT, imageFormat, { &pipeline });

	window.createSwapChainFrameBuffers(window._swapChainExtent, {}, 0u, pipeline.getVkRenderPass(), proc._device);

	gpu_vector<res::vertex> vertices(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	});

	gpu_vector<uint32_t> indices(proc, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		{0, 1, 2, 2, 3, 0 }
	);

	//////////////////////////////////////////////////////////////
	proc.createDescriptorSets(&pipeline);
	//////////////////////////////////////////////////////////////

	//// Note that simply setting the index and vertex buffers does not automatically update
	//// them in current command buffer, they have to be binded using rt.updateBuffers() or rt.update()
	//// every frame that the command buffer is reset
	//renderTarget.setIndexBuffer(indexBuffer, indices.size());
	//renderTarget.setVertexBuffer(vertexBuffer, vertices.size());
	
	RENDER_GRAPH renderGraph;
	renderGraph.loadFromFile("experimental-features/renderGraph_Draft.hpp");

	renderGraph.compile(proc._MAX_FRAMES_IN_FLIGHT,filepath("experimental-features"));


	PASS_CONTEXT passContext = {
		proc,
		window.getSizeAsRect2D(),
		{ { 0.0f, 0.04f, 0.2f, 1.0f } } /*clear values*/
	};

	auto& graphicsQueue = proc._graphicsQueue;


	VkCommandBuffer cmd;
	BAKE_RENDER_PASS(DRAW_RECT, proc,
		READ(vertices, indices)
		INPUT(pipeline, window, cmd),
		pipeline.getVkRenderPass(),
		0
	);


	while (!window.shouldClose()) {
		auto& graphicsQueue = proc._graphicsQueue;
		auto& presentQueue = window._presentQueue;
		auto& currentFrame = proc._currentFrame;

		glfwPollEvents();

		VkCommandBuffer& cmd = graphicsQueue._commandBuffers[currentFrame];

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);




		// Update view information, stored in a UBO
		updateUniformBuffer(proc, uboHdl);

		/* * * * * * * * * * * * * * * * * */

		RESET_COMMAND_BUFFER(cmd);
		BEGIN_COMMAND_BUFFER(cmd);

		BEGIN_RENDER_PASS(passContext, pipeline, framebuffer, cmd, FIXED);

		CALL_RENDER_PASS(DRAW_RECT, proc,
			READ(vertices, indices)
			INPUT(pipeline, window, cmd)
		);

		END_RENDER_PASS(cmd);

		END_COMMAND_BUFFER(cmd);

		/* * * * * * * * * * * * * * * * * */

		graphicsQueue.submit(currentFrame, cmd, window.getPresentFence(), window.getPresentQueue());

		window.display(imageFormat, { graphicsQueue.getSemaphore(currentFrame)});

		proc.nextFrame();
	}

	vertices.destroy(proc);
	indices.destroy(proc);

	glfwTerminate();
#ifndef NDEBUG
	_CrtDumpMemoryLeaks();
#endif // !NDEBUG

	return EXIT_SUCCESS;
}