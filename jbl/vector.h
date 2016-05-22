//------------------------------------------------------------------------------
// vector.h
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
// * Neither the name of jbl nor the names of its
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

#ifndef _JBL_VECTOR_H_
#define _JBL_VECTOR_H_

#include <stdlib.h>
#include <assert.h>
#include <memory.h>

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
	public:
		/**
		 * Constructs an iterator.
		 * @param ptr A pointer to the array.
		 * @param count The amount of elements in the array.
		 * @param position The starting position of the iterator.
		 */
		Iterator(T *ptr, unsigned int count, unsigned int position)
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
		unsigned int mCount;
		
		/**
		 * The position of the iterator.
		 */
		unsigned int mPosition;
		
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
	public:
		/**
		 * Constructs a constant iterator.
		 * @param ptr A pointer to the array.
		 * @param count The amount of elements in the array.
		 * @param position The starting position of the iterator.
		 */
		CIterator(T *ptr, unsigned int count, unsigned int position)
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
		unsigned int mCount;
		
		/**
		 * The position of the iterator.
		 */
		unsigned int mPosition;
		
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
	 * @param capacity A power of 2 initial capacity of the vector.
	 */
	Vector(unsigned int capacity)
	{
#ifndef NDEBUG
		// Check to make sure that capacity is a power of 2.
#endif
		
		mArray = reinterpret_cast<T*>(calloc(sizeof(T), capacity));
		mAllocSize = capacity;
		mCount = 0;
	}
	
	/**
	 * Frees the memory that is allocated within the vector.
	 */
	~Vector()
	{
		if (mArray)
			free(mArray);
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
	inline unsigned int count() const
	{
		return mCount;
	}
	
	/**
	 * Gets an element at the specified index.
	 * @param index The location of the element.
	 * @return The element at the specified index.
	 */
	inline T operator[](unsigned int index) const
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
		for (unsigned int i = 0; i < mCount; ++i)
		{
			if (mArray[i] == item)
				return true;
		}
		return false;
	}
	
	/**
	 * Removes an item out of the array.
	 * @param item The item to remove within the vector.
	 * @return true if the item was removed, false otherwise.
	 */
	inline bool remove(const T &item)
	{
		unsigned int position = 0xffffffff;
		for (unsigned i = 0; i < mCount; ++i)
		{
			if (mArray[i] == item)
			{
				position = i;
				break;
			}
		}
		if (position == 0xffffffff)
			return false;
		
		// TODO: optimize
		--mCount;
		for (unsigned int i = position; i < mCount; ++i)
		{
			mArray[i] = mArray[i+1];
		}
		return true;
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
	unsigned int mCount;
	
	/**
	 * The capacity of the vector.
	 */
	unsigned int mAllocSize;
	
	/**
	 * Expands the capacity of the array so that it automatically has
	 * enough space. It is logrithmic growth.
	 */
	void expand()
	{
		// if we already allocated 0xffffffff size of vector, we can't allocate more.
		assert((mAllocSize << 1) != 0xffffffff);
		
		mAllocSize <<= 1;
		mArray = reinterpret_cast<T*>(realloc(mArray, mAllocSize * sizeof(T)));
		
		// we could potentially be allocating a LOT of memory. Check to make sure
		// the allocation was successful.
		if (mArray == nullptr)
			exit(-1);
	}
};

#endif // _JBL_VECTOR_H_