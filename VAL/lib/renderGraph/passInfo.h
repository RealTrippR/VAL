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
#include <stdlib.h>

enum PASS_FLAGS {
	FIXED = 1
};

struct PASS_INFO {
	enum PASS_FLAGS flags;
	
	struct ARG_BLOCK readBlock;
	struct ARG_BLOCK writeBlock;
	struct ARG_BLOCK readWriteBlock;

	char* passName;
	char* execSrc;
	uint32_t execSrcLen;
};

VAL_C_COMPATIBLE_BINDING void PRINT_ARG_BLOCK(struct ARG_BLOCK* argblock);

VAL_C_COMPATIBLE_BINDING void PASS_INFO_CLEANUP(struct PASS_INFO* pass);

#endif // !VAL_PASS_INFO_H