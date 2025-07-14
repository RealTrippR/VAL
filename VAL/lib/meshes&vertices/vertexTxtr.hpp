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


#ifndef VERTEX_TEXTURED_HPP
#define VERTEX_TEXTURED_HPP

#include <VAL/lib/meshes&vertices/vertexBase.hpp>
#include <VAL/lib/meshes&vertices/vertexInputAttributeList.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <string.h>

namespace val
{
	class VertexTxtr : public VertexBase
	{
	public:
		VertexTxtr() = default;
		VertexTxtr(glm::vec3 _pos, glm::vec2 _texCoord, glm::vec3 _normal)
		{
			pos = _pos;
			texCoord = _texCoord;
			normal = _normal;
		}

		VertexTxtr& operator=(const VertexTxtr&) = default;

		bool operator==(const VertexTxtr&) const = default;

		
	public:
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;

	public:

		inline void setPositionFromVec3(glm::vec3 v) { pos = v; }

		inline void setNormalFromVec3(glm::vec3 v) { normal = v; }

		inline void setColorFromVec4(glm::vec3 v) {}

		inline glm::vec3 getPositionAsVec3() const
		{
			return pos;
		}

		void loadFromAttributes(const glm::vec3& pos, const size_t index, const glm::vec4 color, const glm::vec3& normal, const tiny_vector<glm::vec2>& UVs)
		{
			this->pos = pos;
			if (!UVs.empty()) 
			{
				this->texCoord = UVs[0];
			}
			this->normal = normal;
		}

		static const tiny_vector<VkVertexInputAttributeDescription>& getInputAttributeDescriptions()
		{
			static tiny_vector<VkVertexInputAttributeDescription> r;
			if (r.size() > 0)
				return r;
			static const VertexInputAttributeList vertexAttributes = {
				   VertexInputAttribute(0, (VERTEX_ATTRIBUTE_TYPE)getPositionFormat(), offsetof(VertexTxtr, pos)),
				   VertexInputAttribute(1, vec2, offsetof(VertexTxtr, texCoord)),
				   VertexInputAttribute(2, vec3, offsetof(VertexTxtr, normal))
			};
			r = vertexAttributes.toVkVertexInputAttributeDescription();
			return r;
		}

		static VkVertexInputBindingDescription getBindingDescription()
		{
			static const VkVertexInputBindingDescription bindingDescription
			{
				.binding = 0,
				.stride = sizeof(VertexTxtr),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			};
			return bindingDescription;
		}

		inline static VkDeviceSize getStride() {
			return sizeof(VertexTxtr);
		}

		inline static VkFormat getPositionFormat() {
			return (VkFormat)VERTEX_ATTRIBUTE_TYPE::vec3;
		}
	};
}



namespace std {
	template<> struct hash<val::VertexTxtr> {
		inline size_t operator()(val::VertexTxtr const& vertex) const {
			return (hash<glm::vec3>()(vertex.pos));
		}
	};
}


#endif