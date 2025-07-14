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


#ifndef VAL_PLY_SCENE_HPP
#define VAL_PLY_SCENE_HPP

#include <ExternalLibraries/cply/c_polygon.h>
#include <VAL/lib/system/system_utils.hpp>

namespace val
{
	class PlyScene
	{
	public:
		PlyScene() = default;
		PlyScene(const fs::path& source)
		{
			loadFromDisk(source);
		}
		PlyScene(void* memory, const size_t memorySize)
		{
			loadFromMemory(memory, memorySize);
		}
		~PlyScene()
		{
			destroy();
		}

		operator cply::PlyScene* ()
		{
			return &_scene;
		}

		operator const cply::PlyScene* () const
		{
			return &_scene;
		}

        inline const cply::PlyScene* getPlyScene() const {
            return &_scene;
        }

        inline cply::PlyScene* getPlyScene() {
            return &_scene;
        }
	public:
		VAL_RETURN_CODE loadFromMemory(const void* memoryBegin, size_t memorySize);

		VAL_RETURN_CODE loadFromDisk(const fs::path& filepath);

		void destroy();

	private:

        cply::PlyScene _scene = { 0 };

    public:
        inline static double getDataFromPropertyOfElement(const struct cply::PlyElement* e, const struct cply::PlyProperty* prop, const cply::U64 dataLineIdx, cply::U8* success)
        {
            using namespace cply;
            const U64 offset = e->dataLineBegins[dataLineIdx] + prop->dataLineOffsets[dataLineIdx];
            if (offset >= e->dataSize || dataLineIdx >= e->dataLineCount) {
                if (success)
                    *success = 0;
                return 0;
            }

            U8* f = ((U8*)e->data) + offset;
            if (success)
                *success = 1;
            return PlyScaleBytesToD64(f, prop->scalarType);
        }


        inline static void getDataFromPropertyOfElementAsList(double* dstBuffer, const size_t dstBufferSize, size_t* elementCountOut,
            const struct cply::PlyElement* e, const struct cply::PlyProperty* prop, const cply::U64 dataLineIdx, cply::U8* success)
        {
            using namespace cply;
            U64 offset = e->dataLineBegins[dataLineIdx] + prop->dataLineOffsets[dataLineIdx];
            if (offset >= e->dataSize) { // check for out of bounds read
                if (success)
                    *success = 0;
                return;
            }
            U8* f = ((U8*)e->data) + offset;
            U64 count = (U64)PlyScaleBytesToD64(f, prop->listCountType);
            if (elementCountOut != NULL) {
                *elementCountOut = count;
            }
            if (!dstBuffer) {
                return;
            }
            offset += PlyGetSizeofScalarType(prop->listCountType);

            for (U64 i = 0; i < std::min(count, dstBufferSize / sizeof(double)); ++i)
            {
                U8* f2 = ((U8*)e->data) + offset;
                const U64 sze = PlyGetSizeofScalarType(prop->scalarType);
                if (offset + sze > e->dataSize) { // check for out of bounds read
                    if (success)
                        *success = 0;
                    return;
                }
                dstBuffer[i] = PlyScaleBytesToD64(f2, prop->scalarType);

                offset += sze;
            }
            if (success)
                *success = 1;
        }

	};
}

#endif // !VAL_PLY_SCENE_HPP