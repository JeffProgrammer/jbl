cmake_minimum_required(VERSION 3.1.3)
set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk)

if (MSVC)
	message("Using Toolset: ${CMAKE_VS_PLATFORM_TOOLSET}")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
	# Clang and GCC
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall")

	# Specific Mac OSX stuff
	if (APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
		set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
	endif()
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	jbl
)
set (JBL_SRC
	jbl/compiler.hpp
	jbl/conditionVariable.hpp
	jbl/conditionVariable.cpp
	jbl/dictionary.hpp
	jbl/hashFunction.hpp
	jbl/lib.hpp
	jbl/memoryChunker.hpp
	jbl/mutex.hpp
	jbl/mutex.cpp
	jbl/stack.hpp
	jbl/string.hpp
	jbl/string.cpp
	jbl/thread.hpp
	jbl/thread.cpp
	jbl/types.hpp
	jbl/typetraits.hpp
	jbl/vector.hpp
)
add_library(JBL ${JBL_SRC})

#------------------------------------------------------------------------------
# Tests
#------------------------------------------------------------------------------

option(BuildJBLTests "Build the JBL Test programs." ON)
if (BuildJBLTests)
	add_executable(VectorTest tests/testVector.cpp)
	target_link_libraries(VectorTest JBL)

	add_executable(ThreadingTest tests/testThreading.cpp)
	target_link_libraries(ThreadingTest JBL)

	add_executable(StackTest tests/testStack.cpp)
	target_link_libraries(StackTest JBL)

	add_executable(StringTest tests/testString.cpp)
	target_link_libraries(StringTest JBL)
	
	add_executable(DictionaryTest tests/testDictionary.cpp)
	target_link_libraries(DictionaryTest JBL)
endif()
