//-----------------------------------------------------------------------------
// string.hpp
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

#ifndef _JBL_STRING_H_
#define _JBL_STRING_H_

#include "jbl/compiler.h"
#include "jbl/lib.hpp"
#include "jbl/types.h"

/// Note: This implementation of small string optimization needs massive
/// improvements to get the most out of SSO.
///
/// Also, the implementation of this code is branch heavy. This needs
/// to be addressed eventually.

class String
{
	enum Constants
	{
		eSSOContents = 15, // Last byte has to be null terminator, so this is 1 less
		
		// Note: Changing this could affect padding in class
		// definition. Use at your own risk and adjust accordingly.
		eSSO = 16
	};
	
public:
	String();
	String(const char *str);
	String(const String &str);
	String(String &&str);
	~String();
	
	String& operator=(const String &str);
	String& operator=(String &&str);

	String operator+(const String &str);
	String& operator+=(const String &str);
	
	char operator[](S32 index);
	const char operator[](S32 index) const;
	
	FORCE_INLINE S32 length() const { return mCount; };
	
	const char* c_str() const;
	
	void reserve(S32 size);
	
private:
	char mStackBuffer[Constants::eSSO];
	char *mHeapBuffer;
	S32 mCount;
	S32 mCapacity;
};

inline bool operator==(const String &lhs, const String &rhs)
{
	// Shortcut, if the lengths don't match it's obviously not equal.
	S32 length = lhs.length();
	if (length != rhs.length())
		return false;

	const char *lStr = lhs.c_str();
	const char *rStr = rhs.c_str();

	while (--length >= 0)
	{
		if (lStr[length] != rStr[length])
			return false;
	}
	return true;
}

#endif // _JBL_STRING_H_
