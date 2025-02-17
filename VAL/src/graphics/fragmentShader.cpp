#include <FML/lib/graphics/fragmentShader.hpp>

namespace fml {
;	VkShaderStageFlagBits fragmentShader::getStageFlagBits() {
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	std::vector<VkDescriptorSetLayoutBinding>* fragmentShader::getLayoutBindings() {
		std::vector<VkDescriptorSetLayoutBinding>* layoutBindings = shader::getLayoutBindings();

		return layoutBindings;
	}
}