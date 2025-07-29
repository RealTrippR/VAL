#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/system/framebuffer.hpp>;

#include <VAL/lib/meshes&vertices/mesh.hpp>
#include <VAL/lib/meshes&vertices/vertexTxtr.hpp>
#include <VAL/lib/meshes&vertices/vertexTxtr2D.hpp>


#define GLM_FORCE_DEPTH_ZERO_TO_ONE

// it is important that this comes last
#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
const bool enableValidationLayers = true;

#define VAL_ENABLE_EXPIREMENTAL /*required for render graph*/

//#define VAL_RENDER_GRAPH_COMPILE_MODE
#include <VAL/lib/renderGraph/renderGraph.hpp>
#include <VAL/lib/renderGraph/passFunctionDefinitions.hpp>

/************************************************/
#include GRAPH_FILE(shadowMapping);
/************************************************/



const float DEPTH_BIAS = 0.001;
#define FRAMES_IN_FLIGHT 2u
#define DEPTH_FORMAT VK_FORMAT_D32_SFLOAT
#define NEAR_PLANE 0.5f
#define FAR_PLANE 7.0f

#define VIEW_DISTANCE 3.f

const VkFormat IMG_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

struct ViewMatrix {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec3 viewPos;
	float _pddd;
	alignas(16) float farPlane;
};

struct LightMatrix {
	alignas(16) glm::mat4 space;
	alignas(16) glm::vec3 lightPos;
};

struct Light {
	alignas(16) glm::vec3 position;
	float _pad1;              // Padding to match std140
	alignas(16) glm::vec3 color;
	float intensity;
};



static float time_sec = 0u;

void updateTime()
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	static std::chrono::steady_clock::time_point lastTime;
	auto currentTime = std::chrono::high_resolution_clock::now();
	const auto dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime);
	//printf("FPS: %f\n",1/dt.count());
	time_sec = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	lastTime = currentTime;
}

void updateLightMatrix(val::ValProc& proc, val::UBO_Handle& hdl, Light* light)
{
	using namespace val;

	const VkExtent2D& extent = proc._windowVAL->getSize();

	LightMatrix& ubo = *(LightMatrix*)hdl.getData(proc);

	// https://www.gamedev.net/forums/topic/710579-creating-shadows-with-vulkan/
	const float orthoSize = 3;
	glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, NEAR_PLANE, FAR_PLANE);
	lightProjection[1][1] *= -1;
	glm::vec3 lightPos = glm::normalize(light->position) * VIEW_DISTANCE;
	glm::mat4 lightView = glm::lookAt(lightPos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	ubo.space = lightSpaceMatrix;
	ubo.lightPos = lightPos;
}

void updateViewMatrix(val::ValProc& proc, val::UBO_Handle& hdl)
{
	using namespace val;
	const VkExtent2D& extent = proc._windowVAL->getSize();

	/*Z IS UP*/
	ViewMatrix& ubo = *(ViewMatrix*)hdl.getData(proc);
	ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 viewPos = glm::normalize(glm::vec3(3.0, 3.0, 3.0)) * VIEW_DISTANCE;
	ubo.view = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, NEAR_PLANE, FAR_PLANE);

	ubo.proj[1][1] *= -1;
	ubo.viewPos = viewPos;
	ubo.farPlane = FAR_PLANE;
}

void updateLight(val::ValProc& proc, val::UBO_Handle& hdl)
{
	using namespace val;
	using namespace glm;

	const float ARM_DISTANCE = 4.0;
	const float Z_POS = 4.0;
	/*Z IS UP*/
	Light& light = *(Light*)hdl.getData(proc);
	light.color = glm::vec3(1.0, 1.0, 1.0);
	light.intensity = 15.f;
	light.position = { sin(time_sec) * ARM_DISTANCE, cos(time_sec) * ARM_DISTANCE, Z_POS };
}


void setupLightingPipeline(val::ValProc& proc, val::GraphicsPipeline& pipeline)
{
	using namespace val;

	// state info
	static rasterizerState rasterizer;
	rasterizer.setCullMode(CULL_MODE::BACK);
	rasterizer.setTopologyMode(TOPOLOGY_MODE::FILL);
	rasterizer.setEnableDepthBias(true);
	rasterizer.setConstantDepthBias(0.0);
	rasterizer.setDepthBiasSlope(1.05f);
	pipeline.setRasterizer(&rasterizer);


	static depthStencilState depthState;
	depthState.enableDepthTesting(true);
	depthState.enableDepthWrite(true);
	depthState.setCompareOp(VK_COMPARE_OP_LESS);
	pipeline.setDepthStencilState(&depthState);


	pipeline.setDynamicStates({ DYNAMIC_STATE::Scissor, DYNAMIC_STATE::Viewport });
}



