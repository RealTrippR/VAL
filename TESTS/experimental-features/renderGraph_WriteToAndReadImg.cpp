#define FRAMES_IN_FLIGHT 2u

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
#define _CRTDBG_MAP_ALLOC
const bool enableValidationLayers = true;
#endif //!NDEBUG

#define VAL_ENABLE_EXPIREMENTAL // for render graphs and gpu_vector

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/framebuffer.hpp>;
#include <VAL/lib/ext/gpu_vector.hpp>
#include <VAL/lib/ext/VkPipelineStagesToString.hpp>

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
#include GRAPH_FILE(renderGraph_ReadAndWriteImg);
/************************************************/


struct ViewMatrix {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

void updateViewMatrix(val::ValProc& proc, val::UBO_Handle& hdl)
{
	using namespace val;
	const VkExtent2D extent = proc._windowVAL->getSize();
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


void setGraphicsPipelineInfo1(val::GraphicsPipeline& pipeline)
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

	/* A graphics pipeline can have as many color blend attachments as there are color attachments in the subpass it's associated with; no more, no less.*/
	static colorBlendState blendState;
	blendState.bindBlendAttachment(&colorBlendAttachment);
	pipeline.setColorBlendState(&blendState);

	pipeline.setDynamicStates({ DYNAMIC_STATE::SCISSOR, DYNAMIC_STATE::VIEWPORT });
}


void setGraphicsPipelineInfo2(val::GraphicsPipeline& pipeline)
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
	colorAttach.setLoadOperation(RENDER_ATTACHMENT_OPERATION::Clear);
	colorAttach.setStoreOperation(RENDER_ATTACHMENT_OPERATION::Store);
	colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	static Subpass subpass(renderPassMngr, PIPELINE_TYPE::Graphics);
	subpass.bindAttachment(&colorAttach);
}


