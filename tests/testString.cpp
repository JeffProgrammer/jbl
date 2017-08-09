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
#include "jbl/types.h"
#include "jbl/lib.hpp"
#include "jbl/string.hpp"

S32 main(S32 argc, const char **argv)
{
	String x = "hello world";
	printf("x: %s\n", x.c_str());
	
	String y = move_cast(x);
	printf("y: %s\n", y.c_str());
	
	String z = "This is a very long string to avoid the SSO engine.";
	printf("z: %s\n", z.c_str());
	
	String zA = move_cast(z);
	printf("zA: %s\n", zA.c_str());
	
	auto addStrings = [](String a, String b) -> String
	{
		return a + b;
	};
	
	String zB = addStrings("Hello, ", "World!");
	printf("zb: %s\n", zB.c_str());
	
	printf("The letter at position 1 inside of zB is: %c\n", (char)zB[1]);
	
	String a = "Hello, ";
	a += "World!";
	printf("The result of the string a addition is: %s\n", a.c_str());
	
	String reservedString;
	reservedString.reserve(250);
	for (S32 i = 0; i < 32; i++)
		reservedString += "a";
	printf("Reserved string result is: %s\n", reservedString.c_str());
	
	String addedString;
	for (S32 i = 0; i < 32; i++)
	{
		addedString += "*";
		printf("addedString at %d, %s\n", i, addedString.c_str());
	}

#ifdef _WIN32
   system("pause");
#endif
	return 0;
}
