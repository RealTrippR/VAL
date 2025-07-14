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
const std::vector<const char*> validationLayers = {};
#else
const bool enableValidationLayers = true;
const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif //!NDEBUG

#define FRAMES_IN_FLIGHT 2u

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/ext/gpu_vector.hpp>
#include <VAL/lib/meshes&vertices/mesh.hpp>
#include <VAL/lib/meshes&vertices/vertexTxtr.hpp>
#include <VAL/lib/descriptorSheets/descriptorSheet.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// it is important that this comes last
#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>

struct ViewMatrix {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct Light {
	alignas(16) glm::vec3 pos;
	alignas(16) glm::vec3 color;
};

static float time_sec = 0u;

void calculateTime() 
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	static std::chrono::steady_clock::time_point lastTime;
	auto currentTime = std::chrono::high_resolution_clock::now();
	const auto dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime);
	//printf("FPS: %f\n",1/dt.count());
	time_sec = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	lastTime = currentTime;
}

void updateViewMatrix(val::ValProc& proc, val::UBO_Handle& hdl)
{
	using namespace val;
	const VkExtent2D& extent = proc._windowVAL->getSize();

	const float ARM_DIST = .3f;
	ViewMatrix& ubo = *(ViewMatrix*)hdl.getData(proc);
	ubo.model = glm::rotate(glm::mat4(1.0f), time_sec * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(ARM_DIST), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;
}

void updateLight(val::ValProc& proc, val::UBO_Handle& lightUbo)
{
	Light* light = (Light*)lightUbo.getData(proc);
	light->color = { 1.f,1.f,1.f };
	light->pos = { sin(time_sec) * .2f,cos(time_sec) * .2f, .1f };
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

	pipeline.setDynamicStates({ DYNAMIC_STATE::SCISSOR, DYNAMIC_STATE::VIEWPORT });
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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	{
		using namespace val;

		ValProc proc;

		PhysicalDeviceRequirements deviceRequirements(DEVICE_TYPES::dedicated_GPU | DEVICE_TYPES::integrated_GPU);

		// Configure and create window
		WindowProperties windowConfig;
		windowConfig.setProperty(WN_BOOL_PROPERTY::Resizable, true);
		Window window(windowConfig, 800, 800, "Basic Lighting Test", proc);

		// creates Vulkan logical and physical devices
		// if a window is passed through, the windowSurface is also created
		proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, QUEUE_FLAGS::Graphics, &window);

		// VAL uses the image format requirements to pick the best image format
		// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
		val::ImageFormatRequirements formatReqs;
		formatReqs.acceptedFormats = { VK_FORMAT_R8G8B8A8_SRGB };
		formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
		formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
		formatReqs.acceptedColorSpaces = { window.getColorSpace() };
		VkFormat imageFormat = val::findSupportedImageFormat(proc._physicalDevice, formatReqs);


		UBO_Handle viewUBO(sizeof(ViewMatrix));
		UBO_Handle lightUBO(sizeof(Light));

		//////////////////////////////////////////////////////////////
		// load mesh, texture, and create img sampler
		Mesh<VertexTxtr, 1> mesh;

		FbxScene fbx("res/WoodenCube.fbx");
		//mesh.importFromScene(proc, fbx, 0);

		Texture2D texture(proc, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, imageFormat);
		fbx.importTexture2D(&texture, proc, 0, 0, FBX_MATERIAL_PROPERTY::diffuse);

		// this will happen automatically upon the call of it's destructor, 
		// but it's best practice to destroy once we're done using it
		fbx.destroy();

		PlyScene scene("res/bun_zipper.ply");
		mesh.importFromScene(proc, scene);

		ImageView imgView(proc, texture, VK_IMAGE_ASPECT_COLOR_BIT);

		Sampler imgSampler(proc, val::combinedImage);
		imgSampler.bindImageView(imgView);
		imgSampler.create();


		DescriptorSheet descriptorSheet(
		   {{0, viewUBO, SHADER_STAGE::Vertex},
			{1, lightUBO, SHADER_STAGE::Fragment},
			{2, imgSampler, SHADER_STAGE::Fragment}}, 
			FRAMES_IN_FLIGHT
		);

		// load and configure vert shader
		Shader fragShader("shaders-compiled/basicLightingTextured.frag.spv", SHADER_STAGE::Fragment);

		// load and configure vert shader
		Shader vertShader("shaders-compiled/basicLightingTextured.vert.spv", SHADER_STAGE::Vertex);
		vertShader.setVertexAttributes(VertexTxtr::getInputAttributeDescriptions());
		vertShader.setBindingDescription(VertexTxtr::getBindingDescription());



		//////////////////////////////////////////////////////////////

		val::GraphicsPipeline pipeline;
		pipeline.setDescriptorSheet(&descriptorSheet);
		pipeline.setShaders({ &vertShader, &fragShader });

		setGraphicsPipelineInfo(pipeline);

		val::RenderPassManager renderPassMngr(proc);
		setRenderPass(renderPassMngr, imageFormat);
		pipeline.setRenderPassManager(&renderPassMngr);

		//////////////////////////////////////////////////////////////
		proc.create(window, FRAMES_IN_FLIGHT, imageFormat, { &pipeline });

		window.createSwapChainFrameBuffers(proc, pipeline.getVkRenderPass());

		pipeline.allocateAndWriteDescriptorSets(proc);
		//////////////////////////////////////////////////////////////

		Queue graphicsQueue(proc, QUEUE_FLAGS::Graphics);


		// configure the render target, setting vertex buffers, scissors, area, etc
		val::renderTarget renderTarget;
		renderTarget.setQueue(graphicsQueue);
		renderTarget.setFormat(imageFormat);
		renderTarget.setRenderArea(window.getSize());
		renderTarget.setClearValues({ { 0.0f, 0.0f, 0.0f, 1.0f } });
		// Note that simply setting the index and vertex buffers does not update them in current command buffer, they have to be binded using rt.updateBuffers() or rt.update()
		renderTarget.setIndexBuffer(mesh.indices, mesh.indices.size());
		renderTarget.setVertexBuffer(mesh.vertices, mesh.vertices.size());
		// config viewport, covers the entire size of the window
		VkViewport viewport{ 0,0, window.getSize().width, window.getSize().height, 0.f, 1.f };


		while (!window.shouldClose())
		{
			window.pollEvents();

			calculateTime();

			// Update view information, stored in a UBO
			updateViewMatrix(proc, viewUBO);
			updateLight(proc, lightUBO);

			VkFramebuffer framebuffer = window.beginDraw(imageFormat);
			renderTarget.begin(proc);

			renderTarget.beginPass(proc, pipeline.getVkRenderPass(), framebuffer);
			renderTarget.updateDescriptorSet(proc, pipeline, descriptorSheet, proc.getCurrentFrame());
			renderTarget.updateBuffers(proc);
			renderTarget.updatePipeline(proc, pipeline);
			renderTarget.updateViewport(proc, viewport, 0);
			renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window.getSize() });
			renderTarget.render(proc);
			renderTarget.endPass(proc);

			renderTarget.submit(proc, { window.getPresentQueue().getSemaphore() }, window.getPresentFence());
			window.display(imageFormat, { graphicsQueue.getSemaphore() });

			proc.nextFrame();
		}

		mesh.destroy(proc);
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}