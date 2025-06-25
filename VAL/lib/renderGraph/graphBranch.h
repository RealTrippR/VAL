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

#include <VAL/lib/renderGraph/passInfo.h>
#include <VAL/lib/VALreturnCode.h>
#include <VAL/lib/C_compatibleBinding.h>
#ifndef VAL_GRAPH_BRANCH_H
#define VAL_GRAPH_BRANCH_H

struct graphBranch
{
	struct PASS_INFO* head;
	struct PASS_INFO** intermediatePasses;
	uint32_t intermediatePassCount;
	struct PASS_INFO* tail;
};

VAL_C_COMPATIBLE_BINDING VAL_RETURN_CODE graphBranchNew(graphBranch* graphBranch);

VAL_C_COMPATIBLE_BINDING VAL_RETURN_CODE graphBranchDestroy(graphBranch* graphBranch);

VAL_C_COMPATIBLE_BINDING VAL_RETURN_CODE graphBranchAddIntermediatePass();

VAL_C_COMPATIBLE_BINDING VAL_RETURN_CODE graphBranchAddIntermediatePass();

#endif // !VAL_GRAPH_BRANCH_H