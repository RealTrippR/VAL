#include <VAL/lib/system/pushDescriptor.hpp>

namespace val {
	void pushDescriptor::setBindingIndex(const uint16_t index) {
		_bindingIndex = index;
	}
	uint16_t pushDescriptor::getBindingIndex() {
		return _bindingIndex;
	}
	void pushDescriptor::setValueCount(const uint16_t count) {
		_valueCount = count;
	}
	uint16_t pushDescriptor::getValueCount() {
		return _valueCount;
	}
	void pushDescriptor::setDescType(const DESC_TYPE dtype) {
		_type = dtype;
	}
	DESC_TYPE pushDescriptor::getDescType() {
		return _type;
	}
}