void setupMeshPipeline(val::ValProc& proc, val::GraphicsPipeline& pipeline)
{
	using namespace val;

	// state infos
	static rasterizerState rasterizer;
	rasterizer.setCullMode(CULL_MODE::BACK);
	rasterizer.setTopologyMode(TOPOLOGY_MODE::FILL);
	rasterizer.setEnableDepthBias(true);
	//rasterizer.setConstantDepthBias(DEPTH_BIAS + 1.0);
	//rasterizer.setDepthBiasSlope(1.25f);
	pipeline.setRasterizer(&rasterizer);

	// the color blend state affects how the output of the fragmennt shader is 
	// blended into the existing content of the the framebuffer.
	static ColorBlendStateAttachment colorBlendAttachment(false/*Disable blending*/);
	colorBlendAttachment.setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	/* A graphics pipeline can have as many color blend attachments as there are color attachments in the subpass it's associated with; no more, no less.*/
	static ColorBlendState blendState;
	blendState.bindBlendAttachment(&colorBlendAttachment);
	pipeline.setColorBlendState(&blendState);

	static depthStencilState depthState;
	depthState.enableDepthTesting(true);
	depthState.enableDepthWrite(true);
	pipeline.setDepthStencilState(&depthState);


	pipeline.setDynamicStates({ DYNAMIC_STATE::Scissor, DYNAMIC_STATE::Viewport });
}


void setupRenderPassLight(val::RenderPassManager& renderPassMngr)
{
	using namespace val;

	static DepthAttachment depthAttachment;
	depthAttachment.setImgFormat(DEPTH_FORMAT);
	depthAttachment.setLoadOperation(Clear);
	depthAttachment.setStoreOperation(Store);
	depthAttachment.setFinalLayout(IMAGE_LAYOUT::DepthStencilAttachment);

	static Subpass subpass(renderPassMngr, PIPELINE_TYPE::Graphics);
	subpass.bindAttachment(&depthAttachment);
}


void setupRenderPassMain(val::RenderPassManager& renderPassMngr)
{
	using namespace val;

	static ColorAttachment colorAttachment;
	colorAttachment.setImgFormat(IMG_FORMAT);
	colorAttachment.setLoadOperation(RENDER_ATTACHMENT_OPERATION::Clear);
	colorAttachment.setStoreOperation(RENDER_ATTACHMENT_OPERATION::Store);
	colorAttachment.setFinalLayout(IMAGE_LAYOUT::PresentSrc);

	static DepthAttachment depthAttachment;
	depthAttachment.setImgFormat(DEPTH_FORMAT);
	depthAttachment.setLoadOperation(Clear);
	depthAttachment.setStoreOperation(Store);
	depthAttachment.setFinalLayout(IMAGE_LAYOUT::DepthStencilAttachment);

	static Subpass subpass(renderPassMngr, PIPELINE_TYPE::Graphics);
	subpass.bindAttachment(&colorAttachment);
	subpass.bindAttachment(&depthAttachment);
}




