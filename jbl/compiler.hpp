//-----------------------------------------------------------------------------
// compiler.h
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

#ifndef _JBL_COMPILER_H_
#define _JBL_COMPILER_H_

#if defined(_DEBUG) || !defined(NDEBUG)
	#define DEBUG_BUILD
#endif

#ifdef _MSC_VER
	// We support compiler intrinsics on Microsoft's compilers.
	#include <intrin.h>
	#define SSE_INTRINSICS

	// Macro to force alignment
	#define ALIGN(size) __declspec(align(size))

	// Macro to force inline a function
	#define FORCE_INLINE __forceinline

	// Macro for 32bit vs 64bit
	#if defined(_M_X64)
		#define IS_64_BIT
	#else
		#define IS_32_BIT
	#endif
#elif defined(__llvm__) || defined(__GNUC__)
	// Only X86 architecture provides support for SSE registers.
	#if defined(__i386__) || defined(__x86_64__)
		// We support compiler intrinsics on x86 and x64 architecture.
		#include <emmintrin.h>
		#define SSE_INTRINSICS
	#endif

	// Macro to force alignment
	#define ALIGN(size) __attribute__((assumed_align(size)))

	// Macro to force inline on a function
	#define FORCE_INLINE __attribute__((always_inline)) inline

	// Macro for 32bit vs 64bit
	#if defined(__x86_64__) || defined(__ppc64__)
		#define IS_64_BIT
	#else
		#define IS_32_BIT
	#endif
#else
	#error "Please implement these macros for your compiler."
#endif

#endif // _JBL_COMPILER_H_
