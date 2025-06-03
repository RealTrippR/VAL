#define FRAMES_IN_FLIGHT 2u

#ifdef NDEBUG
const bool enableValidationLayers = false;

#else
const bool enableValidationLayers = true;
#endif //!NDEBUG

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/system/system_utils.hpp>
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


void setGraphicsPipelineInfo1(val::graphicsPipelineCreateInfo& pipeline)
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


void setGraphicsPipelineInfo2(val::graphicsPipelineCreateInfo& pipeline)
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
	colorAttach.setLoadOperation(CLEAR);
	colorAttach.setStoreOperation(STORE);
	colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	static subpass subpass(renderPassMngr, GRAPHICS);
	subpass.bindAttachment(&colorAttach);
}


void setRenderPass2(val::renderPassManager& renderPassMngr, VkFormat imgFormat) {
	using namespace val;
	static colorAttachment colorAttach;
	colorAttach.setImgFormat(imgFormat);
	colorAttach.setLoadOperation(CLEAR);
	colorAttach.setStoreOperation(STORE);
	colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	static subpass subpass(renderPassMngr, GRAPHICS);
	subpass.bindAttachment(&colorAttach);
}

int main()
{

	val::VAL_PROC proc;
	val::physicalDeviceRequirements deviceRequirements(val::DEVICE_TYPES::dedicated_GPU | val::DEVICE_TYPES::integrated_GPU);

	// Configure and create window
	val::windowProperties windowConfig;
	windowConfig.setProperty(val::WN_BOOL_PROPERTY::RESIZABLE, true);
	val::window window(windowConfig, 800, 800, "Render pass to image", &proc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// FML uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };


	//////////////////////////////////////////////////////////////////////////////
	///// CREATE PHYSICAL DEVICES              ///////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////


	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, &window);



	// UBO which stores view information
	val::UBO_Handle uboHdl(sizeof(uniformBufferObject));

	// load and configure frag shader
	// CONSIDER STORING IMAGE INFO INSIDE THE SHADER CLASS
	val::shader fragShaderImage("shaders-compiled/imageshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	val::sampler imgSampler(proc, val::combinedImage);
	imgSampler.setMaxAnisotropy(8.f);
	fragShaderImage.setImageSamplers({ { &imgSampler, 1 } });

	// load and configure vert shader
	val::shader vertShader("shaders-compiled/shadervert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription() });
	vertShader._UBO_Handles = { {&uboHdl,0} };



	// configure graphics pipeline
	val::graphicsPipelineCreateInfo pipeline1;
	pipeline1.shaders = { &vertShader,&fragShaderImage };

	setGraphicsPipelineInfo1(pipeline1);

	VkFormat imageFormat = val::findSupportedImageFormat(proc._physicalDevice, formatReqs);

	val::renderPassManager renderPass1(proc);
	setRenderPass(renderPass1, imageFormat);
	pipeline1.renderPass = &renderPass1;

	//////////////////////////////////////////////////////////////////////////////
	///// SECOND PIPELINE: RENDER TO SWAPCHAIN ///////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////

	val::graphicsPipelineCreateInfo pipeline2;

	val::UBO_Handle uboHdl2(sizeof(uniformBufferObject));

	val::shader vertShader2("shaders-compiled/shadervert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader2.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader2.setBindingDescriptions({ res::vertex::getBindingDescription() });

	vertShader2._UBO_Handles = {{ &uboHdl2,0 }};

	val::shader fragShaderColor("shaders-compiled/colorshaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
	pipeline2.shaders = { &vertShader2,&fragShaderColor };

	setGraphicsPipelineInfo2(pipeline2);


	//////////////////////////////////////////////////////////////////
	///// SECONDARY RENDER PASS //////////////////////////////////////
	//////////////////////////////////////////////////////////////////


	val::renderPassManager renderPass2(proc);
	setRenderPass2(renderPass2, imageFormat);
	pipeline2.renderPass = &renderPass2;


	//////////////////////////////////////////////////////////////////
	// CREATE MAIN_PROC, PIPELINES, & THEIR RESOURCES /////////////////
	//////////////////////////////////////////////////////////////////
	proc.create(&window, FRAMES_IN_FLIGHT, imageFormat, { &pipeline1, &pipeline2 });

	/*
	texture2d(VAL_PROC& proc, const uint16_t width, const uint16_t height, const VkFormat format,
		const VkImageUsageFlagBits usages, const VkImageLayout layout, const bufferSpace buffspace = GPU_ONLY, const uint8_t mipLevels = 0u) : _proc(proc)
	{
		create(width, height, space, format, usages, layout, buffspace, mipLevels);
	}
		
	*/

	val::texture2d renderTargetImg(proc, 800, 800, imageFormat, VkImageUsageFlagBits(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT  | VK_IMAGE_USAGE_SAMPLED_BIT), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	/*VkImage renderTargetImg;
	VkDeviceMemory renderTargImgMem;
	VkExtent2D secondaryRenderTargetExtent = { 800,800 };
	mainProc.createImage(secondaryRenderTargetExtent.width, secondaryRenderTargetExtent.height, imageFormat, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderTargetImg, renderTargImgMem);*/

	val::imageView renderTargImgView(proc, renderTargetImg, VK_IMAGE_ASPECT_COLOR_BIT);













	window.createSwapChainFrameBuffers(window._swapChainExtent, {}, 0u, pipeline1.getVkRenderPass(), proc._device);


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
	// create secondary vertex and index buffers
	// buffer wrapper for vertex Buffer
	val::gpu_vector<res::vertex> vertices2(proc, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		});





	VkExtent2D secondaryRenderTargetExtent = { 800,800 };


	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = pipeline1.getVkRenderPass();
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &renderTargImgView.getImageView();
	framebufferInfo.width = secondaryRenderTargetExtent.width;
	framebufferInfo.height = secondaryRenderTargetExtent.height;
	framebufferInfo.layers = 1;

	VkFramebuffer renderTargetFramebuffer;
	vkCreateFramebuffer(proc._device, &framebufferInfo, nullptr, &renderTargetFramebuffer);

	////////////////////////////////////////////////////////////
	imgSampler.bindImageView(renderTargImgView);

	////////////////// CREATE DESCRIPTOR SETS //////////////////
	proc.createDescriptorSets(&pipeline1);
	proc.createDescriptorSets(&pipeline2);
	////////////////////////////////////////////////////////////

	// configure the render target, setting vertex buffers, scissors, area, etc
	val::renderTarget renderTarget;
	renderTarget.setFormat(imageFormat);
	renderTarget.setRenderArea(window._swapChainExtent);
	renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
	renderTarget.setIndexBuffer(indices, indices.size());
	renderTarget.setVertexBuffer(vertices1, vertices1.size());

	// config viewport, covers the entire size of the window
	VkViewport viewport{ 0,0, window._swapChainExtent.width, window._swapChainExtent.height, 0.f, 1.f };


	renderTargetImg.transitionLayout(NULL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	while (!window.shouldClose()) {
		auto& graphicsQueue = proc._graphicsQueue;
		auto& presentQueue = window._presentQueue;
		auto& currentFrame = proc._currentFrame;

		VkCommandBuffer cmdBuffer = proc._graphicsQueue._commandBuffers[currentFrame];
		glfwPollEvents();
		updateUniformBuffer(proc, uboHdl);
		updateUniformBuffer(proc, uboHdl2);

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);

		renderTarget.beginPass(proc, pipeline1.getVkRenderPass(), framebuffer);

		renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });

		renderTarget.updatePipeline(proc, pipeline1);
		renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window.getSize() });
		renderTarget.updateViewport(proc, viewport);

		renderTarget.updateBuffers(proc);

		renderTarget.render(proc);

		renderTarget.endPass(proc);




		renderTarget.beginPass(proc, pipeline2.getVkRenderPass(), renderTargetFramebuffer);
		renderTarget.setIndexBuffer(indices.getVkBuffer(), indices.size());
		renderTarget.setVertexBuffer(vertices2, vertices2.size());
		renderTarget.setClearValues({ { 0.0f, 0.2f, 0.5f, 1.0f } });

		renderTarget.updatePipeline(proc, pipeline1);
		renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window.getSize() });
		renderTarget.updateViewport(proc, viewport);

		renderTarget.updateBuffers(proc);

		renderTarget.render(proc);

		renderTarget.endPass(proc);

	
		renderTargetImg.transitionLayout(NULL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		//proc.transitionImageLayout(renderTargetImg.getVkImage(), imageFormat,
		//	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL/*this should match the colorAttachment.finalLayout of the 2nd render pass*/, 
		//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		//	cmdBuffer);

		renderTarget.submit(proc, { presentQueue._semaphores[currentFrame] }, window.getPresentFence());
		window.display(imageFormat, { graphicsQueue._semaphores[currentFrame] });

		proc.nextFrame();
	}

	glfwTerminate();

	return EXIT_SUCCESS;
}