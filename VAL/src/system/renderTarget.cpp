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


#include <VAL/lib/system/renderTarget.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/system/system_utils.hpp>

namespace val {
	void renderTarget::render(const uint32_t& instanceCount /*DEFAULT = 1U*/)
	{
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];

		if (_indexCount > 0) {
			vkCmdDrawIndexed(commandBuffer, (uint32_t)(_indexCount), instanceCount, 0, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDrawIndexed.html
		}
		else {
			vkCmdDraw(commandBuffer, _vertexCount, instanceCount, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDraw.html
		}
	}

	void renderTarget::rebindDescriptorSet(const GraphicsPipeline& pipeline) {
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _proc._graphicsPipelines[pipelineIdx]);
	}

	void renderTarget::updatePipeline(const GraphicsPipeline& pipeline)
	{
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _proc._graphicsPipelines[pipelineIdx]);
	}

	void renderTarget::updateViewport(const VkViewport& viewport)
	{
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	void renderTarget::updateViewport(const VkViewport& viewport, const uint16_t index)
	{
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	void renderTarget::updateViewports(const std::vector<VkViewport>& viewports)
	{
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetViewport(commandBuffer, 0, (uint32_t)viewports.size(), viewports.data());
	}

	void renderTarget::updateViewports(const std::vector<VkViewport>& viewports, const uint16_t startIndex) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetViewport(commandBuffer, startIndex, (uint32_t)viewports.size(), viewports.data());
	}

	void renderTarget::updateScissor(const VkRect2D& scissor) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void renderTarget::updateScissor(const VkRect2D& scissor, const uint16_t index) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetScissor(commandBuffer, index, 1, &scissor);
	}

	void renderTarget::updateScissors(const std::vector<VkRect2D>& scissors) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetScissor(commandBuffer, 0, (uint32_t)scissors.size(), scissors.data());
	}

	void renderTarget::updateScissors(const std::vector<VkRect2D>& scissors, const uint16_t startIndex) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetScissor(commandBuffer, startIndex, (uint32_t)scissors.size(), scissors.data());
	}


	void renderTarget::updateLinewidth(const float lineWidth) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetLineWidth(commandBuffer, lineWidth);
	}

	void renderTarget::updateBlendConstants(const std::array<float, 4>& depthConstants) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetBlendConstants(commandBuffer, depthConstants.data());
	}

	void renderTarget::updateTopologyMode(const TOPOLOGY_MODE topologyMode) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetPrimitiveTopology(commandBuffer, (VkPrimitiveTopology)topologyMode);
	}

	void renderTarget::updateCullMode(const CULL_MODE cullMode) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetCullMode(commandBuffer, VkCullModeFlags(cullMode));
	}

	void renderTarget::updateDepthBias(const float depthBiasConstant, const float depthBiasClamp, const float depthBiasSlopeFactor) {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdSetDepthBias(commandBuffer, depthBiasConstant, depthBiasClamp, depthBiasSlopeFactor);
	}

	void renderTarget::updateBuffers()
	{
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		// bind buffers
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::setCommandBuffers(VkCommandBuffer* cmdBuffers)
	{
		_commandBuffers = cmdBuffers;
	}

	void renderTarget::updateDescriptorSet(GraphicsPipeline& pipeline, DescriptorSheet& sheet, const uint32_t setIndex)
	{
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(_proc),
			//0, 1, &proc._descriptorSets[pipelineIdx][proc._currentFrame], 0, nullptr);
			0, 1, &(sheet.getVkDescriptorSets()[setIndex]), 0, VK_NULL_HANDLE);

#ifndef NDEBUG
		if (sheet.isInitialized() == false) {
			dbg::printError("renderTarget::updateDescriptorSet: Attempted to use DescriptorSheet @ %p which has not yet been initialized.", sheet);
		}
#endif // !NDEBUG

	}


	void renderTarget::updateIndexBuffer() {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateVertexBuffers() {
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	/*****************************************************************************************************************************/
	/* BUFFER MANIPULATION*/

	void renderTarget::updateAndSetIndexBuffer(val::Buffer& buffer, const uint32_t& indexCount) {
		setIndexBuffer(buffer, indexCount);
		VkCommandBuffer commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		};
	}

	void renderTarget::updateAndSetIndexBuffer(const VkBuffer& buffer, const uint32_t& indexCount) {
		setIndexBuffer(buffer, indexCount);
		VkCommandBuffer commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		};
	}


	void renderTarget::updateAndSetVertexBuffer(const VkBuffer& buffer, const uint32_t& vertexCount) {
		setVertexBuffer(buffer, vertexCount);
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBuffer(val::Buffer& buffer, const uint32_t& vertexCount) {
		setVertexBuffer(buffer, vertexCount);
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBuffers(const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBuffers(const std::vector<val::Buffer*>& vertexBuffers, const uint32_t& vertexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBufferAndIndexBuffer(val::Buffer& vertexBuffer, const uint32_t& vertexCount, val::Buffer& indexBuffer, const uint32_t& indexCount) {
		setVertexBuffer(vertexBuffer, vertexCount);
		setIndexBuffer(indexBuffer, indexCount);
		VkCommandBuffer commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateAndSetVertexBufferAndIndexBuffer(const VkBuffer& vertexBuffer, const uint32_t& vertexCount, const VkBuffer& indexBuffer, const uint32_t& indexCount) {
		setVertexBuffer(vertexBuffer, vertexCount);
		setIndexBuffer(indexBuffer, indexCount);
		VkCommandBuffer commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateAndSetVertexBuffersAndIndexBuffer(const std::vector<val::Buffer*>& vertexBuffers, const uint32_t& vertexCount, val::Buffer& indexBuffer, const uint32_t& indexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		setIndexBuffer(indexBuffer, indexCount);

		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateAndSetVertexBuffersAndIndexBuffer(const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount, const VkBuffer& indexBuffer, const uint32_t& indexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		setIndexBuffer(indexBuffer, indexCount);

		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}
	/*****************************************************************************************************************************/


	void renderTarget::beginPass(VkRenderPass renderPass, VkFramebuffer& frameBuffer)
	{
		VkCommandBuffer& commandBuffer = _commandBuffers[_proc.getCurrentFrame()];

		// this would make more sense to have in the update function
		_renderPassBeginInfo.renderPass = renderPass;
		_renderPassBeginInfo.framebuffer = frameBuffer;

		vkCmdBeginRenderPass(commandBuffer, &_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void renderTarget::endPass() {
		vkCmdEndRenderPass(_commandBuffers[_proc.getCurrentFrame()]);
	}

}