//-----------------------------------------------------------------------------
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
// * Neither the name of threadGL nor the names of its
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

#include <stdio.h>
#include "jbl/types.h"
#include "jbl/thread.h"
#include "jbl/mutex.h"
#include "jbl/conditionVariable.h"
#include "jbl/stack.h"

Mutex mutex;
S32 inc = 0;

Mutex pcMutex;
ConditionVariable cv;
Stack<S32> stack;
volatile bool pcDone = false;

void incriment()
{
	LockGuard g(&mutex);
	++inc;
	printf("inc is now: %d\n", inc);
}

void myThreadFn(void *arg)
{
	S32 x = *(S32*)arg;
	for (S32 i = 0; i < x; ++i)
		incriment();
	Thread::sleep(1000);
}

void producer(void *)
{
	for (S32 i = 0; i < 20; ++i)
	{
		pcMutex.lock();
		stack.push(i);
		pcMutex.unlock();
		cv.signal();
		Thread::sleep(750);
	}
	pcDone = true;
	cv.signal();
}

void consumer(void *)
{
	while (true)
	{
		pcMutex.lock();
		while (!pcDone && stack.isEmpty())
			cv.wait(&pcMutex);

		if (pcDone)
		{
			pcMutex.unlock();
			break;
		}
	
		printf("%d\n", stack.getTop());
		stack.pop();
		pcMutex.unlock();
	}
}

S32 main(S32 argc, const char **argv)
{
	S32 x = 20;
	Thread t(myThreadFn, &x);
	Thread t2(myThreadFn, &x);
	t.join();
	t2.join();

	printf("Testing producer consumer condition variable.\n");
	Thread c(consumer, nullptr);
	Thread p(producer, nullptr);
	c.join();
	p.join();
	
#ifdef _WIN32
   system("pause");
#endif
	return 0;
}