/*
	MIT License

	Copyright (c) 2021 Teselka

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once
#ifndef XORSTR_FILE_HPP
#define XORSTR_FILE_HPP

#if __cplusplus > 201703L
#define XORSTR_CPP20
#define XORSTR_CONSTEXPR consteval
#else
#define XORSTR_CONSTEXPR constexpr
#endif

namespace __xorstr_impl
{
#if !defined(XORSTR_CPP20)
	namespace detail {
		template<const unsigned long n>
		struct xstr {
			XORSTR_CONSTEXPR xstr(const char* str, char* dst, const unsigned char key) noexcept {
				dst[n] = str[n] ^ key;

				detail::xstr<n - 1>(str, dst, key);
			}
		};

		template<>
		struct xstr<0> {
			XORSTR_CONSTEXPR xstr(const char* str, char* dst, const unsigned char key) noexcept {
				dst[0] = str[0] ^ key;
			}
		};
	}
#endif

	template<const unsigned long n, const unsigned char key>
	struct string_lit {
		template<typename T>
		XORSTR_CONSTEXPR string_lit(T str) noexcept {
#ifdef XORSTR_CPP20	
			for (unsigned long i = 0; i < n; i++)
				v[i] = str[i] ^ key;
#else
			detail::xstr<n-1>(str, v, key);
#endif 
		}

		char v[n];
	};

	template<const unsigned long n, const unsigned char key>
	class xstr 
	{
	public:
		template<typename T>
		XORSTR_CONSTEXPR xstr(T str) noexcept : buf(str) {}

#if _MSC_VER 
		__forceinline
#else
		__attribute__((always_inline))
#endif
		const char* dec() noexcept {
			buf.v[0] ^= key;
			for (volatile auto i=1; i<n; i++)
				buf.v[i] ^= key;

			return buf.v;
		}
	private:
		string_lit<n, key> buf;
	};

	template<const unsigned long n>
	XORSTR_CONSTEXPR auto make(const char(&str)[n]) noexcept {
		return xstr<n, static_cast<const unsigned char>((__TIME__[7] - '0' + __COUNTER__)^n)>(str);
	}
}

#if _DEBUG || DEBUG
#define _x(s) s
#else
#define _x(s) __xorstr_impl::make(s).dec()
#endif

#undef XORSTR_CPP20
#undef XORSTR_CONSTEXPR

#endif
