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

#include <VAL/lib/C_compatibleBinding.h>
#include <stdint.h>
#include <stdbool.h>

/// streql() - Tests equality between two strings
/// @name streql
/// @brief Tests equality between two C strings. It differs from strcmp in 
/// the fact that it stops once immediately after a mismatch is detected, 
/// thus it is more efficient for certain use cases.
VAL_C_COMPATIBLE_BINDING bool streql(const char* str1, const char* str2);

/// strneql() - Tests equality between two strings within range n
/// @name strneql
/// @brief Tests equality between two C strings within range n. It differs from strncmp in 
/// the fact that it stops once immediately after a mismatch is detected, 
/// thus it is more efficient for certain use cases.
VAL_C_COMPATIBLE_BINDING bool strneql(const char* str1, const char* str2, const uint64_t n);