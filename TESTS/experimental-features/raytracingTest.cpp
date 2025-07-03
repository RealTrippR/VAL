// https://nvpro-samples.github.io/vk_mini_path_tracer/index.html

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/system/raytracingPipeline.hpp>

#include <VAL/lib/meshes&vertices/mesh.hpp>
#include <VAL/lib/meshes&vertices/vertexTxtr.hpp>
#include <VAL/lib/meshes&vertices/vertexTxtr2D.hpp>

// it is important that this comes last
#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const bool enableValidationLayers = true;



#define FRAMES_IN_FLIGHT 2u

struct ViewMatrix {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct Light {
	alignas(16) glm::vec3 pos;
	alignas(16) glm::vec3 color;
};

float time_sec = 0u;
void calculateTime()
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	static std::chrono::steady_clock::time_point lastTime;
	auto currentTime = std::chrono::high_resolution_clock::now();
	const auto dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime);
	//printf("FPS: %f\n", 1 / dt.count());
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

void setRenderPass(val::renderPassManager& renderPassMngr, VkFormat imgFormat) {
	using namespace val;
	static colorAttachment colorAttach;
	colorAttach.setImgFormat(imgFormat);
	colorAttach.setLoadOperation(Clear);
	colorAttach.setStoreOperation(Store);
	colorAttach.setFinalLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	static Subpass subpass(renderPassMngr, Graphics);
	subpass.bindAttachment(&colorAttach);
}

void setImgGraphicsPipelineInfo(val::GraphicsPipeline& pipeline)
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

uint32_t alignUp(uint32_t val, uint32_t alignment) {
	return (val + alignment - 1) & ~(alignment - 1);
}


