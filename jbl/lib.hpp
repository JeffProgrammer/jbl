//------------------------------------------------------------------------------
// lib.hpp
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

#ifndef _JBL_LIB_HPP_
#define _JBL_LIB_HPP_

#include <string.h>
#include "jbl/compiler.h"
#include "jbl/types.h"

template<typename T>
FORCE_INLINE T&& move_cast(T &ref)
{
	return static_cast<T&&>(ref);
}

template<typename T>
FORCE_INLINE T max(T a, T b)
{
	return (a > b) ? a : b;
}

template<typename T>
FORCE_INLINE T min(T a, T b)
{
	return (a < b) ? a : b;
}

// Equals override.
template<typename T> FORCE_INLINE bool equals(const T &lhs, const T &rhs);

template<typename T>
FORCE_INLINE bool equals(const T &lhs, const T &rhs)
{
	return lhs == rhs;
}

template<>
FORCE_INLINE bool equals(const char* const &lhs, const char *const &rhs)
{
	return strcmp(lhs, rhs) == 0;
}

#endif // _JBL_LIB_H_
