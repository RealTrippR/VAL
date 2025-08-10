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

#ifndef VAL_RENDER_TARGET_HPP
#define VAL_RENDER_TARGET_HPP

#include <VAL/lib/system/system_utils.hpp>

#include <VAL/lib/system/buffer.hpp>

namespace val {
	class QueueManager; // forward declaration
	class GraphicsPipeline; // forward declaration
	class renderTarget {
	public:
		renderTarget() = default;
		renderTarget(renderTarget& other) = delete; // I do need to implement this functions eventually
		renderTarget(renderTarget&& other) = delete;

	public:
		void render(ValProc& proc, const uint32_t& instanceCount = 1u);

		void rebindDescriptorSet(ValProc& proc, const GraphicsPipeline& pipeline);

		void updatePipeline(ValProc& proc, const GraphicsPipeline& pipeline);

		void updateViewport(ValProc& proc, const VkViewport& viewport);

		void updateViewport(ValProc& proc, const VkViewport& viewport, const uint16_t index);

		void updateViewports(ValProc& proc, const std::vector<VkViewport>& viewports);

		void updateViewports(ValProc& proc, const std::vector<VkViewport>& viewports, const uint16_t startIndex);

		void updateScissor(ValProc& proc, const VkRect2D& scissor);

		void updateScissor(ValProc& proc, const VkRect2D& scissor, const uint16_t index);

		void updateScissors(ValProc& proc, const std::vector<VkRect2D>& scissors);

		void updateScissors(ValProc& proc, const std::vector<VkRect2D>& scissors, const uint16_t startIndex);

		void updateLinewidth(ValProc& proc, const float width);

		void updateBlendConstants(ValProc& proc, const std::array<float, 4>& depthConstants);

		void updateTopologyMode(ValProc& proc, const TOPOLOGY_MODE topologyMode);

		void updateCullMode(ValProc&, const CULL_MODE cullMode);

		void updateDepthBias(ValProc& proc, const float depthBiasConstant, const float depthBiasClamp, const float depthBiasSlopeFactor);

		void updateBuffers(ValProc& proc);

		/************************************************************************************************************/
		/* BUFFER MANIPULATION */

		void updateDescriptorSet(ValProc& proc, GraphicsPipeline& pipeline, DescriptorSheet& sheet, const uint32_t setIndex);

		void updateIndexBuffer(ValProc& proc);

		void updateVertexBuffers(ValProc& proc);

		void updateAndSetIndexBuffer(ValProc& proc, val::Buffer& buffer, const uint32_t& indexCount);

		void updateAndSetIndexBuffer(ValProc& proc, const VkBuffer& buffer, const uint32_t& indexCount);

		void updateAndSetVertexBuffer(ValProc& proc, const VkBuffer& buffer, const uint32_t& vertexCount);

		void updateAndSetVertexBuffer(ValProc& proc, val::Buffer& buffer, const uint32_t& vertexCount);

		void updateAndSetVertexBuffers(ValProc& proc, const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount);

		void updateAndSetVertexBuffers(ValProc& proc, const std::vector<val::Buffer*>& vertexBuffers, const uint32_t& vertexCount);

		void updateAndSetVertexBufferAndIndexBuffer(ValProc& proc, val::Buffer& vertexBuffer, const uint32_t& vertexCount, val::Buffer& indexBuffer, const uint32_t& indexCount);

		void updateAndSetVertexBufferAndIndexBuffer(ValProc& proc, const VkBuffer& vertexBuffer, const uint32_t& vertexCount, const VkBuffer& indexBuffer, const uint32_t& indexCount);

		void updateAndSetVertexBuffersAndIndexBuffer(ValProc& proc, const std::vector<val::Buffer*>& vertexBuffers, const uint32_t& vertexCount, val::Buffer& indexBuffer, const uint32_t& indexCount);
		
		void updateAndSetVertexBuffersAndIndexBuffer(ValProc& proc, const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount, const VkBuffer& indexBuffer, const uint32_t& indexCount);

		/************************************************************************************************************/

		void update(ValProc& proc, const GraphicsPipeline& pipeline, const std::vector<VkViewport>& viewports);

		void begin(ValProc& proc);

		void beginPass(ValProc& proc, VkRenderPass renderPass, VkFramebuffer& frameBuffer);

		void endPass(ValProc& proc);

		void submit(ValProc& proc, std::vector<VkSemaphore> waitSemaphores, VkFence fence = VK_NULL_HANDLE);

	public:
		inline void setVertexBuffer(const VkBuffer& buffer, const uint32_t& vertexCount) {
			_vertexBuffers = { buffer };
			_vertexCount = vertexCount;


			_vertexBufferOffsets.resize(_vertexBuffers.size());
		}

		inline void setVertexBuffer(val::Buffer& buffer, const uint32_t& vertexCount) {
			_vertexBuffers = { buffer.getVkBuffer()};
			_vertexCount = vertexCount;


			_vertexBufferOffsets.resize(_vertexBuffers.size());
		}

		inline void setVertexBuffers(const std::vector<VkBuffer>& vertexBuffers, const uint32_t& vertexCount) {
			_vertexBuffers = vertexBuffers;
			_vertexCount = vertexCount;


			_vertexBufferOffsets.resize(vertexBuffers.size());
		}

		inline void setVertexBuffers(const std::vector<val::Buffer*>& vertexBuffers, const uint32_t& vertexCount) {
			_vertexBuffers.resize(vertexBuffers.size());
			for (uint_fast16_t i = 0; i < vertexBuffers.size(); ++i) {
				_vertexBuffers[i] = vertexBuffers[i]->getVkBuffer();
			}
			_vertexCount = vertexCount;


			_vertexBufferOffsets.resize(vertexBuffers.size());
		}


		inline const std::vector<VkBuffer>& getVertexBuffer() const {
			return _vertexBuffers;
		}

		inline void setIndexBuffer(val::Buffer& buffer, const uint32_t& indexCount) {
			_indexBuffer = buffer.getVkBuffer();
			_indexCount = indexCount;
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
			_renderPassBeginInfo.clearValueCount = (uint32_t)_clearValues.size();
			_renderPassBeginInfo.pClearValues = _clearValues.data();
		}

		inline void setClearValues(VkClearValue* clearValues, uint32_t clearValueCount) {
			_clearValues.clear();
			for (uint32_t i = 0; i < clearValueCount; ++i) {
				_clearValues.push_back(clearValues[i]);
			}
			_renderPassBeginInfo.clearValueCount = uint32_t(_clearValues.size());
			_renderPassBeginInfo.pClearValues = _clearValues.data();
		}

		inline const std::vector<VkClearValue>& getClearValues() {
			return _clearValues;
		} 

		inline void setQueue(Queue& queue) 
		{
			_queue = &queue;
		}

	protected:
		Queue* _queue;

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