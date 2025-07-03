#define FRAMES_IN_FLIGHT 2u

#ifdef NDEBUG
const bool enableValidationLayers = false;

#else
const bool enableValidationLayers = true;
#endif //!NDEBUG

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/framebuffer.hpp>;
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

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

float time_sec = 0u;
void calculateTime()
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	static std::chrono::steady_clock::time_point lastTime;
	auto currentTime = std::chrono::high_resolution_clock::now();
	const auto dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime);
	printf("FPS: %f\n", 1 / dt.count());
	time_sec = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	lastTime = currentTime;
}

void updateViewMatrix(val::ValProc& proc, val::UBO_Handle& hdl)
{
	using namespace val;
	const VkExtent2D& extent = proc._windowVAL->getSize();

	ViewMatrix& ubo = *(ViewMatrix*)hdl.getData(proc);
	ubo.model = glm::rotate(glm::mat4(1.0f), time_sec * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;
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
	using namespace val;

	ValProc proc;
	PhysicalDeviceRequirements deviceRequirements(val::DEVICE_TYPES::dedicated_GPU | val::DEVICE_TYPES::integrated_GPU);

	// Configure and create window
	WindowProperties windowConfig;
	windowConfig.setProperty(val::WN_BOOL_PROPERTY::Resizable, true);
	Window window(windowConfig, 800, 800, "Render pass to image", proc);

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


	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, QUEUE_FLAGS::Graphics, &window);





	//////////////////////////////////////////////////////////////////////////////
	///// FIRST PIPELINE: RENDER IMAGE TO SWAPCHAIN///////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	// configure graphics pipeline
	GraphicsPipeline pipeline1;

	// UBO which stores view information
	UBO_Handle uboHdl(sizeof(ViewMatrix));




	Sampler imgSampler(proc, val::combinedImage);

	DescriptorSheet descriptorSheet1(
		{{0, uboHdl, SHADER_STAGE::Vertex},
		 {1, imgSampler, SHADER_STAGE::Fragment} },
		FRAMES_IN_FLIGHT
	);


	// load and configure frag shader
	Shader fragShaderImage("shaders-compiled/imageshader.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

	// load and configure vert shader
	Shader vertShader("shaders-compiled/shader.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription() });

	pipeline1.shaders = { &vertShader,&fragShaderImage };
	pipeline1.setDescriptorSheet(&descriptorSheet1);
	setGraphicsPipelineInfo1(pipeline1);

	VkFormat imageFormat = val::findSupportedImageFormat(proc._physicalDevice, formatReqs);

	val::renderPassManager renderPass1(proc);
	setRenderPass(renderPass1, imageFormat);
	pipeline1.setRenderPassManager(&renderPass1);

	//////////////////////////////////////////////////////////////////////////////
	///// SECOND PIPELINE: RENDER COLOR //////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	GraphicsPipeline pipeline2;

	UBO_Handle uboHdl2(sizeof(ViewMatrix));

	DescriptorSheet descriptorSheet2(
		{ {0, uboHdl2, SHADER_STAGE::Vertex} },
		FRAMES_IN_FLIGHT
	);

	val::Shader vertShader2("shaders-compiled/shader.vert.spv", SHADER_STAGE::Vertex);
	vertShader2.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader2.setBindingDescriptions({ res::vertex::getBindingDescription() });

	Shader fragShaderColor("shaders-compiled/colorshader.frag.spv", SHADER_STAGE::Fragment);
	pipeline2.shaders = { &vertShader2,&fragShaderColor };
	pipeline2.setDescriptorSheet(&descriptorSheet2);

	setGraphicsPipelineInfo2(pipeline2);


	//////////////////////////////////////////////////////////////////
	///// SECONDARY RENDER PASS //////////////////////////////////////
	//////////////////////////////////////////////////////////////////


	val::renderPassManager renderPass2(proc);
	setRenderPass2(renderPass2, imageFormat);
	pipeline2.setRenderPassManager(&renderPass2);


	//////////////////////////////////////////////////////////////////
	// CREATE MAIN_PROC, PIPELINES, & THEIR RESOURCES /////////////////
	//////////////////////////////////////////////////////////////////
	proc.create(window, FRAMES_IN_FLIGHT, imageFormat, { &pipeline1, &pipeline2 });

	val::Texture2D renderTargetImg(proc, 800, 800, imageFormat, 
		VkImageUsageFlagBits(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT  | VK_IMAGE_USAGE_SAMPLED_BIT), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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




	val::Framebuffer renderTargetFramebuffer(proc, 800, 800, pipeline2.getVkRenderPass(), renderTargImgView);
	
	////////////////////////////////////////////////////////////
	imgSampler.bindImageView(renderTargImgView);
	imgSampler.create();


	//////////////////////////////////////////////////////////////
		// create swapchain frame buffers and descriptor sets
	window.createSwapChainFrameBuffers({}, 0u, pipeline1.getVkRenderPass(), proc._device);

	pipeline1.allocateAndWriteDescriptorSets(proc);
	pipeline2.allocateAndWriteDescriptorSets(proc);

	//////////////////////////////////////////////////////////////

	Queue graphicsQueue(proc, QUEUE_FLAGS::Graphics | QUEUE_FLAGS::Compute);

	// configure the render target, setting vertex buffers, scissors, area, etc
	val::renderTarget renderTarget;
	renderTarget.setQueue(graphicsQueue);
	renderTarget.setFormat(imageFormat);
	renderTarget.setRenderArea(window.getSize());
	renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
	renderTarget.setIndexBuffer(indices, indices.size());
	renderTarget.setVertexBuffer(vertices1, vertices1.size());

	// config viewport, covers the entire size of the window
	VkViewport viewport{ 0,0, window.getWidth(), window.getHeight(), 0.f, 1.f};



	while (!window.shouldClose()) 
	{

		glfwPollEvents();
		calculateTime();
		updateViewMatrix(proc, uboHdl);
		updateViewMatrix(proc, uboHdl2);

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);

		renderTarget.begin(proc);

		renderTargetImg.transitionLayout(graphicsQueue, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		/*PIPELINE 2: COLOR PIPELINE*/

		renderTarget.beginPass(proc, pipeline2.getVkRenderPass(), renderTargetFramebuffer);
		renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
		renderTarget.updateDescriptorSet(proc, pipeline2, descriptorSheet2, proc.getCurrentFrame());
		renderTarget.updatePipeline(proc, pipeline2);
		renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window.getSize() });
		renderTarget.updateViewport(proc, viewport);

		renderTarget.updateBuffers(proc);

		renderTarget.render(proc);

		renderTarget.endPass(proc);



		renderTargetImg.transitionLayout(graphicsQueue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


		/*PIPELINE 1: IMAGE PIPELINE*/
		renderTarget.beginPass(proc, pipeline1.getVkRenderPass(), framebuffer);
		renderTarget.setIndexBuffer(indices.getVkBuffer(), indices.size());
		renderTarget.setVertexBuffer(vertices2, vertices2.size());
		renderTarget.setClearValues({ { 0.0f, 0.2f, 0.5f, 1.0f } });
		renderTarget.updateDescriptorSet(proc, pipeline1, descriptorSheet1, proc.getCurrentFrame());
		renderTarget.updatePipeline(proc, pipeline1);
		renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window.getSize() });
		renderTarget.updateViewport(proc, viewport);

		renderTarget.updateBuffers(proc);

		renderTarget.render(proc);

		renderTarget.endPass(proc);


		renderTarget.submit(proc, { window.getPresentSemaphore()}, window.getPresentFence());
		window.display(imageFormat, { graphicsQueue.getSemaphore() });

		proc.nextFrame();
	}

	glfwTerminate();

	return EXIT_SUCCESS;
}