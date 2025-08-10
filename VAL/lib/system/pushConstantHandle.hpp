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

#ifndef VAL_PUSH_CONSTANT_HANDLE_HPP
#define VAL_PUSH_CONSTANT_HANDLE_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	class ValProc; // forward declaration
	class PipelineBase; // forward declaration
	class Shader; // forward declaration
	struct pushConstantHandle {
		pushConstantHandle() = default;
		pushConstantHandle(const uint16_t size, SHADER_STAGE stageFlags, const uint16_t offset = 0)
		{
			/*align to 4 byte size*/
			const uint32_t multiple = 4;
			const uint32_t s = (size + multiple - 1) / multiple;
			_size = s * multiple;

			_stageFlags = (VkShaderStageFlags)stageFlags;
			_offset = offset;
			_procMemoryOffset = 0;
		}
		
		void update(ValProc& proc, void* data, const PipelineBase& pipeline, VkCommandBuffer cmdBuffer);

		void update(ValProc& proc, void* data, VkPipelineLayout pipelineLayout, VkCommandBuffer cmdBuffer);

		void setSize(uint32_t size);
		
		uint32_t getSize() const;

		VkPushConstantRange toVkPushConstantRange();
	public:

		uint16_t _size;
		uint16_t _offset;

		VkShaderStageFlags _stageFlags = 0;
		uint16_t _procMemoryOffset = 0;
	};
}

#endif // !VAL_PUSH_CONSTANT_HANDLE_HPP