int main()
{
	using namespace val;


	ValProc proc;

	PhysicalDeviceRequirements deviceRequirements(DEVICE_TYPES::dedicated_GPU | DEVICE_TYPES::integrated_GPU);


	// Configure and create window
	WindowProperties windowConfig;
	windowConfig.setProperty(WN_BOOL_PROPERTY::Resizable, true);
	Window window(windowConfig, 1200, 1200, "Shadow Mapping Test", proc);


	// creates Vulkan logical and physical devices
	// if a window is passed through, the windowSurface is also created
	proc.initDevices(deviceRequirements, validationLayers, enableValidationLayers, QUEUE_FLAGS::Graphics, &window);


	/*******************************************************************************************************************/

	// create view matrix
	UBO_Handle viewMatrix(sizeof(ViewMatrix));

	UBO_Handle lightMatrix(sizeof(LightMatrix));

	UBO_Handle light(sizeof(Light));

	// create texture samplerr
	Sampler meshTextureSampler(proc, SAMPLER_TYPE::combinedImage);

	// create mesh and respective texture

	FbxScene scene;
	scene.loadFromDisk("res/pillars.fbx");

	Mesh<VertexTxtr, 1> mesh;
	mesh.importFromScene(proc, scene, 0);

	scene.destroy();


	Texture2D meshTexture(proc, IMAGE_LAYOUT::ColorAttachment, IMG_FORMAT);
	meshTexture.createFromDisk("res/get.png", IMAGE_USAGE::Sampled, IMAGE_LAYOUT::ShaderReadOnly);

	ImageView meshTxtrView(proc, meshTexture, VK_IMAGE_ASPECT_COLOR_BIT);

	Sampler meshTxtrSampler(proc);
	meshTxtrSampler.bindImageView(meshTxtrView);
	meshTxtrSampler.create();

	/*******************************************************************************************************************/


	// create the RenderPasses, which holds state information about elements of the grahics pipeline
	val::RenderPassManager renderPassMain(proc);
	setupRenderPassMain(renderPassMain);

	val::RenderPassManager renderPassLight(proc);
	setupRenderPassLight(renderPassLight);


	/*******************************************************************************************************************/

	// create depth buffer
	Image lightDepthBufferImage(proc, window.getWidth(), window.getHeight(),
		DEPTH_FORMAT, IMAGE_LAYOUT::DepthStencilAttachment,
		IMAGE_USAGE::DepthStencilAttachment | IMAGE_USAGE::Sampled,
		IMAGE_TILING::Optimal, IMAGE_ASPECT::Depth);

	ImageView lightDepthBufferView(proc, lightDepthBufferImage, IMAGE_ASPECT::Depth);
	Sampler lightDepthBufferSampler(proc, lightDepthBufferView, SAMPLER_TYPE::combinedImage);
	lightDepthBufferSampler.setCompareMode(VK_COMPARE_OP_LESS); // Disable hardware compare mode
	lightDepthBufferSampler.setMipmapMode(VK_SAMPLER_MIPMAP_MODE_NEAREST); //  Depth maps don’t need mipmapping
	lightDepthBufferSampler.setMagnificationFilter(VK_FILTER_NEAREST); // or VK_FILTER_LINEAR for soft edges
	lightDepthBufferSampler.setMinificationFilter(VK_FILTER_NEAREST); // same as above
	lightDepthBufferSampler.useUnnormalizedCoordinates(false); // Use normalized [0,1] coordinates
	lightDepthBufferSampler.setAddressModeU(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER); // Clamp outside frustum
	lightDepthBufferSampler.setAddressModeV(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER); // Clamp outside frustum
	lightDepthBufferSampler.setAddressModeW(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER); // Clamp outside frustum
	lightDepthBufferSampler.setBorderColor(VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE); // Matches far plane depth = 1.0

	lightDepthBufferSampler.create();

	DepthBuffer depthBuffer(proc, window.getWidth(), window.getHeight(), DEPTH_FORMAT);
	/*******************************************************************************************************************/

	// LIGHTING PIPELINE BEGIN
	GraphicsPipeline lightingPipeline;
	{
		// create descriptor sheet
		static DescriptorSheet dsheet({
			{ 0, lightMatrix, SHADER_STAGE::Vertex },
			{ 1, viewMatrix, SHADER_STAGE::Vertex} }, // only used to read model space
			FRAMES_IN_FLIGHT
			);
		// create shaders
		static Shader vertShader("shaders-compiled/shadow_mapping_light.vert.spv", SHADER_STAGE::Vertex);
		vertShader.setBindingDescription(VertexTxtr::getBindingDescription());
		vertShader.setVertexAttributes(VertexTxtr::getInputAttributeDescriptions());

		static Shader fragShader("shaders-compiled/shadow_mapping_light.frag.spv", SHADER_STAGE::Fragment);

		lightingPipeline.setDescriptorSheet(&dsheet);
		lightingPipeline.setShaders({ &vertShader, &fragShader });

		setupLightingPipeline(proc, lightingPipeline);

		lightingPipeline.setRenderPassManager(&renderPassLight);
	}










	// LIGHTING PIPELINE END

	/*******************************************************************************************************************/

	// MESH PIPELINE BEGIN
	GraphicsPipeline meshPipeline;
	{
		// create descriptor sheet
		static DescriptorSheet dsheet({
			{0, lightDepthBufferSampler, SHADER_STAGE::Fragment, IMAGE_LAYOUT::DepthStencilReadOnly},
			{1, viewMatrix, SHADER_STAGE::Vertex | SHADER_STAGE::Fragment},
			{2, lightMatrix, SHADER_STAGE::Vertex | SHADER_STAGE::Fragment},
			{3, light, SHADER_STAGE::Fragment}
			}, FRAMES_IN_FLIGHT
		);
		// create shaders
		static Shader vertShader("shaders-compiled/shadow_mapping_main.vert.spv", SHADER_STAGE::Vertex);
		vertShader.setBindingDescription(VertexTxtr::getBindingDescription());
		vertShader.setVertexAttributes(VertexTxtr::getInputAttributeDescriptions());

		static Shader fragShader("shaders-compiled/shadow_mapping_main.frag.spv", SHADER_STAGE::Fragment);
		fragShader.addSpecializationConstant(0, (void*)&DEPTH_BIAS, sizeof(DEPTH_BIAS));

		meshPipeline.setDescriptorSheet(&dsheet);
		meshPipeline.setShaders({ &vertShader, &fragShader });

		setupMeshPipeline(proc, meshPipeline);

		meshPipeline.setRenderPassManager(&renderPassMain);
	}

	// END MESH PIPELINE

	/*******************************************************************************************************************/

	// create ValProc (create pipelines, create descriptor layouts, allocate descriptor sets, and more)
	proc.create(
		"pipeline_caches/phong.cache",
		window,
		FRAMES_IN_FLIGHT,
		IMG_FORMAT,
		{ &lightingPipeline, &meshPipeline }
	);


	/*******************************************************************************************************************/
	// create the swap chain frame buffers
	window.createSwapChainFrameBuffers(proc, meshPipeline.getVkRenderPass(), {depthBuffer.imgViews[0]});

	// populate descriptor sets
	lightingPipeline.allocateAndWriteDescriptorSets(proc);

	// populate descriptor sets
	meshPipeline.allocateAndWriteDescriptorSets(proc);


	/*******************************************************************************************************************/

	// create render graph

	RENDER_GRAPH renderGraph;
	renderGraph.loadFromFile("experimental-features/ShadowMapping/shadowMapping.rg.hpp");
	renderGraph.compile(proc.getFramesInFlight(), "experimental-features/ShadowMapping", "experimental-features/renderGraphDiagrams");

	
	PASS_CONTEXT lightPassContext = {
		proc,
		window.getSizeAsRect2D(),
		{{.depthStencil = {1.0f, 0}}},
		{ meshPipeline }
	};

	PASS_CONTEXT meshPassContext = {
		proc,
		window.getSizeAsRect2D(),
		{	{.color = { 0.0f, 0.04f, 0.2f, 1.0f } },
			{.depthStencil = {1.0f, 0}} },
		{ lightingPipeline }
	};

	Queue graphicsQueue(proc, QUEUE_FLAGS::Graphics);

	val::Framebuffer shadowMapframebuffer(proc, window.getWidth(), window.getHeight(), lightingPipeline.getVkRenderPass(), lightDepthBufferView);


	while (!window.shouldClose())
	{
		window.pollEvents();

		updateTime();
		updateViewMatrix(proc, viewMatrix);
		updateLight(proc, light);
		updateLightMatrix(proc, lightMatrix, (Light*)light.getData(proc));


		/* * * * * * * * * * * * * * * * * */

		graphicsQueue.reset();
		graphicsQueue.begin();

		CALL_RENDER_PASS(SHADOW, proc, lightPassContext,
			READ(mesh),
			WRITE(shadowMapframebuffer),
			INPUT(lightingPipeline, window, graphicsQueue)
		);

		VkFramebuffer swapchainFramebuffer = window.beginDraw(IMG_FORMAT);

		TRANSITION_IMAGE_LAYOUT(proc, lightDepthBufferImage, IMAGE_LAYOUT::DepthReadOnly,
			IMAGE_ASPECT::Depth, PIPELINE_STAGE::LateFragmentTests, PIPELINE_STAGE::FragmentShader,
			ACCESS_FLAGS::DepthStencilAttachmentWrite, ACCESS_FLAGS::ShaderRead,
			DEPENDENCY_FLAGS::None, graphicsQueue);

		CALL_RENDER_PASS(MAIN, proc, meshPassContext,
			READ(mesh),
			WRITE(swapchainFramebuffer),
			INPUT(meshPipeline, window, graphicsQueue)
		);

		TRANSITION_IMAGE_LAYOUT(proc, lightDepthBufferImage, IMAGE_LAYOUT::DepthStencilAttachment,
			IMAGE_ASPECT::Depth, PIPELINE_STAGE::TopOfPipe, PIPELINE_STAGE::BottomOfPipe,
			ACCESS_FLAGS::None, ACCESS_FLAGS::None,
			DEPENDENCY_FLAGS::None, graphicsQueue);


		graphicsQueue.end();

		/* * * * * * * * * * * * * * * * * */

		graphicsQueue.submit(window.getPresentQueue(), meshPassContext.getWaitStages(), window.getPresentFence());

		window.display(IMG_FORMAT, { graphicsQueue.getSemaphore() });

		proc.nextFrame();
	}

	mesh.destroy(proc);

	lightDepthBufferImage.destroy(proc);

	glfwTerminate();
}