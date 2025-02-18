// TESTS SHADER INPUTS:
// PUSH CONSTANTS
// UBOS
// STORAGE BUFFERS
// DESCRIPTOR BINDINGS
// SHADER I/O

// https://snorristurluson.github.io/TextRenderingWithFreetype/ - FreeType con Vulkan
// https://freetype.org/freetype2/docs/tutorial/step1.html / - Freetype beginner tutorial

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
#include "polygonVertex.hpp"

// it is important that this is the last include
#define STB_IMAGE_IMPLEMENTATION
#include <ExternalLibraries/stb_image.h>

#define DEG_TO_RAD(x)(x * double(0.01745329251994329576923690768488612713442871888541725456097191440171009114))


void updatePushConstants() {

}
void updateUBOs() {

}
void updateStorageBuffers() {	

}

void setGraphicsPipelineInfo(val::pipelineCreateInfo& info) {
	VkPipelineRasterizationStateCreateInfo& rasterizer = info.rasterizer;
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo& multisampling = info.multisampling;
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState& colorBlendAttachment = info.colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo& colorBlending = info.colorBlending;
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
}

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

void updatePolygon1() {

}

using res::polygonVertex;

// creates a triangle fan circle with a circumferences comprised of vertexCount vertices.
// Vertex count should be input as a multiple of 2.
void createCircle(glm::vec3 color, uint16_t vertexCount, uint16_t* vertexCountOut, polygonVertex** pVerticesOut, uint16_t** indicesListOut, uint16_t* indicesCountOut) {

	vertexCount++;

#ifndef NDEBUG //
	if (vertexCount < 3) {
		assert(0 && "VERTEX COUNT MUST BE GREATER THAN 3");
	}
#endif // !NDEBUG

	*pVerticesOut = (polygonVertex*)malloc(sizeof(polygonVertex) * vertexCount);
	*indicesCountOut = (vertexCount - 1) * 3;
	*indicesListOut = (uint16_t*)malloc(sizeof(uint16_t) * (*indicesCountOut));

	polygonVertex* centerVertex = &(*pVerticesOut)[0];
	(*centerVertex) = { {0,0}, color };

	for (uint16_t i = 1; i < vertexCount; i++) {
		double rad = DEG_TO_RAD((double(i-1)/(vertexCount - 1)) * 360);
		float x = cos(rad)*.5;
		float y = sin(rad)*.5;

		(*pVerticesOut)[i] = { {x,y}, color };
	}

	// index 0 is the center vertex
	for (int i = 0; i < vertexCount-1; i++) {
		uint16_t idx = i * 3;
		(*indicesListOut)[idx] = 0;
		(*indicesListOut)[idx+1] = i+1;
		(*indicesListOut)[idx+2] = (i+2) % vertexCount;
	}

	// last triangle edge case
	(*indicesListOut)[(vertexCount - 2) * 3 + 1] = vertexCount - 1; // Last vertex
	(*indicesListOut)[(vertexCount - 2) * 3 + 2] = 1;               // First vertex

	*vertexCountOut = vertexCount;
}

int main() {
	val::VAL_PROC mainProc;

	/////////// consider moving this into the window class ///////////
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // by saying NO_API we tell GLFW to not use OpenGL
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // non resizable
	//////////////////////////////////////////////////////////////////

	GLFWwindow* windowHDL_GLFW = glfwCreateWindow(800, 800, "Test", NULL, NULL);

	val::window window(windowHDL_GLFW, &mainProc, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);


	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// FML uses the image format requirements to pick the best image format
	// see: https://docs.vulkan.org/spec/latest/chapters/formats.html
	val::imageFormatRequirements formatReqs;
	formatReqs.acceptedFormats = { VK_FORMAT_B8G8R8A8_SRGB };
	formatReqs.tiling = VK_IMAGE_TILING_OPTIMAL;
	formatReqs.features = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;
	formatReqs.acceptedColorSpaces = { VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };



	// The shader class is poorly optimized and fucking retarded at the moment
	// load and configure vert shader
	val::shader polygonVertShader("shaders/polygonvert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
	polygonVertShader.setVertexAttributes(res::polygonVertex::getAttributeDescriptions().data(),
		res::polygonVertex::getAttributeDescriptions().size());
	polygonVertShader.setBindingDescription(res::polygonVertex::getBindingDescription());

	val::shader polygonFragShader("shaders/polygonfrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");

	val::pipelineCreateInfo pipelineInfo;

	pipelineInfo.shaders.push_back(&polygonVertShader);
	pipelineInfo.shaders.push_back(&polygonFragShader);

	setGraphicsPipelineInfo(pipelineInfo);

	mainProc.init(deviceExtensions, validationLayers, enableValidationLayers, windowHDL_GLFW, &window, &formatReqs, 2, &pipelineInfo);

	// vertices of circle
	polygonVertex* vertices = NULL;
	uint16_t vertexCount = 16;
	
	// indices of circle
	uint16_t* indices = NULL;
	uint16_t indicesCount;

	const glm::vec3 circleColor = { 0.f,1.f,.05f };
	// inits vertex list and index list
	createCircle(circleColor, vertexCount, &vertexCount, &vertices, &indices, &indicesCount);



	VkBuffer vertexBuffer = NULL;
	VkDeviceMemory vertexBufferMem = NULL;
	mainProc.createVertexBuffer(vertices, vertexCount, sizeof(polygonVertex), &vertexBuffer, &vertexBufferMem);


	

	VkBuffer indexBuffer = NULL;
	VkDeviceMemory indexBufferMem = NULL;
	mainProc.createIndexBuffer(indices, indicesCount, &indexBuffer, &indexBufferMem);


	mainProc.createDescriptorSets(&pipelineInfo);

	while (!glfwWindowShouldClose(windowHDL_GLFW)) {
		glfwPollEvents();

		updatePolygon1();
		mainProc.drawFrameExperimental(vertexBuffer, indexBuffer, indices, indicesCount);

		// Wait idle for the GPU to finish executing the command buffer
		vkDeviceWaitIdle(mainProc._device);
	}

	mainProc.cleanup(windowHDL_GLFW);

	free(vertices);
	free(indices);

	return EXIT_SUCCESS;
}