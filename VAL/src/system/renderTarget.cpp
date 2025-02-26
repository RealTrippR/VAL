#include <VAL/lib/system/renderTarget.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>

namespace val {
	void renderTarget::render(VAL_PROC& proc, const std::vector<VkViewport>& viewports, VkCommandBuffer& commandBuffer, VkRenderPass& renderPass,
		VkFramebuffer& frameBuffer) {
		
		// this would make more sense to have in the update function
		_renderPassBeginInfo.renderPass = renderPass;
		_renderPassBeginInfo.framebuffer = frameBuffer;

		vkCmdSetViewport(commandBuffer, 0, viewports.size(), viewports.data());
		vkCmdBeginRenderPass(commandBuffer, &_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


		if (_indexBuffer) {
			vkCmdDrawIndexed(commandBuffer, (uint32_t)(_indexCount), 1, 0, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDrawIndexed.html
		}
		else {
			vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0); // https://registry.khronos.org/vulkan/specs/latest/man/html/vkCmdDraw.html
		}

		vkCmdEndRenderPass(commandBuffer);
	}

	void renderTarget::update(VAL_PROC& proc, VkCommandBuffer& commandBuffer, const uint16_t& pipelineIdx) {		

		vkCmdSetScissor(commandBuffer, 0, 1, &_scissor);

		// bind pipeline and respective descriptor sets
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._graphicsPipelines[pipelineIdx]);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, proc._pipelineLayouts[pipelineIdx],
			0, 1, &proc._descriptorSets[pipelineIdx][proc._currentFrame], 0, nullptr);

		VkDeviceSize offsets[] = { 0 };
		// bind buffers
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_vertexBuffer, offsets);
		if (_indexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}

	}
}