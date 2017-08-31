//-----------------------------------------------------------------------------
// typetraits.hpp
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

#ifndef _JBL_TYPETRAITS_HPP_
#define _JBL_TYPETRAITS_HPP_

/// Compile time type traits implementation.
namespace TypeTraits
{
	/// The base typetrait class, taken from cplusplus.com as reference.
	/// @reference http://www.cplusplus.com/reference/type_traits/integral_constant/
	template<typename T, T v>
	struct IntegralConstant
	{
		static constexpr T value = v;
		typedef T valueType;
		typedef IntegralConstant<T, v> type;
		constexpr operator T() { return v; }
	};

	/// @group IsSame
	///
	/// Checks if type A and type B are the same type.
	/// Has a value of true if they are the same, false if they are different.
	template<typename A, typename B>
	struct IsSame : IntegralConstant<bool, false> {};

	template<typename A>
	struct IsSame<A, A> : IntegralConstant<bool, true> {};
	/// @endgroup IsSame
};
#endif // _JBL_TYPETRAITS_HPP_