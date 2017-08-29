//-----------------------------------------------------------------------------
// hashFunction.hpp
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

#ifndef _JBL_HASHFUNCTION_HPP_
#define _JBL_HASHFUNCTION_HPP_

#include "lib.hpp"
#include "string.hpp"

template<class T>
class HashFunction;

/// Helper function to hashing variables.
/// This is intended for public use.
template<typename T>
FORCE_INLINE size_t Hash(const T &t)
{
	HashFunction<T> b;
	return b(t);
}

template<class T>
class HashFunction<T*>
{
public:
	FORCE_INLINE size_t operator()(T *ref)
	{
		// Pointers just take the address.
		return reinterpret_cast<size_t>(ref);
	}
};

template<>
class HashFunction<String>
{
public:
	size_t operator()(String ref) const
	{
		// string hashing. Use 32bit FNV-1a algorithm. 
		// The algorithm is in the public domain.

		constexpr U32 offset_basis = 2166136261;
		constexpr U32 FNV_prime = 16777619;

		U32 hash = offset_basis;
		S32 length = ref.length();
		for (S32 i = 0; i < length; ++i)
		{
			hash = hash ^ static_cast<size_t>(ref[i]);
			hash = hash * FNV_prime;
		}
		return static_cast<size_t>(hash);
	}
};

#define IMPLEMENT_HASH_FUNCTION_PRIMITIVE(type) \
template<>                                      \
class HashFunction<type>                        \
{                                               \
public:                                         \
	FORCE_INLINE size_t operator()(type ref)    \
    {                                           \
		/* Primitive types just return. */      \
		return static_cast<size_t>(ref);        \
	}                                           \
}

IMPLEMENT_HASH_FUNCTION_PRIMITIVE(bool);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(S8);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(S16);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(S32);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(S64);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(U8);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(U16);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(U32);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(U64);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(F32);
IMPLEMENT_HASH_FUNCTION_PRIMITIVE(F64);

#endif // _JBL_HASHFUNCTION_HPP_