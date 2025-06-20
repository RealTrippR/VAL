/*
Copyright � 2025 Tripp Robins

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the �Software�), to deal in the Software
without restriction, including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef VAL_ARG_BLOCK_H
#define VAL_ARG_BLOCK_H

#include <VAL/lib/C_compatibleBinding.h>
#include <stdint.h>

struct ARG_BLOCK {
	// A contiguous block of memory, laid out like so: "arg1\0arg2\0";
	char* args;
	// The number of arguments in char* args.
	uint16_t argCount;
};


VAL_C_COMPATIBLE_BINDING void ARG_BLOCK_DESTROY(struct ARG_BLOCK* block);

#endif // !VAL_ARG_BLOCK_H