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


#ifndef VAL_RENDER_GRAPH_PASS_HPP
#define VAL_RENDER_GRAPH_PASS_HPP

#include <VAL/lib/system/VAL_PROC.hpp>
#include <VAL/lib/renderGraph/passContext.hpp>
#include <VAL/lib/system/window.hpp>
#include <VAL/lib/renderGraph/renderGraphBlock.h>
#include <VAL/lib/system/renderTarget.hpp>
#include <VAL/lib/renderGraph/passFunctions.hpp>

#define PASS_BEGIN(NAME) void PASS_##NAME(val::VAL_PROC& V_PROC,
#define PASS_END

#define FIXED_BEGIN(...) { __VA_ARGS__ 
#define FIXED_END }

#define READ(...) __VA_ARGS__,
#define WRITE(...) __VA_ARGS__,
#define READ_WRITE(...) __VA_ARGS__,
#define INPUT(...) __VA_ARGS__ 
#define EXEC(v)

#ifndef VAL_RENDER_PASS_COMPILE_MODE

#define CALL_RENDER_PASS(NAME, ...) pass_main##NAME(__VA_ARGS__)
#define BAKE_RENDER_PASS(NAME, ...) pass_bake##NAME(__VA_ARGS__)

#else

#define CALL_RENDER_PASS(...)
#define BAKE_RENDER_PASS(...)

#endif // !VAL_RENDER_PASS_COMPILE_MODE

#endif // !VAL_RG_PASS_HPP