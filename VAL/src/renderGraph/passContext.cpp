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

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/renderGraph/passContext.hpp>

namespace val {
	void PASS_CONTEXT::createWaitStages(ValProc& proc, const tiny_vector<GraphicsPipeline>& graphicsPipelines/*tiny_vector<ComputePipeline>& computePipelines*/)
	{
		bool hasVertexShaders = false;
		bool hasFragmentShaders = false;
		bool hasComputeShaders = false;
		bool hasTransferShaders = false;
		bool hasGeometryShaders = false;
		bool hasTessellationControlShaders = false;
		bool hasTessellationEvalShaders = false;
		bool hasColorBlendOutput = false;
		bool hasDepthBuffer = false;

		for (size_t i = 0; i < graphicsPipelines.size(); ++i) 
		{
			const GraphicsPipeline& pipeline = graphicsPipelines[i];

			if (pipeline.getDepthStencilState() != NULL) {
				hasDepthBuffer = true;
			}
			if (pipeline.getColorBlendState() != NULL) {
				hasColorBlendOutput = true;
			}
			for (const Shader* shader : pipeline.shaders) {
				if (shader->_shaderStageFlags == SHADER_STAGE::Vertex) 
				{
					hasVertexShaders = true;
				}

				if (shader->_shaderStageFlags == SHADER_STAGE::Fragment) 
				{
					hasFragmentShaders = true;
				}

				if (shader->_shaderStageFlags == SHADER_STAGE::Compute) 
				{
					hasComputeShaders = true;
				}

				if (shader->_shaderStageFlags == SHADER_STAGE::Geometry) 
				{
					hasGeometryShaders = true;
				}

				if (shader->_shaderStageFlags == SHADER_STAGE::TessellationControl)
				{
					hasTessellationControlShaders = true;
				}

				if (shader->_shaderStageFlags == SHADER_STAGE::TessellationEvaluation) 
				{
					hasTessellationEvalShaders = true;
				}
			}
		}


		//// create wait stages
		//for (uint32_t i = 0; i < waitStagesFlags.size(); ++i) 
		//{
			VkPipelineStageFlags& stageFlags = waitStages;
			stageFlags = 0x0;
			if (hasVertexShaders)  // VERTEX
			{
				stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			}
			if (hasFragmentShaders) // FRAGMENT
			{
				stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			if (hasComputeShaders)  // COMPUTE
			{
				stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
			}
			if (hasTransferShaders) // TRANSFER
			{
				stageFlags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			if (hasDepthBuffer)  // DEPTH BUFFER
			{
				stageFlags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			}
			if (hasColorBlendOutput) {
				stageFlags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			}
			if (hasGeometryShaders) // GEOMETRY SHADER
			{
				stageFlags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
			}
			if (hasTessellationControlShaders) // TESSELLATION CONTROL
			{
				stageFlags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
			}
			if (hasTessellationEvalShaders) // TESSELLATION EVALUATION
			{
				stageFlags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
			}
		//}
	}
}