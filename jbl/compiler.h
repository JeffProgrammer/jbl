//-----------------------------------------------------------------------------
// compiler.h
//
// Copyright(c) 2016, Jeff Hutchinson
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and / or other materials provided with the distribution.
//
// * Neither the name of VoxelGame nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef _JBL_COMPILER_H_
#define _JBL_COMPILER_H_

#ifdef _MSC_VER
	// We support compiler intrinsics on Microsoft's compilers.
	#include <intrin.h>
	#define SSE_INTRINSICS

	// Macro to force alignment
	#define ALIGN(size) __declspec(align(size))

	// Macro to force inline a function
	#define FORCE_INLINE __forceinline
#elif defined(__llvm__) || defined(__GNU_C__)
	// Only X86 architecture provides support for SSE registers.
	#if defined(__i386__) || defined(__x86_64__)
		// We support compiler intrinsics on x86 and x64 architecture.
		#include <emmintrin.h>
		#define SSE_INTRINSICS
	#endif

	// Macro to force alignment
	#define ALIGN(size) __attribute__((assumed_align(size)))

	// Macro to force inline on a function
	#define FORCE_INLINE __attribute__((always_inline))
#else
	#error "Please implement these macros for your compiler."
#endif

#endif // _JBL_COMPILER_H_