//------------------------------------------------------------------------------
// stack.h
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
	explicit Stack(const U32 reserve) : mCount(0), mCapacity(reserve)
	{
		mArray = reinterpret_cast<T*>(calloc(reserve, sizeof(T)));
	}

	~Stack()
	{
		if (mArray)
			free(mArray);
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
	void pop()
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
	void popZeroMem()
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
	T& getTop() const
	{
		assert(mCount);
		return mArray[mCount - 1];
	}

	/**
	 * Checks to see if the stack is empty.
	 * @return true if the stack is empty, false otherwise.
	 */
	bool isEmpty() const
	{
		return mCount == 0;
	}

	/**
	 * Gets the count of how many items there are on the stack.
	 * @return the amount of items on the stack.
	 */
	U32 getCount() const
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
	U32 mCount;

	/**
	 * The capacity of the stack storage region.
	 */
	U32 mCapacity;

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
