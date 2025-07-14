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
	void renderTarget::render(ValProc& proc, const uint32_t& instanceCount /*DEFAULT = 1U*/)
	{
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();

		if (_indexCount>0) {
			vkCmdDrawIndexed(commandBuffer, (uint32_t)(_indexCount), instanceCount, 0, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDrawIndexed.html
		}
		else {
			vkCmdDraw(commandBuffer, _vertexCount, instanceCount, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDraw.html
		}
	}

	void renderTarget::rebindDescriptorSet(ValProc& proc, const GraphicsPipeline& pipeline) {
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelines[pipelineIdx]);
	}

	void renderTarget::updatePipeline(ValProc& proc, const GraphicsPipeline& pipeline)
	{
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelines[pipelineIdx]);
	}

	void renderTarget::updateViewport(ValProc& proc, const VkViewport& viewport)
	{
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	void renderTarget::updateViewport(ValProc& proc, const VkViewport& viewport, const uint16_t index)
	{
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	void renderTarget::updateViewports(ValProc& proc, const std::vector<VkViewport>&viewports)
	{
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetViewport(commandBuffer, 0, viewports.size(), viewports.data());
	}

	void renderTarget::updateViewports(ValProc& proc, const std::vector<VkViewport>& viewports, const uint16_t startIndex) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetViewport(commandBuffer, startIndex, (uint32_t)viewports.size(), viewports.data());
	}

	void renderTarget::updateScissor(ValProc& proc, const VkRect2D& scissor) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void renderTarget::updateScissor(ValProc& proc, const VkRect2D& scissor, const uint16_t index) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetScissor(commandBuffer, index, 1, &scissor);
	}

	void renderTarget::updateScissors(ValProc& proc, const std::vector<VkRect2D>& scissors) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetScissor(commandBuffer, 0, scissors.size(), scissors.data());
	}

	void renderTarget::updateScissors(ValProc& proc, const std::vector<VkRect2D>& scissors, const uint16_t startIndex) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetScissor(commandBuffer, startIndex, scissors.size(), scissors.data());
	}


	void renderTarget::updateLinewidth(ValProc& proc, const float lineWidth) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetLineWidth(commandBuffer, lineWidth);
	}

	void renderTarget::updateBlendConstants(ValProc& proc, const std::array<float, 4>& depthConstants) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetBlendConstants(commandBuffer, depthConstants.data());
	}

	void renderTarget::updateTopologyMode(ValProc& proc, const TOPOLOGY_MODE topologyMode) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetPrimitiveTopology(commandBuffer,(VkPrimitiveTopology)topologyMode);
	}

	void renderTarget::updateCullMode(ValProc& proc, const CULL_MODE cullMode) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetCullMode(commandBuffer, VkCullModeFlags(cullMode));
	}

	void renderTarget::updateDepthBias(ValProc& proc, const float depthBiasConstant, const float depthBiasClamp, const float depthBiasSlopeFactor) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdSetDepthBias(commandBuffer, depthBiasConstant, depthBiasClamp, depthBiasSlopeFactor);
	}

	void renderTarget::updateBuffers(ValProc& proc)
	{
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		// bind buffers
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateDescriptorSet(ValProc& proc, GraphicsPipeline& pipeline, DescriptorSheet& sheet, const uint32_t setIndex)
	{
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(proc),
			//0, 1, &proc._descriptorSets[pipelineIdx][proc._currentFrame], 0, nullptr);
			0, 1, &(sheet.getVkDescriptorSets()[setIndex]), 0, VK_NULL_HANDLE);
	}

	
	void renderTarget::updateIndexBuffer(ValProc& proc) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateVertexBuffers(ValProc& proc) {
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	/*****************************************************************************************************************************/
	/* BUFFER MANIPULATION*/

	void renderTarget::updateAndSetIndexBuffer(ValProc& proc, val::Buffer& buffer, const uint32_t& indexCount) {
		setIndexBuffer(buffer, indexCount);
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		};
	}

	void renderTarget::updateAndSetIndexBuffer(ValProc& proc, const VkBuffer& buffer, const uint32_t& indexCount) {
		setIndexBuffer(buffer, indexCount);
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		};
	}


	void renderTarget::updateAndSetVertexBuffer(ValProc& proc, const VkBuffer& buffer, const uint32_t& vertexCount) {
		setVertexBuffer(buffer, vertexCount);
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBuffer(ValProc& proc, val::Buffer& buffer, const uint32_t& vertexCount) {
		setVertexBuffer(buffer, vertexCount);
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBuffers(ValProc& proc, const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBuffers(ValProc& proc, const std::vector<val::Buffer*>& vertexBuffers, const uint32_t& vertexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
	}

	void renderTarget::updateAndSetVertexBufferAndIndexBuffer(ValProc& proc, val::Buffer& vertexBuffer, const uint32_t& vertexCount, val::Buffer& indexBuffer, const uint32_t& indexCount) {
		setVertexBuffer(vertexBuffer, vertexCount);
		setIndexBuffer(indexBuffer, indexCount);
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, _vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateAndSetVertexBufferAndIndexBuffer(ValProc& proc, const VkBuffer& vertexBuffer, const uint32_t& vertexCount, const VkBuffer& indexBuffer, const uint32_t& indexCount) {
		setVertexBuffer(vertexBuffer, vertexCount);
		setIndexBuffer(indexBuffer, indexCount);
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateAndSetVertexBuffersAndIndexBuffer(ValProc& proc, const std::vector<val::Buffer*>& vertexBuffers, const uint32_t& vertexCount, val::Buffer& indexBuffer, const uint32_t& indexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		setIndexBuffer(indexBuffer, indexCount);

		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void renderTarget::updateAndSetVertexBuffersAndIndexBuffer(ValProc& proc, const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount, const VkBuffer& indexBuffer, const uint32_t& indexCount) {
		setVertexBuffers(vertexBuffers, vertexCount);
		setIndexBuffer(indexBuffer, indexCount);

		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount > 0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}
	/*****************************************************************************************************************************/

	void renderTarget::update(ValProc& proc, const GraphicsPipeline& pipeline, const std::vector<VkViewport>& viewports)
	{
		const auto& pipelineIdx = pipeline.pipelineIdx;
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();

		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelines[pipelineIdx]);

		// bind buffers
		vkCmdBindVertexBuffers(commandBuffer, 0, (uint32_t)_vertexBuffers.size(), _vertexBuffers.data(), _vertexBufferOffsets.data());
		if (_indexCount>0) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}

		vkCmdSetViewport(commandBuffer, 0, (uint32_t)viewports.size(), viewports.data());
	}

	void renderTarget::begin(ValProc& proc)
	{
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();

		vkResetCommandBuffer(commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

	}

	void renderTarget::beginPass(ValProc& proc, VkRenderPass& renderPass, VkFramebuffer& frameBuffer)
	{
		VkCommandBuffer& commandBuffer = _queue->getCommandBuffer();

		// this would make more sense to have in the update function
		_renderPassBeginInfo.renderPass = renderPass;
		_renderPassBeginInfo.framebuffer = frameBuffer;

		vkCmdBeginRenderPass(commandBuffer, &_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void renderTarget::endPass(ValProc& proc) {
		vkCmdEndRenderPass(_queue->getCommandBuffer());
	}

	
	void renderTarget::submit(ValProc& proc,
		std::vector<VkSemaphore> waitSemaphores, VkFence fence /*DEFAULT=VK_NULL_HANDLE*/)
	{
		auto& graphicsQueue = _queue;
		const auto& currentFrame = proc._currentFrame;

		// END RECORDING
		if (vkEndCommandBuffer(_queue->getCommandBuffer()) != VK_SUCCESS) {
			throw std::runtime_error("FAILED TO RECORD COMMAND BUFFER");
		}

		// SUBMIT
		VkPipelineStageFlags* waitStages = (VkPipelineStageFlags*)calloc(waitSemaphores.size(), sizeof(VkPipelineStageFlags));
		for (size_t i = 0; i < waitSemaphores.size(); ++i) {
			waitStages[i] = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_queue->getCommandBuffer();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &_queue->getSemaphore();

#ifndef NDEBUG
		if (vkQueueSubmit(_queue->getVkQueue(), 1, &submitInfo, fence) != VK_SUCCESS) {
			dbg::printError("Failed to submit command buffer @ %p to graphics queue.", &graphicsQueue->getCommandBuffer());
			throw std::runtime_error("failed to submit draw command buffer!");
		}
#else
		vkQueueSubmit(_queue->getVkQueue(), 1, &submitInfo, fence);
#endif // !NDEBUG

	

		free(waitStages);
	}
}