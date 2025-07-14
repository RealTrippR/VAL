#ifndef VAL_ACCELERATION_STRUCTURE_INSTANCE_HPP
#define VAL_ACCELERATION_STRUCTURE_INSTANCE_HPP

#include <vulkan/vulkan_core.h>
#include <VAL/lib/system/system_utils.hpp>
#include <VAL/lib/system/accelerationStructureGeometry.hpp>

namespace val
{
	class AccelerationStructure; // forward declaration

	class AccelerationStructureInstance
	{
	public:
		AccelerationStructureInstance() = default;

		AccelerationStructureInstance(const AccelerationStructureGeometry* geometry) {
			_geometry = geometry;
		}

		AccelerationStructureInstance(const AccelerationStructureGeometry& geometry) {
			_geometry = &geometry;
		}

	public:

		inline void setGeometry(const AccelerationStructureGeometry* geometry);

		inline const AccelerationStructureGeometry* getGeometry() const;

		inline void setTransformPosition(const glm::vec3 pos);

		inline void setTransform(const glm::mat4x3 transform);

		inline void setTransform(const VkTransformMatrixKHR transform);

		inline VkTransformMatrixKHR getTransform() const;

		inline void setFlags(const VkGeometryInstanceFlagsKHR flags);

		inline VkGeometryInstanceFlagsKHR getFlags() const;

		inline void setMask(uint8_t mask);

		inline uint8_t getMask() const;

		inline void setShaderBindingRecordTableOffset(const uint32_t offset);

		inline uint32_t getShaderBindingRecordTableOffset() const;

		inline VkAccelerationStructureInstanceKHR& asVkAccelerationStructureInstanceKHR();
		/*
		   uint32_t                      instanceCustomIndex:24;
    uint32_t                      mask:8;
    uint32_t                      instanceShaderBindingTableRecordOffset:24;
    VkGeometryInstanceFlagsKHR    flags:8;
    uint64_t                      accelerationStructureReference;
	*/
	private:
		/*	VkAccelerationStructureDeviceAddressInfoKHR addressInfo{};
		addressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		addressInfo.accelerationStructure = yourBlasHandle;

		VkDeviceAddress blasAddress = vkGetAccelerationStructureDeviceAddressKHR(device, &addressInfo);
		*/
		VkAccelerationStructureInstanceKHR accelStructInstance = {
			.transform = {
				1, 0, 0, 0/*x*/,
				0, 1, 0, 0/*y*/,
				0, 0, 1, 0/*z*/
			},
			.mask = 0xFF
		};
		const AccelerationStructureGeometry* _geometry;
	};



	inline void AccelerationStructureInstance::setGeometry(const AccelerationStructureGeometry* geometry)
	{
		_geometry = geometry;
	}

	inline const AccelerationStructureGeometry* AccelerationStructureInstance::getGeometry() const
	{
		return _geometry;
	}

	inline void AccelerationStructureInstance::setTransformPosition(const glm::vec3 pos)
	{
		VkTransformMatrixKHR& tr = accelStructInstance.transform;
		tr.matrix[0][3] = pos.x;
		tr.matrix[1][3] = pos.y;
		tr.matrix[2][3] = pos.z;
	}


	inline void AccelerationStructureInstance::setTransform(const glm::mat4x3 transform__)
	{
		VkTransformMatrixKHR& tr = accelStructInstance.transform;
		for (int row = 0; row < 3; ++row) {
			for (int col = 0; col < 4; ++col) {
				tr.matrix[row][col] = transform__[col][row];
			}
		}
	}


	inline void AccelerationStructureInstance::setTransform(const VkTransformMatrixKHR transform)
	{
		accelStructInstance.transform = transform;
	}

	inline VkTransformMatrixKHR AccelerationStructureInstance::getTransform() const
	{
		return accelStructInstance.transform;
	}

	inline void AccelerationStructureInstance::setFlags(const VkGeometryInstanceFlagsKHR flags)
	{
		accelStructInstance.flags = flags;
	}

	inline VkGeometryInstanceFlagsKHR AccelerationStructureInstance::getFlags() const
	{
		return accelStructInstance.flags;
	}

	inline void AccelerationStructureInstance::setMask(uint8_t mask)
	{
		accelStructInstance.mask = mask;
	}

	inline uint8_t AccelerationStructureInstance::getMask() const
	{
		return accelStructInstance.mask;
	}

	inline void AccelerationStructureInstance::setShaderBindingRecordTableOffset(const uint32_t offset)
	{
		accelStructInstance.instanceShaderBindingTableRecordOffset = offset;
	}

	inline uint32_t AccelerationStructureInstance::getShaderBindingRecordTableOffset() const
	{
		return accelStructInstance.instanceShaderBindingTableRecordOffset;
	}

	inline VkAccelerationStructureInstanceKHR& AccelerationStructureInstance::asVkAccelerationStructureInstanceKHR() 
	{
		return accelStructInstance;
	}
}

#endif // !VAL_ACCELERATION_STRUCTURE_INSTANCE_HPP