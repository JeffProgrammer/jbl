//-----------------------------------------------------------------------------
// types.h
//
// Copyright (c) 2016-2017 Jeff Hutchinson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _JBL_TYPES_H_
#define _JBL_TYPES_H_

#include <stdint.h>

// Signed integer types
typedef int8_t  S8;  // 8 bit signed integer.
typedef int16_t S16; // 16bit signed integer.
typedef int32_t S32; // 32bit signed integer.
typedef int64_t S64; // 64bit signed integer.

// Unsigned integer types
typedef uint8_t  U8;  // 8 bit unsigned integer.
typedef uint16_t U16; // 16bit unsigned integer.
typedef uint32_t U32; // 32bit unsigned integer.
typedef uint64_t U64; // 64bit unsigned integer.

// Floating point types
typedef float  F32; // 32bit floating point number.
typedef double F64; // 64bit floating point number.

#endif // _JBL_TYPES_H_
