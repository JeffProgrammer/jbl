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
#include "types.h"
#include "compiler.h"
#include "string.hpp"
#include "memoryChunker.hpp"

template<typename T>
FORCE_INLINE size_t HashFunction(const T &ref);

template<typename T>
FORCE_INLINE size_t HashFunction(const T &ref)
{
	// Pointers just take the address.
	// Primitive types just return.
	if constexpr (std::is_pointer<T>>::value)
		return reinterpret_cast<size_t>(ref);
	else
		return ref;
}

template<>
FORCE_INLINE size_t HashFunction(const String &ref)
{
	// string hashing. Use 32bit FNV-1a algorithm. The algorithm is in the public domain.

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

template<typename Key, typename Value, S32 TABLE_SIZE = 32>
class Dictionary
{
private:
	struct Cell
	{
		Key key;
		Value value;

		Cell *next = nullptr;
	};

	static constexpr S32 MAX_STACK_TABLE_SIZE = 32;

	static constexpr bool isHeapAllocated()
	{
		return TABLE_SIZE > MAX_STACK_TABLE_SIZE;
	}

	typedef typename std::conditional<(TABLE_SIZE > MAX_STACK_TABLE_SIZE), Cell*, Cell[MAX_STACK_TABLE_SIZE]> CellType;

	template<typename T>
	FORCE_INLINE size_t hashWithTableSize(const T &ref)
	{
		return HashFunction(ref) % static_cast<size_t>(TABLE_SIZE);
	}

public:
	Dictionary()
	{
		static_assert(!std::is_same<Key, const char*>::value, "You cannot use const char* as a type for your dictionary key type! Please use String instead.");
		static_assert(!std::is_same<Value, const char*>::value, "You cannot use const char* as a type for your dictionary value type! Please use String instead.");

		if constexpr (isHeapAllocated())
		{
			mTable = calloc(TABLE_SIZE, sizeof(Cell*));
		}
	}

	~Dictionary()
	{
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

		for (auto &kv = mTable[hash]; kv != nullptr; kv = kv->next)
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

		Cell *newCell = mPool.alloc(sizeof(Cell));
		
		newCell->key = key;
		newCell->value = value;

		// Attach to end of list
		Cell *toAttach = ref;
		while (toAttach)
			toAttach = toAttach->next;
		toAttach->next = newCell;
	}

private:
	CellType mTable;

	MemoryChunker<Cell> mPool;
};


#endif // _JBL_DICTIONARY_HPP_