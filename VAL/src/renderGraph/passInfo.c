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

#include <stdio.h>
char* GET_ARG_FROM_ARG_BLOCK(const struct ARG_BLOCK* argblock, const uint16_t argIndex) {
	uint32_t i = 0u;
	uint32_t j = 0u;
	while (i < argblock->argCount) {
		if (j == 0 || argblock->args[j] == '\0') {
			if (argblock->args[j] == '\0') { j++; }
			if (i == argIndex) {
				return argblock->args + j;
			}
			i++;
		}
		++j;
	}
	return NULL;
}

void PRINT_ARG_BLOCK(struct ARG_BLOCK* argblock) {
	printf("-- ARG BLOCK: %h --\n", argblock);
	printf("Args:\n");
	uint32_t i = 0u;
	uint32_t j = 0u;
	while (i < argblock->argCount) {
		if (j == 0 || argblock->args[j] == '\0') {
			if (argblock->args[j] == '\0') { j++; }
			printf("| %s |\n", argblock->args+j);
			i++;
		}
		++j;
	}
	printf("-------------------\n");
}

void PASS_INFO_CLEANUP(struct PASS_INFO* pass) {
	ARG_BLOCK_DESTROY(&pass->readBlock);
	ARG_BLOCK_DESTROY(&pass->writeBlock);
	ARG_BLOCK_DESTROY(&pass->readWriteBlock);
	ARG_BLOCK_DESTROY(&pass->inputBlock);

	if (pass->fixedBlocks) {
		free(pass->fixedBlocks);
		pass->fixedBlocks = NULL;
		pass->fixedBlockCount = NULL;
	}

	if (pass->passName) {
		free(pass->passName);
		pass->passName = NULL;
	}
	if (pass->execSrc) {
		free(pass->execSrc);
		pass->execSrc = NULL;
	}
}