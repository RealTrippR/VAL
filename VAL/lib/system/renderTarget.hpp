#ifndef VAL_RENDER_TARGET_HPP
#define VAL_RENDER_TARGET_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	class queueManager; // forward declaration
	class graphicsPipelineCreateInfo; // forward declaration
	class renderTarget {
	public:
		renderTarget() = default;
		renderTarget(renderTarget& other) = delete; // I do need to implement this functions eventually
		renderTarget(renderTarget&& other) = delete;

	public:
		void render(VAL_PROC& proc, const uint32_t& instanceCount = 1u);

		void rebindDescriptorSet(VAL_PROC& proc, const graphicsPipelineCreateInfo& pipeline);

		void updatePipeline(VAL_PROC& proc, const graphicsPipelineCreateInfo& pipeline);

		void updateViewport(VAL_PROC& proc, const VkViewport& viewport);

		void updateViewport(VAL_PROC& proc, const VkViewport& viewport, const uint16_t index);

		void updateViewports(VAL_PROC& proc, const std::vector<VkViewport>& viewports);

		void updateViewports(VAL_PROC& proc, const std::vector<VkViewport>& viewports, const uint16_t startIndex);

		void updateScissor(VAL_PROC& proc, const VkRect2D& scissor);

		void updateScissor(VAL_PROC& proc, const VkRect2D& scissor, const uint16_t index);

		void updateScissors(VAL_PROC& proc, const std::vector<VkRect2D>& scissors);

		void updateScissors(VAL_PROC& proc, const std::vector<VkRect2D>& scissors, const uint16_t startIndex);

		void updateLinewidth(VAL_PROC& proc, const float width);

		void updateBlendConstants(VAL_PROC& proc, const std::array<float, 4>& depthConstants);

		void updateTopologyMode(VAL_PROC& proc, const TOPOLOGY_MODE topologyMode);
		
		void updateCullMode(VAL_PROC&, const CULL_MODE cullMode);

		void updateDepthBias(VAL_PROC& proc, const float depthBiasConstant, const float depthBiasClamp, const float depthBiasSlopeFactor);

		void updateBuffers(VAL_PROC& proc);

		void update(VAL_PROC& proc, const graphicsPipelineCreateInfo& pipeline, const std::vector<VkViewport>& viewports);

		void beginPass(VAL_PROC& proc, VkRenderPass& renderPass, VkFramebuffer& frameBuffer);

		void endPass(VAL_PROC& proc); 

		void submit(VAL_PROC& proc, std::vector<VkSemaphore> waitSemaphores, VkFence fence = VK_NULL_HANDLE);

	public:
		inline void setVertexBuffers(const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount) {
			_vertexBuffers = vertexBuffers;
			_vertexCount = vertexCount;


			_vertexBufferOffsets.resize(vertexBuffers.size());
			memset(_vertexBufferOffsets.data(), 0u, sizeof(VkDeviceSize) * vertexBuffers.size());
		}

		inline const std::vector<VkBuffer>& getVertexBuffer() const {
			return _vertexBuffers;
		}

		inline void setIndexBuffer(const VkBuffer& indexBuffer, const uint32_t& indexCount) {
			_indexBuffer = indexBuffer;
			_indexCount = indexCount;
		}
		
		inline const VkBuffer& getIndexBuffer() const {
			return _indexBuffer;
		}

		inline const void setRenderArea(glm::vec2 extent) {
			_renderPassBeginInfo.renderArea.extent = { (uint32_t)extent.x,(uint32_t)extent.y };
		}

		inline const void setRenderArea(VkExtent2D extent) {
			_renderPassBeginInfo.renderArea.extent.height = extent.height;
			_renderPassBeginInfo.renderArea.extent.width = extent.width;
		}

		inline const glm::vec2& getRenderArea() {
			return { _renderPassBeginInfo.renderArea.extent.width, _renderPassBeginInfo.renderArea.extent.height };
		}

		inline void setRenderOffset(const glm::vec2& offset) {
			_renderPassBeginInfo.renderArea.offset.x = (uint32_t)offset.x;
			_renderPassBeginInfo.renderArea.offset.y = (uint32_t)offset.y;
		}

		inline void setRenderOffset(const VkExtent2D& offset) {
			_renderPassBeginInfo.renderArea.offset.x = (uint32_t)offset.width;
			_renderPassBeginInfo.renderArea.offset.y = (uint32_t)offset.height;
		}

		inline void setRenderOffset(const uint32_t& x, const uint32_t& y) {
			_renderPassBeginInfo.renderArea.offset.x = x;
			_renderPassBeginInfo.renderArea.offset.x = y;
		}

		inline const glm::vec2& getRenderOffset() {
			return { _renderPassBeginInfo.renderArea.offset.x, _renderPassBeginInfo.renderArea.offset.y };
		}

		void setFormat(const VkFormat& format) {
			_imgFormat = format;
		}

		inline const VkFormat& getFormat() {
			return _imgFormat;
		}

		inline void setClearValues(std::vector<VkClearValue> clearValues) {
			_clearValues = clearValues;
			_renderPassBeginInfo.clearValueCount = _clearValues.size();
			_renderPassBeginInfo.pClearValues = _clearValues.data();
		}

		inline void setClearValues(VkClearValue* clearValues, uint32_t clearValueCount) {
			_clearValues.clear();
			for (uint32_t i = 0; i < clearValueCount; ++i) {
				_clearValues.push_back(clearValues[i]);
			}
			_renderPassBeginInfo.clearValueCount = _clearValues.size();
			_renderPassBeginInfo.pClearValues = _clearValues.data();
		}

		inline const std::vector<VkClearValue>& getClearValues() {
			return _clearValues;
		} 


	protected:
		VkRenderPass _renderPass = VK_NULL_HANDLE;
		
		std::vector<VkClearValue> _clearValues;

		VkFormat _imgFormat;
		std::vector<VkBuffer> _vertexBuffers;
		std::vector<VkDeviceSize> _vertexBufferOffsets;
		uint32_t _vertexCount = 0;
		VkBuffer _indexBuffer;
		uint32_t _indexCount = 0;
		VkRenderPassBeginInfo _renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, NULL, VK_NULL_HANDLE, VK_NULL_HANDLE, {0u,0u}, 0u, VK_NULL_HANDLE};
	};
}
#endif // !VAL_RENDER_TARGET_HPP