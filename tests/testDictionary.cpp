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

	printf("Now iterating with an iterator!\n");
	for (auto it = kv.begin(); it != kv.end(); ++it)
		printf(" kv: %s, %d\n", (*it).key.c_str(), (*it).value);

	auto position = kv.find("world");
	if (position != kv.end())
	{
		printf("erasing world.\n");
		kv.erase(position);
	}

	for (const auto &kvPair : kv)
	{
		printf(" kv: %s, %d\n", kvPair.key.c_str(), kvPair.value);
	}

	Dictionary<S32, S32> kvInts(5);
	for (S32 i = 0; i < 1000; ++i)
		kvInts.insert(i, i);

	// Print out the 1-1000
	printf("kvInts Dictionary Contents:\n");
	for (S32 i = 0; i < 1000; i++)
		printf(" kv: %d %d\n", i, kvInts[i]);

	// Remove all of them in reverse order!
	for (S32 i = 999; i > -1; --i)
		kvInts.erase(kvInts.find(i));

	printf("Does kvInts have anything left in it: ");
	bool no = true;
	for (auto vals : kvInts)
	{
		no = false;
		break;
	}
	printf("%s. The expected result was no.\n", no ? "no" : "yes.");

	constexpr S32 inc = 6;
	printf("Now lets add some items and remove a few of them.\n");
	for (S32 i = 100; i < 200; i += inc)
	{
		kvInts.insert(i, i);
		printf(" Inserting %d %d\n", i, i);
	}
	kvInts.erase(kvInts.find(100 + (inc * 7)));
	kvInts.erase(kvInts.find(100 + (inc * 4)));
	kvInts.erase(kvInts.find(100 + (inc * 13)));
	printf("Now lets print out what kvInts has.\n");
	for (auto vals : kvInts)
		printf(" kv: %d %d\n", vals.key, vals.value);

	// Now lets delete 118 in the loop.
	printf("Now lets delete 118 while we are in a loop.\n");
	auto iter = kvInts.begin();
	while (iter != kvInts.end())
	{
		if ((*iter).key == 118)
			iter = kvInts.erase(iter);
		else
			++iter;
	}
	for (auto vals : kvInts)
		printf(" kv: %d %d\n", vals.key, vals.value);

#ifdef _WIN32
   system("pause");
#endif
	return 0;
}
