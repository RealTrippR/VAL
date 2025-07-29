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


#ifndef FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP
#define FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP

#include <VAL/lib/system/pipelineBase.hpp>
#include <VAL/lib/system/renderPass.hpp>
#include <VAL/lib/system/system_utils.hpp>

#include <VAL/lib/pipelineStateInfos/stateInfos.hpp>

#include <VAL/lib/system/image.hpp>
#include <VAL/lib/system/imageView.hpp>

namespace val {
	class Shader; // forward declaration
	class Sampler; // forward declaration

	class GraphicsPipeline : public PipelineBase {
	public:		
		GraphicsPipeline() { setScissorCount(1); setViewportCount(1); _bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; }
	public:
		inline VkPipelineBindPoint getBindPoint() const {return _bindPoint;}

		void setRasterizer(val::rasterizerState* rasterizer);

		rasterizerState* getRasterizer() const;

		void setColorBlendState(val::ColorBlendState* colorState);

		ColorBlendState* getColorBlendState() const;

		void setDepthStencilState(val::depthStencilState* blendState);

		depthStencilState* getDepthStencilState() const;

		void setSampleCount(const VkSampleCountFlags& samples);

		const VkSampleCountFlags& getSampleCount() const;

		void setSampleShadingEnabled(const bool& enabled);

		const bool& getSampleShadingEnabled() const;

		VkPipelineMultisampleStateCreateInfo getVkPipelineMultisampleStateCreateInfo();

		void setTopology(const VkPrimitiveTopology& topolgy);

		VkPrimitiveTopology getTopology() const;

		void setScissors(const tiny_vector<VkRect2D> scissor);

		void setScissorCount(const uint32_t count);

		void setViewports(const tiny_vector<VkViewport> viewport);

		void setViewportCount(const uint32_t count);

		std::optional<tiny_vector<VkRect2D>> getScissors() const;

		uint32_t getScissorCount() const;

		std::optional<tiny_vector<VkViewport>> getViewports() const;

		uint32_t getViewportCount() const;

	
	public: 

		//void pushDescriptor_UNIFORM_BUFFER_DYNAMIC(VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, );
		//void pushDescriptor_STORAGE_BUFFER_DYNAMIC(VkCommandBuffer cmdBuffer, const uint16_t bindingIdx);
		//void pushDescriptor_INPUT_ATTACHMENT(VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const );
		//void pushDescriptor_MUTABLE_EXT(VAL_PROC& proc, VkCommandBuffer cmdBuffer, const uint16_t bindingIdx, const DESC_TYPE type, VkWriteDescriptorSet& write);

		inline VkPipeline& getVkPipeline(ValProc& proc);

		inline void setRenderPassManager(RenderPassManager* passMngr);

		inline VkRenderPass& getVkRenderPass();

	protected:
		friend ValProc;
	protected:

	
		RenderPassManager* renderPass = NULL;

		rasterizerState* _rasterizerState = NULL;
		ColorBlendState* _colorBlendState = NULL;

		depthStencilState* _depthStencilState = NULL;

		VkSampleCountFlags _sampleCountMSAA = VK_SAMPLE_COUNT_1_BIT;
		uint32_t subpassIndex = 0u;
		VkPrimitiveTopology _topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		bool _sampleShadingEnabled = false;
	};
}

#endif // !FML_GRAPHICS_PIPELINE_CREATE_INFO_HPP