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

#define FRAMES_IN_FLIGHT 2u

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif //!NDEBUG

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/renderTarget.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/graphics/shader.hpp>
#include <VAL/lib/ext/gpu_vector.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vertex.hpp"

// it is important that this is the last include
#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>

#include <Windows.h>

struct ViewMatrix {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

void updateViewMatrix(val::ValProc& proc, const val::Window& window, val::UBO_Handle& hdl) {
	using namespace val;
	VkExtent2D extent = window.getSize();
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
{	using namespace val;

	// state infos
	static rasterizerState rasterizer;
	rasterizer.setCullMode(CULL_MODE::BACK);
	rasterizer.setTopologyMode(TOPOLOGY_MODE::FILL);
	pipeline.setRasterizer(&rasterizer);

	// the color blend state affects how the output of the fragmennt shader is 
	// blended into the existing content of the the framebuffer.
	static ColorBlendStateAttachment colorBlendAttachment(false/*Disable blending*/);
	colorBlendAttachment.setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	pipeline.setSampleCount(VK_SAMPLE_COUNT_1_BIT);

	static ColorBlendState blendState;
	blendState.bindBlendAttachment(&colorBlendAttachment);
	pipeline.setColorBlendState(&blendState);

	pipeline.setDynamicStates({ DYNAMIC_STATE::Scissor, DYNAMIC_STATE::Viewport });
}
void setRenderPass(val::RenderPassManager& renderPassMngr, VkFormat imgFormat) 
{
	using namespace val;
	static ColorAttachment colorAttach;
	colorAttach.setImgFormat(imgFormat);
	colorAttach.setLoadOperation(RENDER_ATTACHMENT_OPERATION::Clear);
	colorAttach.setStoreOperation(RENDER_ATTACHMENT_OPERATION::Store);
	colorAttach.setFinalLayout(IMAGE_LAYOUT::PresentSrc);

	static Subpass subpass(renderPassMngr, PIPELINE_TYPE::Graphics);
	subpass.bindAttachment(&colorAttach);
}

int main()
{
	using namespace val;

	ValProc proc;
	PhysicalDeviceRequirements deviceRequirements(DEVICE_TYPES::dedicated_GPU | DEVICE_TYPES::integrated_GPU);
	deviceRequirements.addFeature(DEVICE_FEATURES::anisotropicFiltering);


	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, QUEUE_FLAGS::Graphics);

	Queue omniQueue;
	omniQueue.create(proc, QUEUE_FLAGS::Graphics | QUEUE_FLAGS::Transfer);

	// Configure and create window
	WindowProperties windowConfig;
	windowConfig.setProperty(WN_BOOL_PROPERTY::Resizable, true);
	Window window;
	window.prep(proc, windowConfig, 800, 800, "Image ____");
	window.setTitle("Image Test");
	window.setIcon("testImage.jpg");
	window.setWindowMode(WN_MODE::WindowedFullscreen);

	Cursor cursor("testImage41x26.jpg", 0, 0);
	window.setCursor(cursor);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// AFTER VAL_PROC INIT //////////////////////////////////////////////////
	//////////////////////////////////////////////////

	UBO_Handle viewUBO(sizeof(ViewMatrix));

	Sampler imgSampler(val::combinedImage);
	imgSampler.setMaxAnisotropy(8.f);
	DescriptorSheet descSheet(
		{
			{0, viewUBO, SHADER_STAGE::Vertex},
			{1, {DESC_TYPE::CombinedSampler, 1}, SHADER_STAGE::Fragment}
		},
		FRAMES_IN_FLIGHT
	);

	// load and configure vert shader
	val::Shader vertShader("shaders-compiled/image2D.vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	vertShader.setVertexAttributes(res::vertex::getAttributeDescriptions());
	vertShader.setBindingDescriptions({ res::vertex::getBindingDescription() });



	// load and configure frag shader
	// CONSIDER STORING IMAGE INFO INSIDE THE SHADER CLASS
	val::Shader fragShader("shaders-compiled/image2D.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");


	// config grahics pipeline
	GraphicsPipeline pipeline;
	pipeline.setShaders({ &fragShader, &vertShader });
	setGraphicsPipelineInfo(pipeline);
	val::RenderPassManager renderPassMngr(proc);
	setRenderPass(renderPassMngr, imageFormat);
	pipeline.setRenderPassManager(&renderPassMngr);
	pipeline.setDescriptorSheet(&descSheet);



	pushConstantHandle pushConstantTest(sizeof(bool), SHADER_STAGE::Fragment);
	pipeline.setPushConstants({ &pushConstantTest });


	proc.create(FRAMES_IN_FLIGHT, { &pipeline });



	/*window.create should be called only after proc.create has been called*/
	window.create(imageFormat, pipeline.getVkRenderPass());


	Texture2D img1(omniQueue, "testImage.jpg", TEXTURE_FORMAT_AUTO, IMAGE_USAGE::Sampled, IMAGE_LAYOUT::ShaderReadOnly);
	img1.discardPixels(); // no reason to save the pixels
	ImageView imgView1(proc, img1, VK_IMAGE_ASPECT_COLOR_BIT);

	Texture2D img2(omniQueue, "testImage2.png", TEXTURE_FORMAT_AUTO, IMAGE_USAGE::Sampled, IMAGE_LAYOUT::ShaderReadOnly);
	img2.discardPixels(); // no reason to save the pixels
	ImageView imgView2(proc, img2, VK_IMAGE_ASPECT_COLOR_BIT);



	gpu_vector<res::vertex> vertices(proc, BUFFER_USAGE::Vertex, {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		});


	gpu_vector<uint32_t> indices(proc, BUFFER_USAGE::Index,
		{ 0, 1, 2, 2, 3, 0 }
	);


	// if the img sampler is not standalone, it must be intitially binded with an image view
	// before the descriptor sets are created
	imgSampler.bindImageView(imgView1);
	imgSampler.create(proc);

	int timer = 0;
	bool imgNum = 0;



	descSheet.setSheetElement(1, { 1,imgSampler, SHADER_STAGE::Vertex });

	descSheet.allocateAndWriteSets(proc);

	//////////////////////////////////////////////////////////////

	Queue graphicsQueue(proc, QUEUE_FLAGS::Graphics);


	tiny_vector<VkCommandBuffer> cmdBuffers(proc.getFramesInFlight());
	allocateCommandBuffers(proc, cmdBuffers.data(), cmdBuffers.size());

	tiny_vector<VkCommandBuffer> winCmdBuffers(proc.getFramesInFlight());
	allocateCommandBuffers(proc, winCmdBuffers.data(), winCmdBuffers.size());

	tiny_vector<VkSemaphore> graphicsSemaphores(proc.getFramesInFlight());
	createSemaphores(proc, graphicsSemaphores.data(), graphicsSemaphores.size());
	tiny_vector<VkSemaphore> imgSemaphores(proc.getFramesInFlight());
	createSemaphores(proc, imgSemaphores.data(), imgSemaphores.size());

	// configure the render target, setting vertex buffers, scissors, area, etc
	val::renderTarget renderTarget(proc);
	renderTarget.setQueue(graphicsQueue);
	renderTarget.setFormat(imageFormat);
	renderTarget.setRenderArea(window.getSize());
	renderTarget.setClearValues({ { 0.0f, 0.001f, 0.005f, 1.0f } });
	// Note that simply setting the index and vertex buffers does not update them in current command buffer, they have to be binded using rt.updateBuffers() or rt.update()
	renderTarget.setIndexBuffer(indices, indices.size());
	renderTarget.setVertexBuffer(vertices, vertices.size());
	renderTarget.setCommandBuffers(cmdBuffers.data());


	Fence presentFence;
	presentFence.create(proc);

	PIPELINE_STAGE waitStages = PIPELINE_STAGE::ColorAttachmentOutput;

	while (!window.shouldClose()) {
		window.pollEvents();

		// Update view information, stored in a UBO
		updateViewMatrix(proc, window, viewUBO);

		uint8_t fidx = proc.getCurrentFrame();
		VkFramebuffer framebuffer = window.getSwapchainFramebuffer(imageFormat, imgSemaphores[fidx]);
		resetCommandBuffer(cmdBuffers[fidx]);
		beginCommandBuffer(cmdBuffers[fidx]);

		renderTarget.beginPass(pipeline.getVkRenderPass(), framebuffer);
		renderTarget.updateBuffers();
		renderTarget.updatePipeline(pipeline);
		renderTarget.updateViewport(VkViewport{ 0, 0, (float)window.getSize().width, (float)window.getSize().height, 0.f, 1.f }, 0);
		renderTarget.updateScissor(VkRect2D{ {0,0}, window.getSize() });
		renderTarget.updateDescriptorSet(pipeline, descSheet, proc.getCurrentFrame());

		bool usered = true;
		pushConstantTest.update(proc, &usered, pipeline, cmdBuffers[fidx]);

		renderTarget.render();
		renderTarget.endPass();

		endCommandBuffer(cmdBuffers[fidx]);

		graphicsQueue.submit(cmdBuffers[fidx], imgSemaphores[fidx], graphicsSemaphores[fidx], &waitStages, presentFence);

		presentFence.wait(proc);
		window.display(imageFormat, graphicsSemaphores[fidx]);
		presentFence.reset(proc);

		proc.nextFrame();

		timer++;
		if (timer > 2000) {
			timer = 0;
			imgNum = !imgNum;
			std::cout << "IMAGE SWAPPED!\n\n";
			if (imgNum) {
				imgSampler.bindImageView(imgView2);
				for (uint8_t i = 0; i < proc.getFramesInFlight(); ++i) {
					descSheet.updateDescriptor(proc, 1, i);
				}
				//fragShader.updateImageSampler(proc, pipeline, { imgSampler,1 });
			}
			else {
				imgSampler.bindImageView(imgView1);
				for (uint8_t i = 0; i < proc.getFramesInFlight(); ++i) {
					descSheet.updateDescriptor(proc, 1, i);
				}
				//fragShader.updateImageSampler(proc, pipeline, { imgSampler,1 });
			}
		}
	}

	vertices.destroy(proc);
	indices.destroy(proc);
	imgView1.destroy();
	imgView2.destroy();
	imgSampler.destroy(proc);
	presentFence.destroy(proc);


	graphicsQueue.waitIdle();
	destroySemaphores(proc, graphicsSemaphores.data(), graphicsSemaphores.size());
	destroySemaphores(proc, imgSemaphores.data(), imgSemaphores.size());
	freeCommandBuffers(proc, cmdBuffers.data(), cmdBuffers.size());

	freeCommandBuffers(proc, winCmdBuffers.data(), winCmdBuffers.size());
	glfwTerminate();

	return EXIT_SUCCESS;
}