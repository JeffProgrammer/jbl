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

#include <stdlib.h>
#include "lib.hpp"
#include "string.hpp"
#include "memoryChunker.hpp"

template<class T>
struct HashFunction;

template<class T>
struct HashFunction<T*>
{
	FORCE_INLINE size_t operator()(T *ref)
	{
		// Pointers just take the address.
		return reinterpret_cast<size_t>(ref);
	}
};

#define IMPLEMENT_HASH_FUNCTION_PRIMITIVE(type) \
template<>                                      \
struct HashFunction<type>                       \
{                                               \
	FORCE_INLINE size_t operator()(type ref)                 \
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

template<>
struct HashFunction<String>
{
	size_t operator()(String ref) const
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
};

template<typename DictionaryKey, typename DictionaryValue, class Hash = HashFunction<DictionaryKey>>
class Dictionary
{
private:
	struct Cell
	{
		DictionaryKey key;
		DictionaryValue value;

		Cell *next = nullptr;
	};

	struct TableCell : Cell
	{
		bool hasData = false;
	};

	template<typename T>
	FORCE_INLINE size_t hashWithTableSize(T &ref)
	{
		Hash b;
		return b(ref) % static_cast<size_t>(mTableSize);
	}

public:
	explicit Dictionary(S32 bucketSize)
	{
		static_assert(!std::is_same<DictionaryKey, const char*>::value, "You cannot use const char* as a type for your dictionary key type! Please use String instead.");
		static_assert(!std::is_same<DictionaryValue, const char*>::value, "You cannot use const char* as a type for your dictionary value type! Please use String instead.");

		mTableSize = bucketSize;
		mTable = static_cast<TableCell*>(calloc(bucketSize, sizeof(TableCell)));
	}

	Dictionary(const Dictionary &) = delete;
	Dictionary& operator=(const Dictionary &) = delete;

	Dictionary(Dictionary &&dict)
	{
		mTableSize = dict.mTableSize;
		mTable = ref.mTable;
		mPool = move_cast(ref.mPool);

		ref.mTable = nullptr;
	}

	~Dictionary()
	{
		free(mTable);
		mTable = nullptr;
	}

	Dictionary& operator=(Dictionary &&ref)
	{
		if (this != &ref)
		{
			// Free old contents of the table if the table was heap allocated.
			free(mTable);

			move(ref);
		}
		return *this;
	}

	DictionaryValue& operator[](const DictionaryKey &key)
	{
		// Yes, I know this isn't O(1), but its still faster than doing a linear search
		// over the entire data set. If there's only 1 in the 'bucket' then it is O(1)

		size_t hash = hashWithTableSize(key);
		for (Cell *kv = static_cast<Cell*>(&mTable[hash]); kv != nullptr; kv = kv->next)
		{
			if (equals(key, kv->key))
				return kv->value;
		}

		// Perform an insertion of the key with blank value
		{
			TableCell *tableCell = &mTable[hash];

			// first cell is always a TableCell not a Cell.
			if (!tableCell->hasData)
			{
				// First data hasn't been filled in.
				tableCell->key = key;
				tableCell->hasData = true;
				return tableCell->value;
			}
			else
			{
				// Linked list chain.
				Cell *cell = static_cast<Cell*>(tableCell);
				while (cell->next != nullptr)
					cell = cell->next;

				// Create next cell for next insert.
				Cell *newCell = mPool.alloc(1);
				newCell->key = key;

				cell->next = newCell;
				return newCell->value;
			}
		}
	}

	void insert(const DictionaryKey &key, const DictionaryValue &value)
	{
		size_t hash = hashWithTableSize(key);
		TableCell *tableCell = &mTable[hash];

		// first cell is always a TableCell not a Cell.
		if (!tableCell->hasData)
		{
			// First data hasn't been filled in.
			tableCell->key = key;
			tableCell->value = value;
			tableCell->hasData = true;
		}
		else
		{
			// Linked list chain.
			Cell *cell = static_cast<Cell*>(tableCell);
			while (cell->next != nullptr)
				cell = cell->next;
			
			// Create next cell for next insert.
			Cell *newCell = mPool.alloc(1);
			newCell->key = key;
			newCell->value = value;

			cell->next = newCell;
		}
	}

private:
	TableCell* mTable;
	size_t mTableSize;
	MemoryChunker<Cell> mPool;
};

#endif // _JBL_DICTIONARY_HPP_