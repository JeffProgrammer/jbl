//-----------------------------------------------------------------------------
// thread.cpp
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

#include "thread.h"

#ifdef _WIN32

#include <Windows.h>

DWORD WINAPI Thread::win32ThreadWrapper(void *data)
{
	auto d = static_cast<ThreadData*>(data);
	d->fn(d->arg);
	return 1;
}

#endif // _win32

void Thread::sleep(unsigned int milliseconds)
{
#ifdef _WIN32
	Sleep(milliseconds);
#endif
}

Thread::Thread(threadFunction fn, void *arg)
{
	mThreadData.fn = fn;
	mThreadData.arg = arg;

	ThreadData *d = new ThreadData{ fn, arg };

#ifdef _WIN32
	mDone = false;
	mHandle = CreateThread(NULL, 0, Thread::win32ThreadWrapper, d, 0, &mThreadId);
	if (mHandle == NULL)
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
#endif
}

void Thread::join()
{
#ifdef _WIN32
	WaitForSingleObject(mHandle, INFINITE);
	mDone = true;
#endif
}