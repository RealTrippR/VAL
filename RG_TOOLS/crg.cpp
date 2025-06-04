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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <VAL/lib/renderGraph/renderGraph.hpp>

void printCommandList() {
	printf("-help\n"
		"\t prints a list of all available commands.\n"
	);
	printf("-c <graph source file>\n"
		"\t compiles a render graph.\n"
	);
}


int main(int argc, char* argv[]) {
	printf("-- VAL Render Graph Compiler --\n");
	// remember that argv[0] is the name of the program
	if (argc > 1) {
		const char* cmd = argv[1];

		if (strcmp(cmd, "-help")==0) {
			printCommandList();
			return 1;
		}
		else {
			if (strcmp(cmd,"-compile")) {
				if (argc > 2) {
					// get src path
					const char* graphSrcFile = argv[2];

					using namespace val;

					RENDER_GRAPH renderGraph;
					renderGraph.loadFromFile(graphSrcFile);

					// note that this could technically be compiled to a header file and turned into an inline function :)
					//renderGraph.compile(MSVC, { "C:/Users/TrippR/OneDrive/Documents/REPOS/VAL/" , "C:/Users/TrippR/OneDrive/Documents/REPOS/VAL/ExternalLibraries/glm", "C:/Users/TrippR/OneDrive/Documents/REPOS/VAL/ExternalLibraries/GLFW/include", "C:/Users/TrippR/OneDrive/Documents/REPOS/VAL/ExternalLibraries/Vulkan/Include"},
					//	"test_graph.dll", "experimental-features");

					int test = 5;

					VAL_RETURN_CODE code = renderGraph.compile();
					if (code == VAL_SUCCESS) {
						printf("Successfully compiled render graph\n");
					}
					else {
						printf("Failed to compile render graph\n");
					}
				}
			}

			return 1;
		}


		printf("No or invalid command were provided, -help for a list of commands\n");

	}

	return 0;
}