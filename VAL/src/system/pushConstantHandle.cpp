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

#include <VAL/lib/system/pushConstantHandle.hpp>
#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/graphics/shader.hpp>
#include <VAL/lib/system/pipelineBase.hpp>

namespace val {
	void pushConstantHandle::update(ValProc& proc, void* data, const PipelineBase& pipeline, const Shader& shdr, VkCommandBuffer& cmdBuffer) {
#ifndef NDEBUG
		if (_size % 4 != 0) {
			printf("VAL: WARNING: Push constant at memory address %p has a size that is not a multiple of 4! It's size is: %d", this, _size);
		}

#endif // !NDEBUG

		vkCmdPushConstants(
			cmdBuffer,
			proc._graphicsPipelineLayouts[pipeline.pipelineIdx],
			shdr._shaderStageFlags,
			_offset,
			_size,
			data
		);
	}

	void pushConstantHandle::update(ValProc& proc, void* data, const PipelineBase& pipeline, VkCommandBuffer& cmdBuffer) {
		vkCmdPushConstants(
			cmdBuffer,
			proc._graphicsPipelineLayouts[pipeline.pipelineIdx],
			_stageFlags,
			_offset,
			_size,
			data
		);
	}


	VkPushConstantRange pushConstantHandle::toVkPushConstantRange() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = _stageFlags;  // Used in the vertex shader
		pushConstantRange.offset = _offset;
		pushConstantRange.size = _size;
		return pushConstantRange;
	}
}