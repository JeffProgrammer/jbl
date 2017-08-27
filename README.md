# Jeff's Base Library (JBL)
A base library for C++11 and beyond.

# Motivations
1) Portability - Sticking to C++98 headers from the STL, while using C++11,C++14,C++17 features allow us to use a front end that accepts new modern C++ language features, while still being able to target the standard library to an older version. This is specifically useful when targeting MacOSX 10.6 or lower, because I can use C++11 language features and use libstdc++ as libc++ requires MacOSX 10.7 or higher.

2) Consistency - The lot of implementations of the STL are implementation defined. I want a toolbox that I can use across multiple toolchains and be the same. For example, Small String Optimization inside of my String library.

3) Less Bloat - The STL is a huge toolbox. I doubt that I have ever even used half of what is available in the STL, and it keeps growing. Look at the bloat with streams...

4) Control - I like to have control over fixing bugs instead of relying on compiler vendors to hopefully fix the bugs (Some of the STL had regressions going from Visual studio 2010 and haven't been fixed just until the latest update to VS2015, or take the whole std::chrono debacle where the high precision timer wasn't being used for a long time)

5) Performance - Take std::unordered_map and how many different heap allocations it does. Sure, I could go out of my way to write a custom allocator. Also, there are quite a few inefficiencies between standard libraries versions. For example, small string optimization is implemented in some versions of the STL while others are not. I want consistency in performance.

6) Debug times in some implementations are extremely slow because of error checking and exceptions. I realize that debug is supposed to be slower, but in my own opinion it was sometimes excessive.

7) Personal Learning - I wanted to practice writing data structures and understand the work that the STL hides from the programmer.

# Compilers tested:

* MSVC Community Edition 2017
* Apple XCode 8.3.3 with Clang

# License
This library has been relicensed under the MIT License, from the 3 Clause BSD License. See LICENSE for more information.