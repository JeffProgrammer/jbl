//------------------------------------------------------------------------------
// vector.h
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

#ifndef _JBL_VECTOR_H_
#define _JBL_VECTOR_H_

#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "lib.hpp"

/**
 * Implements a contiguous array that will automatically grow in size
 * as needed. It can contain up to a maximum of 2^32 elements.
 * It's growth is a logrithmic allocation based on powers of 2.
 * It is also possible to reserve the size at vector creation. This is so that
 */
template<typename T>
class Vector
{
public:
	/**
	 * A class that is responsible for iterating over a Vector.
	 * It performs forward iteration at O(n) time.
	 * @see CIterator
	 */
	class Iterator
	{
		friend class Vector<T>;
	public:
		/**
		 * Constructs an iterator.
		 * @param ptr A pointer to the array.
		 * @param count The amount of elements in the array.
		 * @param position The starting position of the iterator.
		 */
		Iterator(T *ptr, S32 count, S32 position)
		{
			mCount = count;
			mPosition = position;
			mPtr = ptr;
		}
		
		/**
		 * Incriments the iterator to advance it to access the next element.
		 * @return The current iterator.
		 */
		Iterator& operator++()
		{
			++mPosition;
			return *this;
		}
		
		/**
		 * Compares if two iterators are at the same position.
		 * @param it The other iterator to check.
		 * @return true if both iterators are at the same position, false
		 *  otherwise.
		 */
		bool operator!=(const Iterator &it) const
		{
			return mPosition != it.mPosition;
		}
		
		/**
		 * Dereferences the current element at the current iterator position.
		 * @return The dereferenced element at the current iterator position.
		 */
		T& operator*() const
		{
			assert(mPosition != mCount);
			return mPtr[mPosition];
		}
	private:
		/**
		 * The amount of elemens within the array.
		 */
		S32 mCount;
		
		/**
		 * The position of the iterator.
		 */
		S32 mPosition;
		
		/**
		 * A pointer to the array.
		 */
		T *mPtr;
	};
	
	/**
	 * A class that is responsible for iterating over a Vector.
	 * It performs forward iteration at O(n) time.
	 * Unlike Iterator, this version is a constant iterator over the Vector.
	 * @see Iterator
	 */
	class CIterator
	{
		friend class Vector<T>;
	public:
		/**
		 * Constructs a constant iterator.
		 * @param ptr A pointer to the array.
		 * @param count The amount of elements in the array.
		 * @param position The starting position of the iterator.
		 */
		CIterator(T *ptr, S32 count, S32 position)
		{
			mCount = count;
			mPosition = position;
			mPtr = ptr;
		}
		
		/**
		 * Incriments the constant iterator to advance it to access the next element.
		 * @return The current constant iterator.
		 */
		const CIterator& operator++()
		{
			++mPosition;
			return *this;
		}
		
		/**
		 * Compares if two constant iterators are at the same position.
		 * @param it The other constant iterator to check.
		 * @return true if both constant iterators are at the same position, false otherwise.
		 */
		const bool operator!=(const CIterator &it) const
		{
			return mPosition != it.mPosition;
		}
		
		/**
		 * Dereferences the current element at the current constant iterator position.
		 * @return The dereferenced element at the current constant iterator position.
		 */
		const T& operator*() const
		{
			assert(mPosition != mCount);
			return mPtr[mPosition];
		}
	private:
		/**
		 * The amount of elemens within the array.
		 */
		S32 mCount;
		
		/**
		 * The position of the iterator.
		 */
		S32 mPosition;
		
		/**
		 * A pointer to the array.
		 */
		T *mPtr;
	};
	
	/**
	 * Creates a vector of type T.
	 */
	Vector()
	{
		mArray = nullptr;
		mAllocSize = 0;
		mCount = 0;
	}
	
	/**
	 * Creates a vector of type T with an initial capacity.
	 * @param capacity The capacity of the vector.
	 */
	Vector(S32 capacity)
	{
		mArray = reinterpret_cast<T*>(calloc(capacity, sizeof(T)));
		mAllocSize = capacity;
		mCount = 0;
	}
	
	/**
	 * Frees the memory that is allocated within the vector.
	 */
	~Vector()
	{
		if (mArray != nullptr)
			free(mArray);
	}

	Vector(const Vector &cpy)
	{
		mArray = reinterpret_cast<T*>(calloc(cpy.mAllocSize, sizeof(T)));
		memcpy(mArray, cpy.mArray, sizeof(T) * cpy.mCount);
		
		mAllocSize = cpy.mAllocSize;
		mCount = cpy.mCount;
	}
	
	Vector(Vector &&ref)
	{
		mArray = ref.mArray;
		mAllocSize = ref.mAllocSize;
		mCount = ref.mCount;
		
		ref.mArray = nullptr;
		ref.mAllocSize = 0;
		ref.mCount = 0;
	}
	
