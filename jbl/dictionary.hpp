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
#include "memoryChunker.hpp"
#include "hashFunction.hpp"

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
		Hash hash;
		return hash(ref) % static_cast<size_t>(mTableSize);
	}

public:
	struct KVPair
	{
		DictionaryKey key;
		DictionaryValue value;
	};

	/// A class that is responsible for iterating over a Dictionary.
	/// It performs forward iteration at O(n) time.
	/// @see CIterator
	class Iterator
	{
	public:
		Iterator(Dictionary *dictionary, S32 tablePosStart)
		{
			mDictionary = dictionary;
			mCurrentCell = nullptr;

			// Find first tablecell within the table that has something
			// If the table is empty, mTablePos will simply incriment to
			// the size of the table, thus making it empty and the same as end
			// condition.
			mTablePos = tablePosStart;
			findNextCell();
		}

		/// Incriments the iterator to advance it to access the next element.
		/// @return The current iterator.
		Iterator& operator++()
		{
			// If the current cell is nullptr assert.
			assert(mCurrentCell != nullptr);

			if (mCurrentCell->next != nullptr)
			{
				// Just grab the next one on the chain for this bucket.
				mCurrentCell = mCurrentCell->next;
			}
			else
			{
				// We need to grab the next cell!
				mTablePos++;
				findNextCell();
			}
			return *this;
		}
		
		/// Compares if two iterators are at the same position.
		/// @param it The other iterator to check.
		/// @return true if both iterators are at the same position, false
		///  otherwise.
		bool operator!=(const Iterator &it) const
		{
			// Both dictionaries must be the same dictionary
			// between iterators!
			assert(mDictionary == it.mDictionary);

			return mTablePos != it.mTablePos;
		}

		/// Dereferences the current element at the current iterator position.
		/// @return The key/value pair of the dictionary at the current position.
		KVPair operator*() const
		{
			assert(mCurrentCell != nullptr);

			KVPair pair;
			pair.key = mCurrentCell->key;
			pair.value = mCurrentCell->value;
			return pair;
		}
	private:
		Dictionary *mDictionary;
		S32 mTablePos;
		Cell *mCurrentCell;

		/// Advances the iterator to the next cell when it has to jump
		/// to another bucket.
		void findNextCell()
		{
			for (; mTablePos < mDictionary->mTableSize; ++mTablePos)
			{
				TableCell *cell = static_cast<TableCell*>(&mDictionary->mTable[mTablePos]);
				if (cell->hasData)
				{
					mCurrentCell = static_cast<Cell*>(cell);
					break;
				}
			}
		}
	};

	/// A class that is responsible for iterating over a Dictionary.
	/// It performs forward iteration at O(n) time.
	/// Unlike Iterator, this version is a constant iterator over the Dictionary.
	/// @see Iterator
	class CIterator
	{
	public:
		CIterator(Dictionary *dictionary, S32 tablePosStart)
		{
			mDictionary = dictionary;
			mCurrentCell = nullptr;

			// Find first tablecell within the table that has something
			// If the table is empty, mTablePos will simply incriment to
			// the size of the table, thus making it empty and the same as end
			// condition.
			mTablePos = tablePosStart;
			findNextCell();
		}

		/// Incriments the iterator to advance it to access the next element.
		/// @return The current iterator.
		const CIterator& operator++()
		{
			// If the current cell is nullptr assert.
			assert(mCurrentCell != nullptr);

			if (mCurrentCell->next != nullptr)
			{
				// Just grab the next one on the chain for this bucket.
				mCurrentCell = mCurrentCell->next;
			}
			else
			{
				// We need to grab the next cell!
				mTablePos++;
				findNextCell();
			}
			return *this;
		}

		/// Compares if two iterators are at the same position.
		/// @param it The other iterator to check.
		/// @return true if both iterators are at the same position, false
		///  otherwise.
		bool operator!=(const CIterator &it) const
		{
			// Both dictionaries must be the same dictionary
			// between iterators!
			assert(mDictionary == it.mDictionary);

			return mTablePos != it.mTablePos;
		}

		/// Dereferences the current element at the current iterator position.
		/// @return The key/value pair of the dictionary at the current position.
		const KVPair& operator*() const
		{
			assert(mCurrentCell != nullptr);

			KVPair pair;
			pair.key = mCurrentCell->key;
			pair.value = mCurrentCell->value;
			return pair;
		}
	private:
		Dictionary *mDictionary;
		S32 mTablePos;
		Cell *mCurrentCell;

		/// Advances the iterator to the next cell when it has to jump
		/// to another bucket.
		void findNextCell()
		{
			for (; mTablePos < mDictionary->mTableSize; ++mTablePos)
			{
				TableCell *cell = static_cast<TableCell*>(&mDictionary->mTable[mTablePos]);
				if (cell->hasData)
				{
					mCurrentCell = static_cast<Cell*>(cell);
					break;
				}
			}
		}
	};

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

	/// Grabs an iterator at the beginning of the Dictionary.
	/// @return an iterator at the beginning of the Dictionary.
	FORCE_INLINE Iterator begin()
	{
		return Iterator(this, 0);
	}

	/// Grabs an iterator at the end of the Dictionary.
	/// @return an iterator at the end of the Dictionary.
	FORCE_INLINE Iterator end()
	{
		return Iterator(this, mTableSize);
	}

	/// Grabs a constant iterator at the beginning of the Dictionary.
	/// @return a constant iterator at the beginning of the Dictionary.
	FORCE_INLINE CIterator begin() const
	{
		return CIterator(this, 0);
	}

	/// Grabs a constant iterator at the end of the Dictionary.
	/// @return a constant iterator at the end of the Dictionary.
	FORCE_INLINE CIterator end() const
	{
		return CIterator(this, mTableSize);
	}

private:
	TableCell* mTable;
	size_t mTableSize;
	MemoryChunker<Cell> mPool;
};

#endif // _JBL_DICTIONARY_HPP_