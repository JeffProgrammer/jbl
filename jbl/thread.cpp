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
#include "thread.hpp"

#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32

DWORD WINAPI Thread::win32ThreadWrapper(void *data)
{
	auto d = static_cast<ThreadData*>(data);
	d->fn(d->arg);
	return 1;
}

#else

void* Thread::pthreadThreadWrapper(void *data)
{
	ThreadData *d = static_cast<ThreadData*>(data);
	d->fn(d->arg);
	return nullptr;
}

#endif // _win32

void Thread::sleep(U32 milliseconds)
{
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}

Thread::Thread(threadFunction fn, void *arg)
{
	mThreadData.fn = fn;
	mThreadData.arg = arg;
	mDone = false;

#ifdef _WIN32
	mHandle = CreateThread(NULL, 0, Thread::win32ThreadWrapper, &mThreadData, 0, &mThreadId);
	if (mHandle == NULL)
		exit(-2);
#else
	if (pthread_create(&mThread, nullptr, Thread::pthreadThreadWrapper, &mThreadData) != 0)
		exit(-2);
#endif
}

Thread::~Thread()
{
#ifdef _WIN32
	if (mHandle)
	{
		if (!mDone)
			WaitForSingleObject(mHandle, INFINITE);
		CloseHandle(mHandle);
	}
#else
	if (!mDone)
		pthread_join(mThread, nullptr);
	pthread_cancel(mThread);
#endif
}

void Thread::join()
{
#ifdef _WIN32
	WaitForSingleObject(mHandle, INFINITE);
#else
	pthread_join(mThread, nullptr);
#endif
	mDone = true;
}
