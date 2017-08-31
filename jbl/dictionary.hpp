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
#include "typetraits.hpp"
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
		Cell *previous = nullptr;
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
public:
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
		static_assert(!TypeTraits::IsSame<DictionaryKey, const char*>::value, "You cannot use const char* as a type for your dictionary key type! Please use String instead.");
		static_assert(!TypeTraits::IsSame<DictionaryValue, const char*>::value, "You cannot use const char* as a type for your dictionary value type! Please use String instead.");

		mTableSize = bucketSize;
		mTable = static_cast<TableCell*>(calloc(bucketSize, sizeof(TableCell)));
	}

	Dictionary(const Dictionary &) = delete;
	Dictionary& operator=(const Dictionary &) = delete;

	Dictionary(Dictionary &&dict)
	{
		mTableSize = dict.mTableSize;
		mTable = dict.mTable;
		mPool = move_cast(dict.mPool);

		dict.mTable = nullptr;
	}

	~Dictionary()
	{
		free(mTable);
		mTable = nullptr;
	}

	Dictionary& operator=(Dictionary &&dict)
	{
		if (this != &dict)
		{
			// Free old contents of the table
			free(mTable);

			mTableSize = dict.mTableSize;
			mTable = dict.mTable;
			mPool = move_cast(dict.mPool);

			dict.mTable = nullptr;
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
				newCell->previous = cell;

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
			newCell->previous = cell;

			cell->next = newCell;
		}
	}

	Iterator find(const DictionaryKey &key)
	{
		size_t hash = hashWithTableSize(key);
		for (Iterator iter = Iterator(this, hash); iter != end(); ++iter)
		{
			if (equals((*iter).key, key))
				return iter;
		}
		return end();
	}

	CIterator find(const DictionaryKey &key) const
	{
		size_t hash = hashWithTableSize(key);
		for (CIterator iter = CIterator(this, hash); iter != end(); ++iter)
		{
			if (equals((*iter).key, key))
				return iter;
		}
		return end();
	}

	Iterator erase(Iterator iterator)
	{
		// Note: Erase doesn't actually 'free' the memory block of
		// the cell. It just removes the pointer and leaves it dangling.
		// The memory will be reclaimed once the dictionary is destroyed.
		/*
		Cell *currentCell = iterator.mCurrentCell;
		Cell *nextCell = currentCell->next;
		Cell *previousCell = currentCell->previous;

		if (previousCell == nullptr)
		{
			// This means that currentCell is a table cell.

			if (nextCell == nullptr)
			{
				// No next cell, we were the only cell in the table slot.
				// mark it as empty...
				static_cast<TableCell*>(currentCell)->hasData = false;
			}
			else
			{
				// Go ahead and move nextCell into currentCell.
				currentCell->key = nextCell->key;
				currentCell->vaue = nextCell->value;
				currentCell->next = nextCell->next;
				currentCell->previous = nullptr;
				static_cast<TableCell*>(currentCell)->hasData = true;
			}
		}
		else
		{
			if (nextCell == nullptr)
				previousCell->next = nullptr;
			else
				previousCell->next = nextCell;
		}
		
		// TODO: adjust iterator and return it.
		*/
		return Iterator(this, 0);
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