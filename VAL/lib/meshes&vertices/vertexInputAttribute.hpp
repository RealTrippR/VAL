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

#ifndef VAL_VERTEX_INPUT_ATTRIBUTES_HPP
#define VAL_VERTEX_INPUT_ATTRIBUTES_HPP

#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/meshes&vertices/vertexAttributeType.hpp>

namespace val 
{
	class VertexInputAttribute
	{
	public:
		VertexInputAttribute() = default;

		VertexInputAttribute(uint32_t location, VERTEX_ATTRIBUTE_TYPE type)
		{
			this->location = location;
			this->type = type;
		}
		VertexInputAttribute(uint32_t location, VERTEX_ATTRIBUTE_TYPE type, uint32_t offset)
		{
			this->location = location;
			this->offset = offset;
			this->type = type;
		}
		VertexInputAttribute(uint32_t location, VERTEX_ATTRIBUTE_TYPE type, uint32_t offset, uint32_t binding)
		{
			this->location = location;
			this->binding = binding;
			this->type = type;
			this->offset = offset;
		}

	public:
		tiny_vector<VkVertexInputAttributeDescription> toVkVertexInputAttributeDescriptions() const;

	public:
		uint32_t    location = 0u;
		uint32_t    binding = 0u;
		VERTEX_ATTRIBUTE_TYPE type = VERTEX_ATTRIBUTE_TYPE::undefined;
		uint32_t    offset = 0u;
	};
}


#endif // !VAL_VERTEX_INPUT_ATTRIBUTES_HPP