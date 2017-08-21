//-----------------------------------------------------------------------------
// dictionary.hpp
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

#ifndef _JBL_DICTIONARY_HPP_
#define _JBL_DICTIONARY_HPP_

// Exception: These are compile time and do not link with the code. 
// Thus I will allow it.
#include <type_traits>

#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "compiler.h"

template<typename T>
FORCE_INLINE size_t HashFunction(const T &ref);

template<typename T>
FORCE_INLINE size_t HashFunction(const std::enable_if<std::is_arithmetic<T>, T>::type &ref)
{
	// Primitive type hashing. Just return the value.
	return ref;
}

template<typename T, S32 MODULUS_SIZE = 32>
FORCE_INLINE size_t HashFunction(const std::enable_if<std::is_pointer<T>, T>::type &ref)
{
	// Pointer hashing. Just return the numeric address.
	return reinterpret_cast<size_t>(ref);
}

template<>
FORCE_INLINE size_t HashFunction(const char* const &ref)
{
	// string hashing. Use 32bit FNV-1a algorithm. The algorithm is in the public domain.

	constexpr U32 offset_basis = 2166136261;
	constexpr U32 FNV_prime = 16777619;

	U32 hash = offset_basis;
	size_t length = strlen(ref);
	for (size_t i = 0; i < length; ++i)
	{
		hash = hash ^ static_cast<size_t>(ref[i]);
		hash = hash * FNV_prime;
	}
	return static_cast<size_t>(hash);
}

template<typename Key, typename Value, S32 TABLE_SIZE = 32>
class Dictionary
{
private:
	struct Cell
	{
		struct Data
		{
			Key key;
			Value value;
		};
		Data *data;
		S32 length;
		S32 capacity;
	};

	constexpr S32 MAX_STACK_TABLE_SIZE = 32;

	constexpr bool isHeapAllocated()
	{
		return TABLE_SIZE > MAX_STACK_TABLE_SIZE;
	}

	typedef std::conditional<!isHeapAllocated(), Cell[MAX_STACK_TABLE_SIZE], Cell*> CellType;

	// Equals override.
	template<typename T> FORCE_INLINE bool equals(const T &lhs, const T &rhs);

	template<typename T>
	FORCE_INLINE bool Dictionary::equals(const T &lhs, const T &rhs)
	{
		return lhs == rhs;
	}

	template<>
	FORCE_INLINE bool Dictionary::equals(const char* const &lhs, const char *const &rhs)
	{
		return strcmp(lhs, rhs) == 0;
	}

	template<typename T>
	FORCE_INLINE size_t hashWithTableSize(const T &ref)
	{
		return HashFunction(ref) % TABLE_SIZE;
	}

public:
	Dictionary()
	{
		if constexpr (isHeapAllocated())
		{
			mTable = calloc(TABLE_SIZE, sizeof(Cell::Data*));
		}

		for (S32 i = 0; i < TABLE_SIZE; ++i)
		{
			mTable[i].data = calloc(2, sizeof(Cell::Data));
			mTable[i].length = 0;
			mTable[i].capacity = 2;
		}
	}

	~Dictionary()
	{
		for (S32 i = 0; i < TABLE_SIZE; ++i)
			free(mTable[i].data);

		// Only free if we are heap allocated.
		if constexpr (isHeapAllocated())
		{
			free(mTable);
		}
	}

	Value& operator=(const Key &key)
	{
		// Yes, I know this isn't O(1), but its still faster than doing a linear search
		// over the entire data set. If there's only 1 in the 'bucket' then it is O(1)

		size_t hash = hashWithTableSize(key);
		for (const auto &kv : mTable[hash])
		{
			if (equals(key, kv.key))
				return kv.value;
		}
		return 0x0; // works for all types.
	}

	void insert(const Key &key, const Value &value)
	{
		size_t hash = hashWithTableSize(key);
		auto &ref = mTable[hash];

		if (ref.length == ref.capacity)
		{
			// Go ahead and up the capacity. grow by 1.5x.
			ref.capacity = static_cast<S32>(ref.capacity * 1.5f);
			ref.data = realloc(ref.data, ref.capacity * sizeof(data));
		}

		ref.data[ref.length].key = key;
		ref.data[ref.length].value = value;
		++ref.length;
	}

private:
	CellType mTable;
};

#endif // _JBL_DICTIONARY_HPP_