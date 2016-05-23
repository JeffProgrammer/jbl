# Jeff's Base Library (JBL)
A base library for C++11.

# Motivations
1) Portability - Sticking to C++98 headers from the STL, while using C++11 and C++14 features allow us to use a front end that accepts new modern C++ language features, while still being able to target the standard library to an older version. This is specifically useful when targeting MacOSX 10.6, because I can use C++11 language features and use libstdc++ as libc++ requires MacOSX 10.7 or higher.

2) Less Bloat - The STL is a huge toolbox. I doubt that I have ever even used half of what is available in the STL, and it keeps growing. Also, heavy use of the templates in the STL for Emscripten increases build sizes in the megabytes.

3) Control - I like to have control over fixing bugs instead of relying on compiler vendors to hopefully fix the bugs (Some of the STL had regressions going from Visual studio 2010 and haven't been fixed just until the latest update to VS2015, or take the whole std::chrono debacle where the high precision timer wasn't being used for a long time)

4) Debug times in some implementations are extremely slow because of error checking and exceptions. I realize that debug is supposed to be slower, but in my own opinion it was sometimes excessive.

5) OOP - Parts of the STL are really not true OOP where they should be. std::find vs a find method on a vector, for example, really sucks.

6) Personal Learning - I wanted to practice writing data structures, because I want to.

# Compilers tested:

* MSVC Community Edition 2015 Update 2
* Apple XCode 7.0.1 with Clang
* Clang 3.8

# License
This library is licensed under the 3 clause BSD license. See LICENSE for more information.