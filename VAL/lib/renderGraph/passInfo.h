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

#ifndef VAL_PASS_INFO_H
#define VAL_PASS_INFO_H

#include <VAL/lib/C_compatibleBinding.h>
#include <VAL/lib/renderGraph/argBlock.h>
#include <VAL/lib/renderGraph/fixedBlock.h>
#include <stdlib.h>


struct PASS_INFO {
	
	struct ARG_BLOCK readBlock;
	struct ARG_BLOCK writeBlock;
	struct ARG_BLOCK readWriteBlock;
	struct ARG_BLOCK inputBlock;

	char* passName; // owned by PASS_INFO
	char* execSrc; // owned by PASS_INFO
	uint32_t execSrcLen;

	uint16_t fixedBlockCount;

	struct FIXED_BLOCK* fixedBlocks;
};

// returns a reference to an argument at the given index.
VAL_C_COMPATIBLE_BINDING char* GET_ARG_FROM_ARG_BLOCK(const struct ARG_BLOCK* argblock, const uint16_t argIndex);

VAL_C_COMPATIBLE_BINDING void PRINT_ARG_BLOCK(struct ARG_BLOCK* argblock);

VAL_C_COMPATIBLE_BINDING void PASS_INFO_CLEANUP(struct PASS_INFO* pass);

#endif // !VAL_PASS_INFO_H