	Vector& operator=(Vector &&ref)
	{
		if (this != &ref)
		{
			if (mArray != nullptr)
			{
				// free our memory as we are being move assigned
				free(mArray);
			}
			
			mArray = ref.mArray;
			mAllocSize = ref.mAllocSize;
			mCount = ref.mCount;
			
			ref.mArray = nullptr;
			ref.mAllocSize = 0;
			ref.mCount = 0;
		}
		return *this;
	}
	
	/**
	 * Adds an element to the Vector. If there is not enough space,
	 * more space will be allocated automatically behind the scenes.
	 * @param item The item to add to the vector.
	 */
	inline void add(T item)
	{
		if (mCount == mAllocSize)
			expand();
		mArray[mCount++] = item;
	}
	
	/**
	 * Grabs the amount of elements within the Vector.
	 * @return The amount of elements in the vector.
	 */
	inline S32 count() const
	{
		return mCount;
	}
	
	/**
	 * Gets an element at the specified index.
	 * @param index The location of the element.
	 * @return The element at the specified index.
	 */
	inline T operator[](S32 index) const
	{
		assert(index >= 0 && index < mCount);
		return mArray[index];
	}
	
	/**
	 * Checks to see if the vector contains the element.
	 * @param item The item to see if it exists within the vector.
	 * @return true if the element is within the vector, false otherwise.
	 */
	inline bool contains(const T &item) const
	{
		for (S32 i = 0; i < mCount; ++i)
		{
			if (equals(mArray[i], item))
				return true;
		}
		return false;
	}

	Iterator find(const T &item)
	{
		for (S32 i = 0; i < mCount; ++i)
		{
			if (equals(mArray[i], item))
			{
				return Iterator(mArray, mCount, i);
			}
		}
		return end();
	}

	Iterator erase(const Iterator &iterator)
	{
		// shift everything down by 1 from that position to keep the array compact.
		S32 i = iterator.mPosition;
		--mCount;
		memmove(mArray + i, mArray + i + 1, (mCount - i) * sizeof(T));
		return Iterator(mArray, mCount, i - 1);
	}
	
	/**
	 * Removes an item out of the array.
	 * @param item The item to remove within the vector.
	 * @return true if the item was removed, false otherwise.
	 */
	inline bool remove(const T &item)
	{
		for (S32 i = 0; i < mCount; ++i)
		{
			if (equals(mArray[i], item))
			{
				// shift everything down by 1 from that position to keep the array compact.
				--mCount;
				memmove(mArray + i, mArray + i + 1, (mCount - i) * sizeof(T));
				return true;
			}
		}
		return false;
	}
	
	/**
	 * Removes all items within the vector that is equal to the item provided.
	 * @param item the item to remove within the vector.
	 * @return true if at least one item was removed from the vector, false
	 *  otherwise.
	 */
	inline bool removeAll(const T &item)
	{
		bool hasRemoved = false;
		bool removed = false;
		while (true)
		{
			// We write this separatly for the potential of remove() being
			// allowed to be inline instead of doing it as:
			//    while (remove(item)) removed = true;
			hasRemoved = remove(item);
			if (!hasRemoved)
				break;
			
			// We have removed at least one item, so it was successful.
			removed = true;
		}
		return removed;
	}
	
	/**
	 * Grabs an iterator at the beginning of the Vector.
	 * @return an iterator at the beginning of the Vector.
	 */
	inline Iterator begin()
	{
		return Iterator(mArray, mCount, 0);
	}
	
	/**
	 * Grabs an iterator at the end of the vector.
	 * @return an iterator at the end of the vector.
	 */
	inline Iterator end()
	{
		return Iterator(mArray, mCount, mCount);
	}
	
	/**
	 * Grabs a constant iterator at the beginning of the vector.
	 * @return a constant iterator at the beginning of the vector.
	 */
	inline CIterator begin() const
	{
		return CIterator(mArray, mCount, 0);
	}
	
	/**
	 * Grabs a constant iterator at the end of the vector.
	 * @return a constant iterator at the end of the vector.
	 */
	inline CIterator end() const
	{
		return CIterator(mArray, mCount, mCount);
	}
	
private:
	/**
	 * The array that contains the elements.
	 */
	T *mArray;
	
	/**
	 * The amount of elements within the vector.
	 */
	S32 mCount;
	
	/**
	 * The capacity of the vector.
	 */
	S32 mAllocSize;
	
	/**
	 * Expands the capacity of the array so that it automatically has
	 * enough space. It is 1.5x size growth
	 */
	void expand()
	{
		mAllocSize = mCeil(mMax(1, mAllocSize) * 1.5f);

		mArray = reinterpret_cast<T*>(realloc(mArray, mAllocSize * sizeof(T)));
		
		// we could potentially be allocating a LOT of memory. Check to make sure
		// the allocation was successful.
		if (mArray == nullptr)
		{
			// TODO: Implement some message box in here to say we are out of
			// memory.
			exit(-1);
		}
	}
};

#endif // _JBL_VECTOR_H_
