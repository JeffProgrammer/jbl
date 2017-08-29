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
#include "jbl/lib.hpp"
#include "jbl/vector.hpp"
#include "jbl/dictionary.hpp"

S32 main(S32 argc, const char **argv)
{
	Dictionary<String, S32> kv(32);

	kv.insert("hello", 2);
	kv.insert("world", 4);
	kv.insert("pq", 66);

	// Now iterate
	printf("First dictionary contents:\n");
	printf(" kv: %s, %d\n", "hello", kv["hello"]);
	printf(" kv: %s, %d\n", "world", kv["world"]);
	printf(" kv: %s, %d\n", "pq", kv["pq"]);

	Dictionary<S32, S32> kvInts(5);
	for (S32 i = 0; i < 1000; ++i)
		kvInts.insert(i, i);

	// Print out the 1-1000
	// TODO: use iterator
	printf("kvInts Dictionary Contents:\n");
	for (S32 i = 0; i < 1000; i++)
		printf(" kv: %d %d\n", i, kvInts[i]);

#ifdef _WIN32
   system("pause");
#endif
	return 0;
}