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
#include "jbl/stack.hpp"

S32 main(S32 argc, const char **argv)
{
	Stack<S32> stack;
	for (S32 i = 0; i < 30; ++i)
		stack.push(i);

	printf("Stack contents from top to bottom:\n");
	while (!stack.isEmpty())
	{
		printf("%d\n", stack.getTop());
		stack.pop();
	}

	// Note, this will only zero the last two items on the stack. Everything else
	// will still have garbage in it.
	stack.push(5);
	stack.push(10);
	stack.popZeroMem();
	stack.popZeroMem();

#ifdef _WIN32
	system("pause");
#endif
	return 0;
}
