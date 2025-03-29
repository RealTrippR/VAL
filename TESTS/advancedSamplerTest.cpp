// test to validate standalone samplers (Sampling from an External Image),
// & multiple image views used on a single sampler
/*
A VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE descriptor only holds an image view,
and the sampler must be provided separately. This is useful if you want to use the same
texture with different samplers (e.g., different filtering or addressing modes).
*/

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

#include <Windows.h>

struct uniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

float ftimeSec;

void updateUniformBuffer(val::VAL_PROC& proc, val::UBO_Handle& hdl) {
	using namespace val;
	VkExtent2D& extent = proc._windowVAL->_swapChainExtent;
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	ftimeSec = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	static uniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), ftimeSec * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
	rasterizer.setCullMode(CULL_MODE_ENUMS::BACK);
	rasterizer.setPolygonMode(POLYGON_MODE_ENUM::FILL);
	pipeline.setRasterizer(&rasterizer);

	// the color blend state affects how the output of the fragmennt shader is 
	// blended into the existing content of the the framebuffer.
	static colorBlendStateAttachment colorBlendAttachment(false/*Disable blending*/);
	colorBlendAttachment.setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	pipeline.setSampleCount(VK_SAMPLE_COUNT_1_BIT);

	static colorBlendState blendState;
	blendState.bindBlendAttachment(&colorBlendAttachment);
	pipeline.setColorBlendState(&blendState);
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


int main() {
	using namespace val;
	VAL_PROC proc;

	physicalDeviceRequirements deviceRequirements(DEVICE_TYPES::dedicated_GPU | DEVICE_TYPES::integrated_GPU);
	deviceRequirements.deviceFeatures = DEVICE_FEATURES::anisotropicFiltering;
	deviceRequirements.deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	imageView imgView1(proc);
	imageView imgView2(proc);

	pushConstantHandle PC_ImgScissor(sizeof(float));
	/////////// consider moving this into the window class ///////////
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable
	//////////////////////////////////////////////////////////////////

	GLFWwindow* windowHDL_GLFW = glfwCreateWindow(800, 800, "Test", NULL, NULL);

	window window(windowHDL_GLFW, &proc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// VAL uses image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	UBO_Handle uboHdl(sizeof(uniformBufferObject));
	// load and configure vert shader
	shader vertShader("shaders-compiled/shader3Dimagevert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription() });
	vertShader.setUBOs({ {&uboHdl,0 } });



	// load and configure frag shader
	// CONSIDER STORING IMAGE INFO INSIDE THE SHADER CLASS
	shader fragShader("shaders-compiled/doubleImageShaderfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
	sampler imgSampler(proc, val::standalone);
	imgSampler.bindImageView(imgView1);
	imgSampler.setMaxAnisotropy(4.f);

	// https://kylehalladay.com/blog/tutorial/vulkan/2018/01/28/Textue-Arrays-Vulkan.html
	fragShader.setImageSamplers({ { &imgSampler, 1 } });
	fragShader.setTextures({ {{&imgView1, &imgView2 }, 2} });
	fragShader.setPushConstant(&PC_ImgScissor);

	// config grahics pipeline
	graphicsPipelineCreateInfo pipeline;
	pipeline.shaders = { &fragShader, &vertShader };

	setGraphicsPipelineInfo(pipeline);

	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, &window);

	VkFormat imageFormat = findSupportedImageFormat(proc._physicalDevice, formatReqs);
	
	renderPassManager renderPassManager(proc);
	setRenderPass(renderPassManager, imageFormat);
	pipeline.renderPass = &renderPassManager;

	proc.create(windowHDL_GLFW, &window, 2U, imageFormat, { &pipeline });

	window.createSwapChainFrameBuffers(window._swapChainExtent, {}, 0u,pipeline.getVkRenderPass(), proc._device);



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// AFTER VAL_PROC INIT //////////////////////////////////////////////////
	//////////////////////////////////////////////////

	image img1(proc, "testImage.jpg", imageFormat);
	imgView1.create(img1, VK_IMAGE_ASPECT_COLOR_BIT);

	image img2(proc, "testImage2.png", imageFormat);
	imgView2.create(img2, VK_IMAGE_ASPECT_COLOR_BIT);


	const std::vector<res::vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	// buffer wrapper for vertex Buffer
	buffer vertexBuffer(proc, vertices.size() * sizeof(res::vertex), val::CPU_GPU, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	memcpy(vertexBuffer.getDataMapped(), (void*)vertices.data(), vertices.size() * sizeof(res::vertex));

	std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0 };
	buffer indexBuffer(proc, indices.size() * sizeof(uint32_t), CPU_GPU, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	memcpy(indexBuffer.getDataMapped(), (void*)indices.data(), indices.size() * sizeof(uint32_t));

	bool imgNum = 0;

	VkClearValue clearValues[1];
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };

	// this needs to be called only after the frag shader's image view has been set
	proc.createDescriptorSets(&pipeline);

	//////////////////////////////////////////////////////////////
	renderTarget renderTarget;
	renderTarget.setFormat(imageFormat);
	renderTarget.setArea(window._swapChainExtent);
	renderTarget.setScissorExtent(window._swapChainExtent);
	renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
	renderTarget.setIndexBuffer(indexBuffer.getVkBuffer(), indices.size());
	renderTarget.setVertexBuffers({ vertexBuffer.getVkBuffer() }, vertices.size());

	// config viewport, covers the entire size of the window
	VkViewport viewport{ 0,0, window._swapChainExtent.width, window._swapChainExtent.height, 0.f, 1.f };

	float tmp = .7;

	while (!glfwWindowShouldClose(windowHDL_GLFW)) {
		glfwPollEvents();

		auto& graphicsQueue = proc._graphicsQueue;
		auto& presentQueue = window._presentQueue;
		auto& currentFrame = proc._currentFrame;
		VkCommandBuffer cmdBuffer = proc._graphicsQueue._commandBuffers[currentFrame];

		updateUniformBuffer(proc, uboHdl);
		float tmp = (sin(ftimeSec) + 1.f) / 2.f;

		VkFramebuffer framebuffer = window.beginDraw(imageFormat);
		renderTarget.beginPass(proc, pipeline.getVkRenderPass(), framebuffer);
		PC_ImgScissor.update(proc, &tmp, pipeline, fragShader, cmdBuffer);
		renderTarget.update(proc, pipeline, { viewport });
		renderTarget.render(proc);
		renderTarget.endPass(proc);

		renderTarget.submit(proc, { presentQueue._semaphores[currentFrame] }, presentQueue._fences[currentFrame]);
		window.display(imageFormat, { graphicsQueue._semaphores[currentFrame] });

		proc.nextFrame();

		// swap image views at regular intervals
		static uint16_t timer = 0;
		timer++;
		if (timer > 4000) {
			fragShader.updateTexture(proc, pipeline, { imgView1,2 }, 0);
			fragShader.updateTexture(proc, pipeline, { imgView2,2 }, 1);
			timer = 0;
		}
		else if (timer==2000) {
			fragShader.updateTexture(proc, pipeline, { imgView1,2 }, 1);
			fragShader.updateTexture(proc, pipeline, { imgView2,2 }, 0);
		}
	}

	imgView1.destroy();
	imgView2.destroy();
	proc.cleanup();

	return EXIT_SUCCESS;
}