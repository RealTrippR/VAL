#include <FML/lib/graphics/vertexShader.hpp>;

namespace fml {

	void vertexShader::setVertexAttributes(VkVertexInputAttributeDescription* attributes, const uint32_t& attribCount) {
		_attributes = attributes;
		_attribCount = attribCount;
	}

	VkVertexInputAttributeDescription* vertexShader::getVertexAttributes() noexcept {
		return _attributes;
	}


	const uint32_t& vertexShader::getVertexAttributesCount() noexcept {
		return _attribCount;
	}

	void vertexShader::setBindingDescription(const VkVertexInputBindingDescription& bindingDescription) {
		_bindingDescription = bindingDescription;
	}

	const VkVertexInputBindingDescription& vertexShader::getBindingDescription() noexcept {
		return _bindingDescription;
	}

	VkShaderStageFlagBits vertexShader::getStageFlagBits() {
		return VK_SHADER_STAGE_VERTEX_BIT;
	}

	std::vector<VkDescriptorSetLayoutBinding>* vertexShader::getLayoutBindings() {
		std::vector<VkDescriptorSetLayoutBinding>* layoutBindings = shader::getLayoutBindings();
		
		return layoutBindings;
	}
}