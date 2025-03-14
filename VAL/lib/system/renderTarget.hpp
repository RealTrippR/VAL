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
		void render(VAL_PROC& proc, const std::vector<VkViewport>& viewports, const uint32_t& instanceCount = 1u);

		void rebindDescriptorSet(VAL_PROC& proc, const graphicsPipelineCreateInfo& pipeline);

		void update(VAL_PROC& proc, const graphicsPipelineCreateInfo& pipeline);

		void begin(VAL_PROC& proc, VkRenderPass& renderPass, VkFramebuffer& frameBuffer);

		void submit(VAL_PROC& proc, std::vector<VkSemaphore> waitSemaphores, VkFence fence = VK_NULL_HANDLE);

	public:
		void setVertexBuffers(const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount) {
			_vertexBuffers = vertexBuffers;
			_vertexCount = vertexCount;


			_vertexBufferOffsets.resize(vertexBuffers.size());
			memset(_vertexBufferOffsets.data(), 0u, sizeof(VkDeviceSize) * vertexBuffers.size());
		}

		const std::vector<VkBuffer>& getVertexBuffer() const {
			return _vertexBuffers;
		}

		void setIndexBuffer(const VkBuffer& indexBuffer, const uint32_t& indexCount) {
			_indexBuffer = indexBuffer;
			_indexCount = indexCount;
		}
		
		const VkBuffer& getIndexBuffer() const {
			return _indexBuffer;
		}

		const void setArea(glm::vec2 extent) {
			_renderPassBeginInfo.renderArea.extent = { (uint32_t)extent.x,(uint32_t)extent.y };
		}

		const void setArea(VkExtent2D extent) {
			_renderPassBeginInfo.renderArea.extent.height = extent.height;
			_renderPassBeginInfo.renderArea.extent.width = extent.width;
		}

		const glm::vec2& getArea() {
			return { _renderPassBeginInfo.renderArea.extent.width, _renderPassBeginInfo.renderArea.extent.height };
		}

		void setOffset(const glm::vec2& offset) {
			_renderPassBeginInfo.renderArea.offset.x = (uint32_t)offset.x;
			_renderPassBeginInfo.renderArea.offset.y = (uint32_t)offset.y;
		}

		void setOffset(const VkExtent2D& offset) {
			_renderPassBeginInfo.renderArea.offset.x = (uint32_t)offset.width;
			_renderPassBeginInfo.renderArea.offset.y = (uint32_t)offset.height;
		}

		void setOffset(const uint32_t& x, const uint32_t& y) {
			_renderPassBeginInfo.renderArea.offset.x = x;
			_renderPassBeginInfo.renderArea.offset.x = y;
		}

		const glm::vec2& getOffset() {
			return { _renderPassBeginInfo.renderArea.offset.x, _renderPassBeginInfo.renderArea.offset.y };
		}

		void setFormat(const VkFormat& format) {
			_imgFormat = format;
		}

		const VkFormat& getFormat() {
			return _imgFormat;
		}

		void setClearValues(std::vector<VkClearValue> clearValues) {
			_clearValues = clearValues;
			_renderPassBeginInfo.clearValueCount = _clearValues.size();
			_renderPassBeginInfo.pClearValues = _clearValues.data();
		}

		void setClearValues(VkClearValue* clearValues, uint32_t clearValueCount) {
			_clearValues.clear();
			for (uint32_t i = 0; i < clearValueCount; ++i) {
				_clearValues.push_back(clearValues[i]);
			}
			_renderPassBeginInfo.clearValueCount = _clearValues.size();
			_renderPassBeginInfo.pClearValues = _clearValues.data();
		}

		const std::vector<VkClearValue>& getClearValues() {
			return _clearValues;
		} 
		
		void setScissor(const VkRect2D& scissor) {
			_scissor = scissor;
		}

		void setScissorExtent(const VkExtent2D& scissor) {
			_scissor.extent.width = scissor.width;
			_scissor.extent.height = scissor.height;
		}

		void setScissorExent(const uint32_t x, const uint32_t y) {
			_scissor.extent = { x, y };

		}

		void setScissorExtentAndOffset(const glm::vec2& size, const glm::vec2& extent) {
			_scissor.offset = { 0, 0 };
			_scissor.extent = { (uint32_t)extent.x, (uint32_t)extent.y };
		}

		const VkRect2D& getScissor() {
			return _scissor;
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
		VkRect2D _scissor{};
	};
}
#endif // !VAL_RENDER_TARGET_HPP