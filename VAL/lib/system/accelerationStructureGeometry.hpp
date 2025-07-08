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

#ifndef VAL_ACCELERATION_STRUCTURE_GEOMETRY_HPP
#define VAL_ACCELERATION_STRUCTURE_GEOMETRY_HPP

#include <vulkan/vulkan_core.h>
namespace val
{
	enum class GEOMETRY_TYPE : VkFlags
	{
		Triangles= VK_GEOMETRY_TYPE_TRIANGLES_KHR,
		AABBs= VK_GEOMETRY_TYPE_AABBS_KHR,
		Instances=VK_GEOMETRY_TYPE_INSTANCES_KHR,
		SpheresNV = VK_GEOMETRY_TYPE_SPHERES_NV,
		LinearSweptSpheresNV =	VK_GEOMETRY_TYPE_LINEAR_SWEPT_SPHERES_NV
	};
	enum class GEOMETRY_FLAGS : VkFlags
	{
		Opaque = VK_GEOMETRY_OPAQUE_BIT_KHR,
		NoDuplicateAnyHitInvocation = VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR
	};

	// can be directly casted to VkAccelerationStructureGeometryKHR 
	struct AccelerationStructureGeometry 
	{
	public:
		void create();

		void setTriangleGeometryData(VkDevice device, const VkDeviceSize vertexStride, const VkFormat positionFormat, const VkBuffer vertexBuffer, const uint32_t vertexCount);

		void setTriangleGeometryData(VkDevice device, const VkDeviceSize vertexStride, const VkFormat positionFormat, const VkBuffer vertexBuffer, const VkBuffer indexBuffer, const uint32_t vertexCount);

		void setAabbGeometryData();

		void setInstancesGeometryData();

		// Conversion function
		VkAccelerationStructureGeometryKHR toVk() const {
			VkAccelerationStructureGeometryKHR geo{};
			geo.sType = sType;
			geo.pNext = pNext;
			geo.geometryType = geometryType;
			geo.geometry = geometry;
			geo.flags = flags;
			return geo;

		}
	public:
		VkStructureType   sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		const void* pNext = VK_NULL_HANDLE;
		VkGeometryTypeKHR                         geometryType;
		VkAccelerationStructureGeometryDataKHR    geometry;
		VkGeometryFlagsKHR                        flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	};
}

#endif // !VAL_ACCELERATION_STRUCTURE_GEOMETRY_HPP