void setRenderPass2(val::renderPassManager& renderPassMngr, VkFormat imgFormat) {
	using namespace val;
	static colorAttachment colorAttach;
	colorAttach.setImgFormat(imgFormat);
	colorAttach.setLoadOperation(RENDER_ATTACHMENT_OPERATION::Clear);
	colorAttach.setStoreOperation(RENDER_ATTACHMENT_OPERATION::Store);
	colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	static Subpass subpass(renderPassMngr, PIPELINE_TYPE::Graphics);
	subpass.bindAttachment(&colorAttach);
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	using namespace val;
	ValProc proc;
	PhysicalDeviceRequirements deviceRequirements(val::DEVICE_TYPES::dedicated_GPU | val::DEVICE_TYPES::integrated_GPU);
	

	// Configure and create window
	WindowProperties windowConfig;
	windowConfig.setProperty(WN_BOOL_PROPERTY::Resizable, true);
	Window window(windowConfig, 800, 800, "Render Pass To Image", proc);

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// FML uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	ImageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { window.getColorSpace() };


	//////////////////////////////////////////////////////////////////////////////
	///// CREATE PHYSICAL DEVICES              ///////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, QUEUE_FLAGS::Graphics, &window);



	//////////////////////////////////////////////////////////////////////////////
	///// FIRST PIPELINE: RENDER IMAGE TO SWAPCHAIN///////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	GraphicsPipeline imgPipeline;

	// create UBO which stores view information
	UBO_Handle uboHdl1(sizeof(ViewMatrix));

	// load and configure frag shader
	Shader fragShaderImage("shaders-compiled/imageshaderfrag.spv", SHADER_STAGE::Fragment, "main");

	sampler imgSampler(proc, val::combinedImage);
	imgSampler.setMaxAnisotropy(8.f);
	fragShaderImage.setImageSamplers({ { &imgSampler, 1 } });

	// load and configure vert shader
	Shader vertShader("shaders-compiled/shadervert.spv", SHADER_STAGE::Vertex, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription() });
	vertShader._UBO_Handles = { {&uboHdl1,0} };

	imgPipeline.shaders = { &vertShader,&fragShaderImage };

	setGraphicsPipelineInfo1(imgPipeline);

	VkFormat imageFormat = val::findSupportedImageFormat(proc._physicalDevice, formatReqs);

	val::renderPassManager renderPass1(proc);
	setRenderPass(renderPass1, imageFormat);
	imgPipeline.setRenderPassManager(&renderPass1);

	//////////////////////////////////////////////////////////////////////////////
	///// SECOND PIPELINE: RENDER COLOR //////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	GraphicsPipeline colorPipeline;

	UBO_Handle uboHdl2(sizeof(ViewMatrix));

	val::Shader vertShader2("shaders-compiled/shadervert.spv", SHADER_STAGE::Vertex, "main");
	vertShader2.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader2.setBindingDescriptions({ res::vertex::getBindingDescription() });

	vertShader2._UBO_Handles = { { &uboHdl2,0 } };

	Shader fragShaderColor("shaders-compiled/colorshaderfrag.spv", SHADER_STAGE::Fragment, "main");
	colorPipeline.shaders = { &vertShader2,&fragShaderColor };

	setGraphicsPipelineInfo2(colorPipeline);


	//////////////////////////////////////////////////////////////////
	///// SECONDARY RENDER PASS //////////////////////////////////////
	//////////////////////////////////////////////////////////////////


	val::renderPassManager renderPass2(proc);
	setRenderPass2(renderPass2, imageFormat);
	colorPipeline.setRenderPassManager(&renderPass2);


	//////////////////////////////////////////////////////////////////
	// CREATE MAIN_PROC, PIPELINES, & THEIR RESOURCES /////////////////
	//////////////////////////////////////////////////////////////////
	proc.create(window, FRAMES_IN_FLIGHT, imageFormat, { &imgPipeline, &colorPipeline });
	window.createSwapChainFrameBuffers({}, 0u, imgPipeline.getVkRenderPass(), proc._device);

	val::Texture2D renderTargetImg(proc, 800, 800, imageFormat,
		VkImageUsageFlagBits(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	val::ImageView renderTargImgView(proc, renderTargetImg, VK_IMAGE_ASPECT_COLOR_BIT);




	val::gpu_vector<res::vertex> vertices1(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		});

	val::gpu_vector<uint32_t> indices(proc, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		{ 0, 1, 2, 2, 3, 0 }
	);

	//////////////////////////////////////////////////////////////////
	// create secondary vertex buffer
	val::gpu_vector<res::vertex> vertices2(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	{
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	});



	val::Framebuffer renderTargetFramebuffer(proc, 800, 800, colorPipeline.getVkRenderPass(), renderTargImgView);

	////////////////////////////////////////////////////////////
	imgSampler.bindImageView(renderTargImgView);

	////////////////// CREATE DESCRIPTOR SETS //////////////////
	proc.createDescriptorSets(&imgPipeline);
	proc.createDescriptorSets(&colorPipeline);

	///////////////////  CREATE RENDER GRAPH ///////////////////
	RENDER_GRAPH renderGraph;

	renderGraph.loadFromFile("experimental-features/renderGraph_ReadAndWriteImg.rg.hpp");


	PASS_CONTEXT colorPassContext = {
		//"COLOR_PIPELINE",
		proc,
		window.getSizeAsRect2D(),
		{ { 0.0f, 0.04f, 0.2f, 1.0f } }, /*clear values*/
		{ colorPipeline }
	};


	PASS_CONTEXT imagePassContext = {
		//"IMAGE_PIPELINE",
		proc,
		window.getSizeAsRect2D(),
		{ { 0.0f, 0.0f, 0.0f, 1.0f } }, /*clear values*/
		{ imgPipeline }
	};

	renderGraph.compile(proc.getFramesInFlight(), filepath("experimental-features"),
		//{ colorPassContext, imagePassContext }
		"experimental-features/renderGraphDiagrams"
	);

	//////////////////  CREATE GRAPHICS QUEUE //////////////////
	Queue graphicsQueue(proc, QUEUE_FLAGS::Graphics | QUEUE_FLAGS::Compute);


	std::cout << "Color pass wait stages: " << val::to_string(colorPassContext.getWaitStages()) << "\n\n";
	std::cout << "Image pass wait stages: " << val::to_string(imagePassContext.getWaitStages()) << "\n\n";

	while (!window.shouldClose()) 
	{
		window.pollEvents();

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);
		
		// Update view information, stored in a UBO
		updateViewMatrix(proc, uboHdl1);
		updateViewMatrix(proc, uboHdl2);

		/* * * * * * * * * * * * * * * * * */

		graphicsQueue.reset();
		graphicsQueue.begin();


		CALL_RENDER_PASS(COLOR, proc, colorPassContext, 
			vertices1, indices, 
			renderTargetFramebuffer, renderTargetImg, 
			colorPipeline, window, graphicsQueue);

		CALL_RENDER_PASS(IMAGE, proc, imagePassContext,
			vertices2, indices, renderTargetImg, 
			framebuffer, 
			imgPipeline, window, graphicsQueue);




		graphicsQueue.end();

		/* * * * * * * * * * * * * * * * * */

		graphicsQueue.submit(window.getPresentQueue(),
			colorPassContext.getWaitStages() | imagePassContext.getWaitStages(), window.getPresentFence());

		window.display(imageFormat, { graphicsQueue.getSemaphore() });

		proc.nextFrame();
	}

	vertices1.destroy(proc);
	vertices2.destroy(proc);
	indices.destroy(proc);
	renderTargetImg.destroy();
	renderTargImgView.destroy();
	window.destroy();
	
	glfwTerminate();


	_CrtDumpMemoryLeaks();

	return EXIT_SUCCESS;
}