//-----------------------------------------------------------------------------
// memoryChunker.hpp
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

#ifndef _JBL_MEMORYCHUNKER_H_
#define _JBL_MEMORYCHUNKER_H_

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "types.hpp"

enum class MemoryChunkerPageSize : U32
{
	e4096  = 4096,
	e8192  = 8192,
	e16384 = 16384
};

template<typename T, MemoryChunkerPageSize PAGE_SIZE = MemoryChunkerPageSize::e4096>
class MemoryChunker
{
private:
	static constexpr U32 getPageSize() { return static_cast<U32>(PAGE_SIZE); }
	static constexpr U32 getFreespace() { return getPageSize() - sizeof(S32) - sizeof(Page*); }

	struct Page
	{
		U8 memory[getFreespace()];
		S32 freespaceLeft;
		Page* next;

		Page()
		{
			memset(memory, 0, getFreespace());
			freespaceLeft = getFreespace();
			next = nullptr;
		}
	};

public:
	MemoryChunker() :
		startPage{nullptr},
		currentPage{nullptr} 
	{
		startPage = reinterpret_cast<Page*>(calloc(1, getPageSize()));
		currentPage = startPage;
	}

	MemoryChunker(const MemoryChunker &) = delete; // Copy constructor not allowed for MemoryChunker
	MemoryChunker& operator=(const MemoryChunker &) = delete; // Copy assignment not allowed for MemoryChunker

	MemoryChunker(MemoryChunker &&ref)
	{
		startPage = ref.startPage;
		currentPage = ref.currentPage;

		ref.startPage = nullptr;
		ref.currentPage = nullptr;
	}

	~MemoryChunker()
	{
		while (startPage)
		{
			Page *next = startPage->next;
			delete startPage;
			startPage = next;
		}
		startPage = nullptr;
	}

	MemoryChunker& operator=(MemoryChunker &&ref)
	{
		if (this != &ref)
		{
			// Free any current memory.
			while (startPage)
			{
				Page *next = startPage->next;
				delete startPage;
				startPage = next;
			}

			startPage = ref.startPage;
			currentPage = ref.currentPage;

			ref.startPage = nullptr;
			ref.currentPage = nullptr;
		}
		return *this;
	}

	T* alloc(S32 count)
	{
		U32 size = (static_cast<U32>(count) * sizeof(T));

		if (size >= getPageSize())
		{
			assert(false);
			// TODO: log this or something. Out of memory we can't alloc more than
			// the page size.
		}

		if (size >= currentPage->freespaceLeft)
		{
			// Allocate on new page.
			Page *newPage = reinterpret_cast<Page*>(calloc(1, getPageSize()));
			currentPage->next = newPage;
			currentPage = newPage;
		}
		
		// Allocate.
		const S32 newSize = static_cast<S32>(getPageSize()) - currentPage->freespaceLeft;
		currentPage->freespaceLeft -= size;

		void *mem = &currentPage->memory[newSize];

		return new (mem) T();
	}

private:
	Page *startPage;
	Page *currentPage;
};

#endif // _JBL_MEMORYCHUNKER_H_
