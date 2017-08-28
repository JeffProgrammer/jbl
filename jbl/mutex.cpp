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
//-----------------------------------------------------------------------------

#include "mutex.hpp"

Mutex::Mutex()
{
#ifdef _WIN32
	InitializeCriticalSection(&mMutex);
#else
	pthread_mutex_init(&mMutex, nullptr);
#endif
}

Mutex::~Mutex()
{
#ifdef _WIN32
	DeleteCriticalSection(&mMutex);
#else
	pthread_mutex_destroy(&mMutex);
#endif
}

void Mutex::lock()
{
#ifdef _WIN32
	EnterCriticalSection(&mMutex);
#else
	pthread_mutex_lock(&mMutex);
#endif
}

bool Mutex::tryLock()
{
#ifdef _WIN32
	return !!TryEnterCriticalSection(&mMutex);
#else
	return !!pthread_mutex_trylock(&mMutex);
#endif
}

void Mutex::unlock()
{
#ifdef _WIN32
	LeaveCriticalSection(&mMutex);
#else
	pthread_mutex_unlock(&mMutex);
#endif
}

//-----------------------------------------------------------------------------

LockGuard::LockGuard(Mutex *mutex)
{
	mMutex = mutex;
	mutex->lock();
}

LockGuard::~LockGuard()
{
	mMutex->unlock();
}
