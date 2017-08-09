//-----------------------------------------------------------------------------
// thread.cpp
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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "jbl/string.hpp"

String::String()
{
	memset(mStackBuffer, 0, sizeof(char) * Constants::eSSO);
	mHeapBuffer = nullptr;
	mCount = 0;
	mCapacity = Constants::eSSO;
}

String::String(const char *str)
{
	memset(mStackBuffer, 0, sizeof(char) * Constants::eSSO);
	mCount = strlen(str);
	if (mCount < Constants::eSSO)
	{
		memcpy(mStackBuffer, str, sizeof(char) * mCount);
		mCapacity = Constants::eSSO;
		mHeapBuffer = nullptr;
	}
	else
	{
		mCapacity = mCount;
		mHeapBuffer = reinterpret_cast<char*>(calloc(mCount + 1, sizeof(char)));
		memcpy(mHeapBuffer, str, mCount * sizeof(char));
		mHeapBuffer[mCount] = 0x0; // Null terminator
	}
}

String::String(const String &str) :
	String(str.c_str())
{
}

String::String(String &&str)
{
	// I can't move assign two stack buffers. So I have to memcpy this.
	// Luckilly, the heap buffer can be moved.
	memcpy(mStackBuffer, str.mStackBuffer, Constants::eSSO);
	mHeapBuffer = str.mHeapBuffer;
	mCapacity = str.mCapacity;
	mCount = str.mCount;
	
	str.mHeapBuffer = nullptr;
	str.mCapacity = 0;
	str.mCount = 0;
}

String::~String()
{
	if (mHeapBuffer != nullptr)
	{
		free(mHeapBuffer);
		mHeapBuffer = nullptr;
	}
}

String& String::operator=(const String &str)
{
	if (mHeapBuffer != nullptr)
		delete[] mHeapBuffer;
	
	mCount = str.mCount;
	if (mCount < Constants::eSSO - 1)
	{
		memcpy(mStackBuffer, str.mStackBuffer, sizeof(char) * mCount);
		mCapacity = Constants::eSSO;
		mHeapBuffer = nullptr;
	}
	else
	{
		mCapacity = str.mCapacity;
		mHeapBuffer = reinterpret_cast<char*>(calloc(mCount + 1, sizeof(char)));
		memcpy(mHeapBuffer, str.mHeapBuffer, mCount * sizeof(char));
		mHeapBuffer[mCount] = 0x0; // Null terminator
	}
	
	return *this;
}

String& String::operator=(String &&str)
{
	if (this != &str)
	{
		if (mHeapBuffer != nullptr)
			delete[] mHeapBuffer;
		
		// I can't move assign two stack buffers. So I have to memcpy this.
		// Luckilly, the heap buffer can be moved.
		memcpy(mStackBuffer, str.mStackBuffer, Constants::eSSO);
		mHeapBuffer = str.mHeapBuffer;
		mCapacity = str.mCapacity;
		mCount = str.mCount;
		
		str.mHeapBuffer = nullptr;
		str.mCapacity = 0;
		str.mCount = 0;
	}
	return *this;
}

String String::operator+(const String &str)
{
	String ret;
	ret.mCount = ret.mCapacity = str.mCount + mCount;
	if (ret.mCount >= Constants::eSSOContents)
	{
		// Doesn't fit in buffer. Alloc
		ret.mHeapBuffer = reinterpret_cast<char*>(calloc(ret.mCount + 1, sizeof(char)));
		memcpy(ret.mHeapBuffer, mHeapBuffer, mCount * sizeof(char));
		memcpy(ret.mHeapBuffer + mCount, str.mHeapBuffer, str.mCount * sizeof(char));
	}
	else
	{
		// Fits in small string buffer.
		memcpy(ret.mStackBuffer, mStackBuffer, mCount * sizeof(char));
		memcpy(ret.mStackBuffer + mCount, str.mStackBuffer, str.mCount * sizeof(char));
	}
	return ret;
}

String& String::operator+=(const String &str)
{
	S32 count = mCount + str.mCount;
	if (count >= Constants::eSSO)
	{
		if (mHeapBuffer != nullptr)
		{
			// It must have been reserved then, since its empty.
			if (mHeapBuffer[0] == 0x0)
			{
				// We need to copy into the heap buffer from the stack.
				memcpy(mHeapBuffer, mStackBuffer, Constants::eSSOContents * sizeof(char));
			}
			
			if ((count + 1) < mCapacity)
			{
				memcpy(mHeapBuffer + mCount, str.c_str(), str.mCount * sizeof(char));
				mCount = count;
				mHeapBuffer[count] = 0x0; // Null terminator
			}
			else
			{
				// Realloc
				mCapacity = static_cast<S32>(mCapacity * 1.5f);
				mHeapBuffer = reinterpret_cast<char*>(realloc(mHeapBuffer, mCapacity * sizeof(char)));
				memcpy(mHeapBuffer + mCount, str.c_str(), str.mCount * sizeof(char));
				mCount = count;
				mHeapBuffer[count] = 0x0; // Null terminator
			}
		}
		else
		{
			mHeapBuffer = reinterpret_cast<char*>(calloc(count + 1, sizeof(char)));
			memcpy(mHeapBuffer, mStackBuffer, Constants::eSSOContents * sizeof(char));
			memcpy(mHeapBuffer + mCount, str.c_str(), str.mCount * sizeof(char));
			mCount = count;
			mCapacity = mCount;
		}
	}
	else
	{
		memcpy(mStackBuffer + mCount, str.mStackBuffer, sizeof(char) * str.mCount);
		mCount += str.mCount;
	}
	return *this;
}

char String::operator[](S32 index)
{
#ifdef DEBUG_BUILD
	if (index < 0 || index >= mCount)
		assert(false);
#endif
	
	if (mCount < Constants::eSSOContents)
		return mStackBuffer[index];
	return mHeapBuffer[index];
}

const char String::operator[](S32 index) const
{
#ifdef DEBUG_BUILD
	if (index < 0 || index >= mCount)
		assert(false);
#endif
	
	if (mCount < Constants::eSSOContents)
		return mStackBuffer[index];
	return mHeapBuffer[index];
}

const char* String::c_str() const
{
	return (mCount < Constants::eSSO) ? mStackBuffer : mHeapBuffer;
}

void String::reserve(S32 size)
{
	mHeapBuffer = reinterpret_cast<char*>(realloc(mHeapBuffer, size * sizeof(char)));
	mHeapBuffer[mCount] = 0x0; // Null termination
	mCapacity = size;
}
