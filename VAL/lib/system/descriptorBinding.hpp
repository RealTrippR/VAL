#ifndef VAL_DESCRIPTOR_BINDING_HPP
#define VAL_DESCRIPTOR_BINDING_HPP


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

namespace val {

	template <typename T> class descriptorBinding
	{
	public:
		descriptorBinding() = default;
		descriptorBinding(T val, uint32_t bindingIndex) {
			this->values.resize(1);
			this->values[0] = val;
			this->bindingIndex = bindingIndex;
		}
		descriptorBinding(std::vector<T> values, uint32_t bindingIndex) {
			this->values = values;
			this->bindingIndex = bindingIndex;
		}
	public:
		std::vector<T> values;
		uint32_t bindingIndex = 0;
	};
}

#endif // !VAL_DESCRIPTOR_BINDING_HPP