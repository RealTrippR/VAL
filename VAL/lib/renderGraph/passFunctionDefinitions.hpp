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

#ifndef VAL_PASS_FUNCTION_DEFINITIONS_HPP
#define VAL_PASS_FUNCTION__DEFINITIONS_HPP

namespace val {
	inline void setPipeline(graphicsPipelineCreateInfo& pipeline) {
		VAL_PROC& proc = (*pass_proc);
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelines[pipelineIdx]);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._pipelineLayouts[pipelineIdx],
			0, 1, &proc._descriptorSets[pipelineIdx][proc._currentFrame], 0, nullptr);
	}

	inline void setViewport(const VkViewport& viewport) {
		VAL_PROC& proc = (*pass_proc);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	inline void setScissor(const VkRect2D& scissor) {
		VAL_PROC& proc = (*pass_proc);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	inline void setScissor(const VkExtent2D& scissor) {
		VAL_PROC& proc = (*pass_proc);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		const VkRect2D _scissor = { 0, 0, scissor.width, scissor.height };
		vkCmdSetScissor(commandBuffer, 0, 1, &_scissor);
	}

	inline void setVertexBuffer(val::buffer& buffer) {
		VAL_PROC& proc = (*pass_proc);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(buffer.getVkBuffer()), NULL);
	}

	inline void setIndexBuffer(val::buffer& buffer) {
		VAL_PROC& proc = (*pass_proc);
		VkCommandBuffer& commandBuffer = proc._graphicsQueue._commandBuffers[proc._currentFrame];
		vkCmdBindIndexBuffer(commandBuffer, buffer.getVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}

#endif // !VAL_PASS_FUNCTION_DEFINITIONS_HPP