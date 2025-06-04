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

#ifndef VAL_CLASS_ENUM_BITWISE_OPS
#define VAL_CLASS_ENUM_BITWISE_OPS

#include <optional> // needed for underlying_type_t

#define DEF_ENUM_BITWISE_OR(TYPE) inline TYPE operator|(const TYPE a, const TYPE b) { return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) | static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_AND(TYPE) inline TYPE operator&(const TYPE a, const TYPE b) { return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) & static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_XOR(TYPE) inline TYPE operator^(const TYPE a, const TYPE b) { return static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) ^ static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_NOT(TYPE) inline TYPE operator~(const TYPE a) { return static_cast<TYPE>(~static_cast<std::underlying_type_t<TYPE>>(a));}

#define DEF_ENUM_BITWISE_OR_ASSIGN(TYPE) inline TYPE operator|=(TYPE& a, const TYPE& b) { return a = static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) | static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_AND_ASSIGN(TYPE) inline TYPE operator&=(TYPE& a, const TYPE& b) { return a = static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) & static_cast<std::underlying_type_t<TYPE>>(b));}
#define DEF_ENUM_BITWISE_XOR_ASSIGN(TYPE) inline TYPE operator^=(TYPE& a, const TYPE& b) { return a = static_cast<TYPE>(static_cast<std::underlying_type_t<TYPE>>(a) ^ static_cast<std::underlying_type_t<TYPE>>(b));}

#define DEF_ENUM_BITWISE_OPERATORS(TYPE)\
DEF_ENUM_BITWISE_OR(TYPE)\
DEF_ENUM_BITWISE_AND(TYPE)\
DEF_ENUM_BITWISE_XOR(TYPE)\
DEF_ENUM_BITWISE_NOT(TYPE)\
DEF_ENUM_BITWISE_OR_ASSIGN(TYPE)\
DEF_ENUM_BITWISE_AND_ASSIGN(TYPE)\
DEF_ENUM_BITWISE_XOR_ASSIGN(TYPE)


#endif // !VAL_CLASS_ENUM_BITWISE_OPS