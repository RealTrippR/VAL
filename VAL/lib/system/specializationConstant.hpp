#ifndef VAL_SPECIALIZATION_CONSTANT_HPP
#define VAL_SPECIALIZATION_CONSTANT_HPP

#include <VAL/lib/system/system_utils.hpp>

namespace val {
	class specializationConstant
	{
	public:
		specializationConstant() = default;
		specializationConstant(void* data, uint16_t dataSize, uint16_t dataOffset, uint16_t id) {
			_data = data;
			_dataSize = dataSize;
			_dataOffset = dataOffset;
#ifndef NDEBUG
			if (dataOffset > dataSize) {
				printf("VAL: ERROR: The offset of a specializationConstant cannot be greater than it's size!\n");
				printf("Size: %u, Offset: %u", dataSize, dataOffset);
				throw std::logic_error("VAL: ERROR: The offset of a specializationConstant cannot be greater than it's size!");
			}
#endif // !NDEBUG
		}

		// VkSpecializationMapEntry to specializationConstant
		specializationConstant operator=(const VkSpecializationMapEntry& other) {
			specializationConstant ret;
			ret._id = other.constantID;
			ret._dataOffset = other.offset;
			ret._dataSize = other.size;

			return ret;
		}

		// conversion operator, converts VkSpecializationMapEntry to VkSpecializationMapEntry
		operator VkSpecializationMapEntry() const {
			VkSpecializationMapEntry entry = {};
			entry.constantID = _id;
			entry.offset = _dataOffset;
			entry.size = _dataSize;
			return entry;
		}


	public:
		void* _data = NULL;
		uint16_t _dataSize = 0u;
		uint16_t _dataOffset = 0u;
		uint16_t _id = 0u;
	};
}
#endif // !VAL_SPECIALIZATION_CONSTANT_HPP
