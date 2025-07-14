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

#ifndef VAL_ACCELERATION_STRUCTURE_HPP
#define VAL_ACCELERATION_STRUCTURE_HPP

#include <VAL/lib/ext/tiny_vector.hpp>
#include <VAL/lib/system/accelerationStructureGeometry.hpp>
#include <VAL/lib/system/accelerationStructureInstance.hpp>
#include <VAL/lib/system/system_utils.hpp>

namespace val
{
	class AccelerationStructure
	{
	public:
		void setInstances(const tiny_vector<AccelerationStructureInstance*>& instances);

		const tiny_vector<AccelerationStructureInstance*>& getInstances() const;

		tiny_vector<AccelerationStructureInstance*>& getInstances();

		tiny_vector<const AccelerationStructureGeometry*> getGeometries() const;

		void setAccelerationStructureInstances(const tiny_vector<AccelerationStructureInstance*>& instances);

		const tiny_vector<AccelerationStructureInstance*>& getAccelerationStructureInstances() const;

		VAL_RETURN_CODE buildAsBottomLevel(ValProc& proc, Queue& rayqueue, uint32_t vertexCount, uint32_t indexCount);

		VAL_RETURN_CODE buildAsTopLevel(ValProc& proc, AccelerationStructure& TLAS, Queue& rayqueue);

		VkAccelerationStructureBuildSizesInfoKHR getBuildSizes(ValProc& proc, const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo, const uint32_t* pMaxPrimitiveCounts) const;
	
		VkDescriptorType getVkDescriptorType() {
			return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		}

		inline static void toObjectDescriptorInfo(ObjectDescriptorInfo* descInfo)
		{
			AccelerationStructure* self = (AccelerationStructure*)descInfo->valObject;
			descInfo->type = self->getVkDescriptorType();
			descInfo->arrCount = 1u;
			VkWriteDescriptorSetAccelerationStructureKHR* descWriteInfo = 
				(VkWriteDescriptorSetAccelerationStructureKHR*)malloc(sizeof(VkWriteDescriptorSetAccelerationStructureKHR));

			descWriteInfo->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
			descWriteInfo->pNext = VK_NULL_HANDLE;
			descWriteInfo->accelerationStructureCount = 1u;
			descWriteInfo->pAccelerationStructures = &self->_accelStructure;

			descInfo->pNext = descWriteInfo;
		}

		operator const ObjectDescriptorInfo()
		{
			ObjectDescriptorInfo info{
				.valObject = this ,
				.updateDataCallback = toObjectDescriptorInfo
			};
			toObjectDescriptorInfo(&info);
			return info;
		}

	public:
		tiny_vector<AccelerationStructureInstance*> _instances;



		VkBuffer _accelStructBuffer = VK_NULL_HANDLE;
		VkDeviceMemory _accelStructMemory = VK_NULL_HANDLE;
		VkAccelerationStructureKHR _accelStructure = VK_NULL_HANDLE;

		VkBuffer _instanceBuff = VK_NULL_HANDLE;
		VkDeviceMemory _instanceBuffMem = VK_NULL_HANDLE;


		VkBuffer _transientScratchBuffer = VK_NULL_HANDLE;
		VkDeviceMemory _transientScratchMemory = VK_NULL_HANDLE;
	};
}

#endif // !NDEBUG