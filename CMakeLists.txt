cmake_minimum_required(VERSION 3.1.3)

if (MSVC)
	message("Using Toolset: ${CMAKE_VS_PLATFORM_TOOLSET}")
	
	# TODO If we are using clang, do not set specific MSVC properties.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
	# Clang and GCC
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	
	# Specific Mac OSX stuff
	if (APPLE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
		set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
	endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------
# Tests
#------------------------------------------------------------------------------

option(BuildJBLTests "Build the JBL Test programs." ON)
if (BuildJBLTests)
	add_executable(VectorTest tests/testVector.cpp)
	add_executable(ListTest   tests/testList.cpp)
endif()