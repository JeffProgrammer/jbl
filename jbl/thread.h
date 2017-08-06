//-----------------------------------------------------------------------------
// thread.h
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

#ifndef _JBL_THREAD_H_
#define _JBL_THREAD_H_

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include "types.h"

typedef void(*threadFunction)(void*);

class Thread
{
public:
	Thread(threadFunction fn, void *arg);
	~Thread();
	
	Thread(const Thread &) = delete;
	Thread(Thread &&) = delete;

	static void sleep(U32 milliseconds);

	void join();

private:
	struct ThreadData
	{
		threadFunction fn;
		void *arg;
	};

	ThreadData mThreadData;
	bool mDone;

#ifdef _WIN32
	HANDLE mHandle;
	DWORD mThreadId;

	static DWORD WINAPI win32ThreadWrapper(void *data);
#else
	pthread_t mThread;
	
	static void* pthreadThreadWrapper(void *data);
#endif
};

#endif // _JBL_THREAD_H_
