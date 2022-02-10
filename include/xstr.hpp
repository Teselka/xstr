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
#define XORSTR_CPPVERSION 20
#elif __cplusplus == 201703L
#define XORSTR_CPPVERSION 17
#else
#define XORSTR_CPPVERSION 0
#endif

#if XORSTR_CPPVERSION >= 20
#define XORSTR_CONSTEXPR consteval
#else
#define XORSTR_CONSTEXPR constexpr
#endif

namespace __xorstr_impl
{
	namespace detail {
#if XORSTR_CPPVERSION < 20
		template<const size_t n, typename B>
		struct xstr {
			XORSTR_CONSTEXPR xstr(const B* str, B* dst, const unsigned char key) noexcept {
				dst[n] = str[n] ^ key;

				detail::xstr<n - 1, B>(str, dst, key);
			}
		};

		template<>
		struct xstr<0, char> {
			XORSTR_CONSTEXPR xstr(const char* str, char* dst, const unsigned char key) noexcept {
				dst[0] = str[0] ^ key;
			}
		};

		template<>
		struct xstr<0, wchar_t> {
			XORSTR_CONSTEXPR xstr(const wchar_t* str, wchar_t* dst, const unsigned char key) noexcept {
				dst[0] = str[0] ^ key;
			}
		};
#endif

		namespace hash {
			template<typename T, size_t n>
			XORSTR_CONSTEXPR unsigned long pjw(const T(&s)[n]) {
				unsigned long h{}, high{};
				unsigned long i = 0;
				while (i < n) {
					h = (h << 4) + s[i++];
					if (high = h & 0xF0000000)
						h ^= high >> 24;
					h &= ~high;
				}
				return h;
			}
		};
	}

	template<const size_t n, const unsigned char key, typename B>
	struct string_lit {
		template<typename T>
		XORSTR_CONSTEXPR string_lit(T str) noexcept {
#if XORSTR_CPPVERSION >= 20	
			for (unsigned long i = 0; i < n; i++)
				v[i] = str[i] ^ key;
#else
			detail::xstr<n - 1, B>(str, v, key);
#endif 
		}

		B v[n];
	};

	template<const size_t n, const unsigned char key, typename B>
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
		const B* dec() noexcept {
			B* str = buf.v;

			*str++ ^= key;
			do {
				*str++ ^= key;
			} while (str - static_cast<B*>(buf.v) < n-1);
			*str = (B)'\0';

			return buf.v;
		}
	private:
		string_lit<n, key, B> buf;
	};

	template<const unsigned long key, const size_t n, typename T>
	XORSTR_CONSTEXPR auto make(const T(&str)[n]) noexcept {
		return xstr<n, static_cast<const T>(key), T>(str);
	}
}

#if _DEBUG || DEBUG
#define _x(s) s
#else
#define _x(s) __xorstr_impl::make<__xorstr_impl::detail::hash::pjw(__TIME__)>(s).dec()
#endif

#undef XORSTR_CPPVERSION
#undef XORSTR_CONSTEXPR

#endif