int main() 
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	{
		using namespace val;

		ValProc proc;

		PhysicalDeviceRequirements deviceRequirements(DEVICE_TYPES::dedicated_GPU | DEVICE_TYPES::integrated_GPU,
													  DEVICE_FEATURES::raytracing | DEVICE_FEATURES::accelerationStructures 
																				  | DEVICE_FEATURES::deviceBufferAddressing);
		deviceRequirements.deviceExtensions = { 
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,	
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_KHR_RAY_QUERY_EXTENSION_NAME,
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME
		};

		// Configure and create window
		WindowProperties windowConfig;
		windowConfig.setProperty(WN_BOOL_PROPERTY::Resizable, true);
		Window window(windowConfig, 800, 800, "Raytracing Test", proc);

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



		//////////////////////////////////////////////////////////////
		// load mesh
		Mesh<VertexTxtr2D, 1> screenMesh;


		FbxScene fbx("res/WoodenCube.fbx");
		screenMesh.importFromScene(proc, fbx, 0, true, BUFFER_USAGE::ShaderDeviceAddress | BUFFER_USAGE::AccelerationStructureBuildInput,
												 BUFFER_USAGE::ShaderDeviceAddress | BUFFER_USAGE::AccelerationStructureBuildInput);

		//Texture2D texture(proc, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, imageFormat);
		//fbx.importTexture2D(&texture, proc, 0, 0, FBX_MATERIAL_PROPERTY::diffuse);

		//////////////////////////////////////////////////////////////
		// create acceleration structures and raytracing pipeline
		AccelerationStructureGeometry cubeAccelGeometry;
		cubeAccelGeometry.setTriangleGeometryData(proc, VertexTxtr::getStride(), VertexTxtr::getPositionFormat(),
				screenMesh.vertices.getVkBuffer(), screenMesh.indices.getVkBuffer(), screenMesh.vertices.size());

		Queue rayQueue(proc, QUEUE_FLAGS::Graphics);

		AccelerationStructure BLAS;
		BLAS.setGeometries({ &cubeAccelGeometry });
		BLAS.buildAsBottomLevel(proc, rayQueue);

		AccelerationStructure TLAS;
		TLAS.buildAsTopLevel(proc, BLAS, rayQueue);


		val::Image rayOutputImg(proc, 800, 800, 
			VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);


		ImageView rayOutputImgView(proc, rayOutputImg, VK_IMAGE_ASPECT_COLOR_BIT, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

		DescriptorSheet rayDescSheet(
		   {{0, TLAS, SHADER_STAGE::Raygen},
			{1, rayOutputImgView, SHADER_STAGE::Raygen}},
			FRAMES_IN_FLIGHT
		);
		
		Shader raygenShader("shaders-compiled/basic.rgen.spv", SHADER_STAGE::Raygen);
		Shader anyHitShader("shaders-compiled/basic.rahit.spv", SHADER_STAGE::AnyHit);

		RaytracingPipeline rayPipeline;
		rayPipeline.setDescriptorSheet(&rayDescSheet);
		rayPipeline.setShaders({&raygenShader, &anyHitShader});













		//////////////////////////////////////////////////////////////
		// create image rendering pipeline

		// UBO which stores view information
		UBO_Handle viewUBO(sizeof(ViewMatrix));
		UBO_Handle lightUBO(sizeof(Light));


		Sampler imgSampler(proc, val::combinedImage);
		imgSampler.bindImageView(rayOutputImgView);
		imgSampler.create();

		DescriptorSheet descriptorSheetImgPipeline(
			{ {0, viewUBO, SHADER_STAGE::Vertex},
			{1, imgSampler, SHADER_STAGE::Fragment} },
			FRAMES_IN_FLIGHT
		);

		
		// load and configure frag shader
		Shader fragShader("shaders-compiled/image2d.frag.spv", SHADER_STAGE::Fragment);

		// load and configure vert shader
		Shader vertShader("shaders-compiled/image2d.vert.spv", SHADER_STAGE::Vertex);
		vertShader.setVertexAttributes(VertexTxtr2D::getInputAttributeDescriptions());
		vertShader.setBindingDescription(VertexTxtr2D::getBindingDescription());

		val::GraphicsPipeline imgRenderingPipeline;
		imgRenderingPipeline.setDescriptorSheet(&descriptorSheetImgPipeline);
		imgRenderingPipeline.setShaders({ &vertShader, &fragShader });

		setImgGraphicsPipelineInfo(imgRenderingPipeline);

		val::renderPassManager renderPassMngr(proc);
		setRenderPass(renderPassMngr, imageFormat);
		imgRenderingPipeline.setRenderPassManager(&renderPassMngr);

		//////////////////////////////////////////////////////////////
		proc.create(window, FRAMES_IN_FLIGHT, imageFormat, { &imgRenderingPipeline }, {}, {&rayPipeline });

		//////////////////////////////////////////////////////////////
		window.createSwapChainFrameBuffers(proc,imgRenderingPipeline.getVkRenderPass());


		rayPipeline.allocateAndWriteDescriptorSets(proc);
		imgRenderingPipeline.allocateAndWriteDescriptorSets(proc);

		//////////////////////////////////////////////////////////////
		Queue graphicsQueue(proc, QUEUE_FLAGS::Graphics);

		dbg::printWarning("val::buffer should be renamed to val::Buffer");

		// configure the render target, setting vertex buffers, scissors, area, etc
		val::renderTarget renderTarget;
		renderTarget.setQueue(graphicsQueue);
		renderTarget.setFormat(imageFormat);
		renderTarget.setRenderArea(window.getSize());
		renderTarget.setClearValues({ { 0.1f, 0.1f, 0.5f, 1.0f } });
		// Note that simply setting the index and vertex buffers does not update them in current command buffer, they have to be binded using rt.updateBuffers() or rt.update()
		renderTarget.setIndexBuffer(screenMesh.indices, screenMesh.indices.size());
		renderTarget.setVertexBuffer(screenMesh.vertices, screenMesh.vertices.size());
		// config viewport, covers the entire size of the window
		VkViewport viewport{ 0,0, window.getSize().width, window.getSize().height, 0.f, 1.f };






		VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProps = {};
		rtProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
		VkPhysicalDeviceProperties2 props2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
		props2.pNext = &rtProps;
		vkGetPhysicalDeviceProperties2(proc.getVkPhysicalDevice(), &props2);

		uint32_t handleSize = rtProps.shaderGroupHandleSize;
		uint32_t handleAlignment = rtProps.shaderGroupBaseAlignment;

		const uint32_t groupCount = 3; // raygen, miss, hit (or whatever you have)
		const uint32_t alignedHandleSize = alignUp(handleSize, handleAlignment);
		const VkDeviceSize sbtSize = groupCount * alignedHandleSize;

		std::vector<uint8_t> shaderHandleStorage(sbtSize);
		vkGetRayTracingShaderGroupHandlesKHR(proc.getVkLogicalDevice(), proc._raytracingPipelines[0], 0, groupCount, sbtSize, shaderHandleStorage.data());



		// SHADER BINDING TABLE
		VkBuffer sbtBuffer;
		VkDeviceMemory sbtMemory;
		// Create with VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
		// Memory should be VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | HOST_COHERENT
		
		proc.createBuffer(handleSize, VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT, sbtBuffer, sbtMemory );

		// Map and copy aligned shader handles
		uint8_t* mapped = ...; // map sbtMemory
		for (uint32_t i = 0; i < groupCount; ++i) {
			memcpy(mapped + i * alignedHandleSize, shaderHandleStorage.data() + i * handleSize, handleSize);
		}
		// unmap
		VkDeviceAddress sbtAddress = vkGetBufferDeviceAddress(proc.getVkLogicalDevice(), sbtBuffer);

		VkStridedDeviceAddressRegionKHR raygenRegion = {
			.deviceAddress = sbtAddress + 0 * alignedHandleSize,
			.stride = alignedHandleSize,
			.size = alignedHandleSize
		};

		VkStridedDeviceAddressRegionKHR missRegion = {
			.deviceAddress = sbtAddress + 1 * alignedHandleSize,
			.stride = alignedHandleSize,
			.size = alignedHandleSize
		};

		VkStridedDeviceAddressRegionKHR hitRegion = {
			.deviceAddress = sbtAddress + 2 * alignedHandleSize,
			.stride = alignedHandleSize,
			.size = alignedHandleSize
		};

		VkStridedDeviceAddressRegionKHR callableRegion = {}; // leave zeroed




	












		while (!window.shouldClose())
		{
			VkCommandBuffer cmd = graphicsQueue.getCommandBuffer();
			window.pollEvents();

			calculateTime();

			// Update view information, stored in a UBO
			updateViewMatrix(proc, viewUBO);

			VkFramebuffer framebuffer = window.beginDraw(imageFormat);
			renderTarget.begin(proc);


			renderTarget.beginPass(proc, imgRenderingPipeline.getVkRenderPass(), framebuffer);
			renderTarget.updateDescriptorSet(proc, imgRenderingPipeline, descriptorSheetImgPipeline, proc.getCurrentFrame());
			renderTarget.updateBuffers(proc);
			renderTarget.updatePipeline(proc, imgRenderingPipeline);
			renderTarget.updateViewport(proc, viewport, 0);
			renderTarget.updateScissor(proc, VkRect2D{ {0,0}, window.getSize() });
			renderTarget.render(proc);
			renderTarget.endPass(proc);



			vkCmdBindPipeline(cmd, rayPipeline.getBindPoint(), proc._raytracingPipelines[rayPipeline.pipelineIdx]);
			vkCmdBindDescriptorSets(cmd, rayPipeline.getBindPoint(), proc._raytracingPipelineLayouts[0],
				0, 1, &(rayPipeline.getDescriptorSheet()->getVkDescriptorSets()[proc.getCurrentFrame()]), 0, VK_NULL_HANDLE);


			// https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdTraceRaysKHR.html
			// https://www.gsn-lib.org/docs/nodes/raytracing.php
			vkCmdTraceRaysKHR(
				cmd,
				&raygenRegion,
				&missRegion,
				&hitRegion,
				&callableRegion,
				window.getWidth(),
				window.getHeight(),
				1.0
			);













			renderTarget.submit(proc, { window.getPresentQueue().getSemaphore() }, window.getPresentFence());
			window.display(imageFormat, { graphicsQueue.getSemaphore() });

			proc.nextFrame();
		}

		screenMesh.destroy(proc);
	}
	
	glfwTerminate();
}