//-----------------------------------------------------------------------------
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

#include <stdio.h>
#include "jbl/types.hpp"
#include "jbl/thread.hpp"
#include "jbl/mutex.hpp"
#include "jbl/conditionVariable.hpp"
#include "jbl/stack.hpp"

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
