//-----------------------------------------------------------------------------
// mutex.cpp
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
//
// Note: The Win32 Condition Variable code for Windows NT below Vista
// is based off of Douglas C. Schmidt and Irfan Pyarali.
// The implementation is here: http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
// The copyright is here: http://www.cs.wustl.edu/~schmidt/ACE-copying.html
// Note that this method is also used inside of glfw2 and TinyCppThread.
// This is needed because Windows NT 5.X and lower do not implement native
// support for condition variables.
//-----------------------------------------------------------------------------

#include <assert.h>
#include <Windows.h>
#include "jbl/conditionVariable.h"

bool ConditionVariable::sInitialized = false;
#ifdef _WIN32
	bool ConditionVariable::sIsVistaOrGreater = false;
#endif

void ConditionVariable::init()
{
#ifdef _WIN32
	auto lib = LoadLibrary("NTdll.dll");
	assert(lib);
	auto fn = GetProcAddress(lib, "IsWindowsVistaOrGreater");
	if (fn != nullptr)
		sIsVistaOrGreater = fn();
	else
		sIsVistaOrGreater = false;
	FreeLibrary(lib);
#endif
	
	sInitialized = true;
}

ConditionVariable::ConditionVariable()
{
	assert(sInitialized);
	
#ifdef _WIN32
	if (sIsVistaOrGreater)
	{
		InitializeConditionVariable(&mConditionVariable);
	}
	else
	{
		mLegacyConditionVariable.waitersCount = 0;
		InitializeCriticalSection(&mLegacyConditionVariable.waitersCountLock);
		
		// Auto Reset Event
		mLegacyConditionVariable.events[Win32LegacyConditionVariable::eSIGNAL] = CreateEvent(NULL, FALSE, FALSE, NULL);
		
		// Manual Reset Event
		mLegacyConditionVariable.events[Win32LegacyConditionVariable::eBROADCAST] = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
#else
	pthread_cond_init(&mConditionVar, nullptr);
#endif
}

ConditionVariable::~ConditionVariable()
{
	assert(sInitialized);
	
#ifdef _WIN32
	// Vista condition variable has no cleanup. See MSDN for more information.
	if (!sIsVistaOrGreater)
	{
		DeleteCriticalSection(&mLegacyConditionVariable.waitersCountLock);
		
		// Kill events
		CloseHandle(mLegacyConditionVariable.events[Win32LegacyConditionVariable::eSIGNAL]);
		CloseHandle(mLegacyConditionVariable.events[Win32LegacyConditionVariable::eBROADCAST]);
	}
#else
	pthread_cond_destroy(&mConditionVar);
#endif
}

void ConditionVariable::wait(Mutex *mutex)
{
	assert(sInitialized);
	
#ifdef _WIN32
	if (sIsVistaOrGreater)
	{
		SleepConditionVariableCS(&mConditionVariable, &mutex->mMutex, INFINITE);
	}
	else
	{
		EnterCriticalSection(&mLegacyConditionVariable.waitersCountLock);
		{
			mLegacyConditionVariable.waitersCount++;
		}
		LeaveCriticalSection(&mLegacyConditionVariable.waitersCountLock);
		
		mutex->unlock();
		
		int result = WaitForMultipleObjects(2, mLegacyConditionVariable.events, FALSE, INFINITE);

		int lastWaiter = 0;
		EnterCriticalSection(&mLegacyConditionVariable.waitersCountLock);
		{
			mLegacyConditionVariable.waitersCount--;
			lastWaiter = result == WAIT_OBJECT_0 + Win32LegacyConditionVariable::eBROADCAST
				&& mLegacyConditionVariable.waitersCount == 0;
		}
		LeaveCriticalSection(&mLegacyConditionVariable.waitersCountLock);

		if (lastWaiter)
		{
			// Last waiter to be notified, reset manual event.
			ResetEvent(mLegacyConditionVariable.events[Win32LegacyConditionVariable::eBROADCAST]);
		}
		
		mutex->lock();
	}
#else
	pthread_cond_wait(&mConditionVar, &mutex->mMutex);
#endif
}

void ConditionVariable::signal()
{
	assert(sInitialized);
	
#ifdef _WIN32
	if (sIsVistaOrGreater)
	{
		WakeConditionVariable(&mConditionVariable);
	}
	else
	{
		EnterCriticalSection(&mLegacyConditionVariable.waitersCountLock);
		int hasWaiters = mLegacyConditionVariable.waitersCount > 0;
		LeaveCriticalSection(&mLegacyConditionVariable.waitersCountLock);
		
		if (hasWaiters)
		{
			// Go ahead and signal
			SetEvent(mLegacyConditionVariable.events[Win32LegacyConditionVariable::eSIGNAL]);
		}
	}
#else
	pthread_cond_signal(&mConditionVar);
#endif
}

void ConditionVariable::signalAll()
{
	assert(sInitialized);
	
#ifdef _WIN32
	if (sIsVistaOrGreater)
	{
		WakeAllConditionVariable(&mConditionVariable);
	}
	else
	{
		EnterCriticalSection(&mLegacyConditionVariable.waitersCountLock);
		int hasWaiters = mLegacyConditionVariable.waitersCount > 0;
		LeaveCriticalSection(&mLegacyConditionVariable.waitersCountLock);
		
		if (hasWaiters)
		{
			// Go ahead and broadcast
			SetEvent(mLegacyConditionVariable.events[Win32LegacyConditionVariable::eBROADCAST]);
		}
	}
#else
	pthread_cond_broadcast(&mConditionVar);
#endif
}

