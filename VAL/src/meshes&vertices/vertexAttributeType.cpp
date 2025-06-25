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

#include <VAL/lib/meshes&vertices/vertexAttributeType.hpp>

namespace val
{
	tiny_vector<VkFormat> VERTEX_ATTRIBUTE_TYPE_toVkFormat(const VERTEX_ATTRIBUTE_TYPE type)
	{

		if (type <= VERTEX_ATTRIBUTE_TYPE::dvec4) {
			return { VkFormat(type) };
		}

		switch (type)
		{
		case VERTEX_ATTRIBUTE_TYPE::mat2:
			return tiny_vector<VkFormat>(2, VK_FORMAT_R32G32_SFLOAT);
		case VERTEX_ATTRIBUTE_TYPE::mat3:
			return tiny_vector<VkFormat>(3, VK_FORMAT_R32G32B32_SFLOAT);
		case VERTEX_ATTRIBUTE_TYPE::mat4:
			return tiny_vector<VkFormat>(4, VK_FORMAT_R32G32B32A32_SFLOAT);

		case VERTEX_ATTRIBUTE_TYPE::mat2x2:
			return tiny_vector<VkFormat>(2, VK_FORMAT_R32G32_SFLOAT);
		case VERTEX_ATTRIBUTE_TYPE::mat3x3:
			return tiny_vector<VkFormat>(3, VK_FORMAT_R32G32B32_SFLOAT);
		case VERTEX_ATTRIBUTE_TYPE::mat4x4:
			return tiny_vector<VkFormat>(4, VK_FORMAT_R32G32B32A32_SFLOAT);

		case VERTEX_ATTRIBUTE_TYPE::mat2x3:
			return tiny_vector<VkFormat>(2, VK_FORMAT_R32G32B32_SFLOAT);
		case VERTEX_ATTRIBUTE_TYPE::mat3x2:
			return tiny_vector<VkFormat>(3, VK_FORMAT_R32G32_SFLOAT);

		case VERTEX_ATTRIBUTE_TYPE::mat4x2:
			return tiny_vector<VkFormat>(4, VK_FORMAT_R32G32_SFLOAT);
		case VERTEX_ATTRIBUTE_TYPE::mat2x4:
			return tiny_vector<VkFormat>(2, VK_FORMAT_R32G32B32A32_SFLOAT);

		case VERTEX_ATTRIBUTE_TYPE::mat4x3:
			return tiny_vector<VkFormat>(4, VK_FORMAT_R32G32B32_SFLOAT);
		case VERTEX_ATTRIBUTE_TYPE::mat3x4:
			return tiny_vector<VkFormat>(3, VK_FORMAT_R32G32B32A32_SFLOAT);
		default:
			break;
		}

	}
}