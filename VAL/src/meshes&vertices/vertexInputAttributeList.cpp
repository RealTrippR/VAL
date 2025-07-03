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

#include <VAL/lib/meshes&vertices/vertexInputAttributeList.hpp>

namespace val
{
	void VertexInputAttributeList::addInputAttribute(const VertexInputAttribute& attribute)
	{
		vertexInputAttributes.push_back(attribute);
	}

	void VertexInputAttributeList::removeInputAttribute(const VertexInputAttribute& attribute)
	{
		vertexInputAttributes.push_back(attribute);
	}

	tiny_vector<VkVertexInputAttributeDescription> VertexInputAttributeList::toVkVertexInputAttributeDescription() const
	{
		uint32_t lastInputLocation = 0u;
		uint16_t lastInputSize = 0u;
		tiny_vector<VkVertexInputAttributeDescription> retVal;
		for (uint32_t i = 0; i < vertexInputAttributes.size(); ++i)
		{
			const VertexInputAttribute& inputAttrib = vertexInputAttributes[i];
			tiny_vector<VkVertexInputAttributeDescription> asInputAttribDescs = inputAttrib.toVkVertexInputAttributeDescriptions();

			for (uint32_t i = 0; i < asInputAttribDescs.size(); ++i)
			{
				retVal.push_back(asInputAttribDescs[i]);
			}
		}

		return retVal;
	}


	std::vector<VkVertexInputAttributeDescription> VertexInputAttributeList::toVkVertexInputAttributeDescriptionStdVector() const
	{
		uint32_t lastInputLocation = 0u;
		uint16_t lastInputSize = 0u;
		std::vector<VkVertexInputAttributeDescription> retVal;
		for (uint32_t i = 0; i < vertexInputAttributes.size(); ++i)
		{
			const VertexInputAttribute& inputAttrib = vertexInputAttributes[i];
			tiny_vector<VkVertexInputAttributeDescription> asInputAttribDescs = inputAttrib.toVkVertexInputAttributeDescriptions();
			
			for (uint32_t i = 0; i < asInputAttribDescs.size(); ++i) 
			{
				retVal.push_back(asInputAttribDescs[i]);
			}
		}

		return retVal;
	}

	const tiny_vector<VertexInputAttribute>& VertexInputAttributeList::getVertexAttributes() const
	{
		return vertexInputAttributes;
	}

	tiny_vector<VertexInputAttribute>& VertexInputAttributeList::getVertexAttributes()
	{
		return vertexInputAttributes;
	}
}