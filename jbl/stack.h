//------------------------------------------------------------------------------
// stack.h
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

#ifndef _JBL_STACK_H_
#define _JBL_STACK_H_

#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "jbl/types.h"

/**
 * Implements a generic stack. The stack will grow but never shrink in size,
 * so be careful and use it wisely. It's growth is a constant growth defined
 * by the constant STACK_CHUNK_SIZE. You can also initially set the size of
 * the stack within one of the constructors. When an item is popped from the
 * stack, the slot will keep the memory in it and treat it as garbage instead
 * of zeroing it out. If you need to zero it out for security reasons, please
 * use the popZeroMem function.
 */
template<typename T>
class Stack
{
public:
	/**
	 * A constant that is used when expanding the size of the stack to give it
	 * constant growth.
	 */
	const U32 STACK_CHUNK_SIZE = 20;

	/**
	 * Creates a stack with an initial size defined by the constant 
	 * STACK_CHUNK_SIZE.
	 */
	Stack() : mCount(0), mCapacity(STACK_CHUNK_SIZE)
	{
		mArray = reinterpret_cast<T*>(calloc(STACK_CHUNK_SIZE, sizeof(T)));
	}

	/**
	 * Creates a stack with an initial size.
	 * @param reserve The capacity of the initial stack size.
	 * @note The stack will not grow every time by this amount. Instead it will
	 *  grow by as much as STACK_CHUNK_SIZE as needed.
	 */
	explicit Stack(const S32 reserve) : mCount(0), mCapacity(reserve)
	{
		mArray = reinterpret_cast<T*>(calloc(reserve, sizeof(T)));
	}
	
	Stack(const Stack &cpy)
	{
		mArray = reinterpret_cast<T*>(calloc(cpy.mCapacity, sizeof(T)));
		memcpy(mArray, cpy.mArray, sizeof(T) * cpy.mCount);
		
		mCount = cpy.mCount;
		mCapacity = cpy.mCapacity;
	}
	
	Stack(Stack &&ref)
	{
		mArray = ref.mArray;
		mCount = ref.mCount;
		mCapacity = ref.mCapacity;
		
		ref.mArray = nullptr;
		ref.mCount = 0;
		ref.mCapacity = 0;
	}

	~Stack()
	{
		if (mArray != nullptr)
			free(mArray);
	}
	
	Stack& operator=(Stack &&ref)
	{
		if (this != &ref)
		{
			if (mArray)
				free(mArray);
			
			mArray = ref.mArray;
			mCount = ref.mCount;
			mCapacity = ref.mCapacity;
			
			ref.mArray = nullptr;
			ref.mCount = 0;
			ref.mCapacity = 0;
		}
		return *this;
	}

	/**
	 * Pushes an item onto the top of the stack.
	 * @param item The item to push on the stack.
	 */
	void push(const T &item)
	{
		if (mCount == mCapacity)
			expand();
		mArray[mCount++] = item;
	}

	/**
	 * Pops an item off of the stack.
	 * @note This will not free dynamically allocated array. You are responsable
	 *  for managing that memory yourself by freeing the memory returned from
	 *  using the getTop function.
	 * @note The memory will not be zerodd out and will be treated as garbage. If
	 *  you want the memory to be zeroed out for security purposes, please use the
	 *  popZeroMem function.
	 * @see popZeroMem, getTop
	 */
	inline void pop()
	{
		assert(mCount);
		--mCount;
	}

	/**
	 * Pops an item off of the stack and zero's the memory out of the stack region.
	 * @note This will not free dynamically allocated array. You are responsable
	 *  for managing that memory yourself by freeing the memory returned from
	 *  using the getTop function.
	 * @note The memory will zero out for security purposes. This is a potentially
	 *  slower operation. If speed is the concern over security, consider using
	 *  pop method only.
	 * @see pop, getTop
	 */
	inline void popZeroMem()
	{
		assert(mCount);
		--mCount;
		memset(mArray + mCount, 0, sizeof(T));
	}

	/**
	 * Gets the element at the top of the stack.
	 * @return the element at the top of the stack.
	 * @note This does not pop the item off of the stack. You have to do that
	 *  yourself.
	 * @see pop, popZeroMem
	 */
	inline T& getTop() const
	{
		assert(mCount);
		return mArray[mCount - 1];
	}

	/**
	 * Checks to see if the stack is empty.
	 * @return true if the stack is empty, false otherwise.
	 */
	inline bool isEmpty() const
	{
		return mCount == 0;
	}

	/**
	 * Gets the count of how many items there are on the stack.
	 * @return the amount of items on the stack.
	 */
	inline S32 getCount() const
	{
		return mCount;
	}

private:
	/**
	 * The memory region of the stack storage.
	 */
	T *mArray;

	/**
	 * The amount of items that are currently in the stack.
	 */
	S32 mCount;

	/**
	 * The capacity of the stack storage region.
	 */
	S32 mCapacity;

	/**
	 * Expands the stack region when it runs out of space. The stack size will
	 * grow by the STACK_CHUNK_SIZE constant.
	 */
	void expand()
	{
		mCapacity += STACK_CHUNK_SIZE;
		mArray = reinterpret_cast<T*>(realloc(mArray, sizeof(T) * mCapacity));

		if (mArray == nullptr)
			exit(-1);
	}
};

#endif // _JBL_STACK_H_
