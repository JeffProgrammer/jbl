//-----------------------------------------------------------------------------
// conditionVariable.h
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

#ifndef _JBL_CONDITIONVARIABLE_H_
#define _JBL_CONDITIONVARIABLE_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "mutex.hpp"
#include "types.hpp"

class ConditionVariable
{
public:
	ConditionVariable();
	~ConditionVariable();

	void wait(Mutex *mutex);
	void signal();
	void signalAll();

	// NOTE: this must be called prior to using condition variables.
	static void init();
	
private:
	static bool sInitialized;
	
#ifdef _WIN32
	CONDITION_VARIABLE mConditionVariable;
	static bool sIsNativeSupport;
	
	// This is here for windows NT 5.x compatability.
	// NT 6.0+ uses the condition variable. Below that it doesn't exist.
	struct Win32LegacyConditionVariable
	{
		enum
		{
			eSIGNAL = 0,
			eBROADCAST = 1,
			eMAXEVENTS = 2
		};
		
		HANDLE events[eMAXEVENTS];
		
		U32 waitersCount;
		CRITICAL_SECTION waitersCountLock;
	} mLegacyConditionVariable;
#else
	pthread_cond_t mConditionVar;
#endif
};

#endif // _JBL_CONDITIONVARIABLE_H_
