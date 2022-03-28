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

#if __cplusplus > 201703L
#define XORSTR_CPPVERSION 20
#elif __cplusplus == 201703L
#define XORSTR_CPPVERSION 17
#else
#define XORSTR_CPPVERSION 0
#endif

#if _MSC_VER 
#define XORSTR_FORCEINLINE __forceinline
#else
#define XORSTR_FORCEINLINE __attribute__((always_inline)) inline
#endif

#if XORSTR_CPPVERSION >= 20
#define XORSTR_CONSTEXPR consteval
#else
#define XORSTR_CONSTEXPR constexpr
#define XORSTR_ENCRYPTDEF(type) template<> struct xstr<type, 0> { XORSTR_FORCEINLINE XORSTR_CONSTEXPR xstr(const type* str, type* dst, const unsigned char key) noexcept { dst[0] = str[0] ^ key; } }
#endif

#if XORSTR_CPPVERSION >= 17
#define XORSTR_IFCONSTEXPR constexpr
#else
#define XORSTR_IFCONSTEXPR
#endif

namespace _xsimpl
{
	typedef unsigned int xsize_t;

	namespace detail {
#if XORSTR_CPPVERSION < 20
		template<typename T, const xsize_t n>
		struct xstr {
			XORSTR_FORCEINLINE XORSTR_CONSTEXPR xstr(const T* str, T* dst, const unsigned char key) noexcept {
				dst[n] = str[n] ^ key;

				xstr<T, n - 1>(str, dst, key);
			}
		};

		XORSTR_ENCRYPTDEF(char);
		XORSTR_ENCRYPTDEF(wchar_t);
#endif

		template<typename T, const T key>
		XORSTR_FORCEINLINE XORSTR_CONSTEXPR const T key_shift(const T x = key, const xsize_t n=sizeof(T)) noexcept {
			if (n == 1)
				return x;

			return key_shift<T, key>(x + (key << ((n-1) * 8)), n-1);
		}

		namespace hash {
			template<typename T, xsize_t n>
			XORSTR_FORCEINLINE static XORSTR_CONSTEXPR unsigned long pjw(const T(&s)[n]) noexcept {
				unsigned long h{}, high{};
				xsize_t i = 0;
				while (i < n) {
					h = (h << 4) + s[i++];
					if ((high = h & 0xF0000000) != 0)
						h ^= high >> 24;
					h &= ~high;
				}
				return h;
			}
		};
	}

	template<const xsize_t n, const unsigned char key, typename T>
	struct string_lit {
		XORSTR_FORCEINLINE XORSTR_CONSTEXPR string_lit(const T(&str)[n]) noexcept {
#if XORSTR_CPPVERSION >= 20
			for (xsize_t i = 0; i < n; i++)
				v[i] = str[i] ^ key;
#else
			detail::xstr<T, n - 1>(str, v, key);
#endif 
		}

		T v[n];
	};

	template<const xsize_t n, const unsigned char key, typename B>
	class xstr
	{
	private:
		typedef unsigned char xuint8_t;
		typedef unsigned short xuint16_t;
		typedef unsigned int xuint32_t;
		typedef unsigned long long xuint64_t;
	public:
		template<typename T>
		XORSTR_CONSTEXPR xstr(const T(&str)[n]) noexcept : buf(str) {}

		XORSTR_FORCEINLINE const B* dec() noexcept {
			tdec<xuint64_t>()
				|| tdec<xuint32_t>()
				|| tdec<xuint16_t>()
				|| tdec<xuint8_t>();

			return buf.v;
		}
	private:
		template<typename T>
		XORSTR_FORCEINLINE bool tdec() noexcept {
			B* str = buf.v;

			//constexpr unsigned long size = n;
			if XORSTR_IFCONSTEXPR (sizeof(T) <= sizeof(void*) &&
#ifdef XORSTR_DISABLE_OPTIMIZATION
					(n % sizeof(T)) == 0
#else
					n >= sizeof(T)
#endif
				) {

				constexpr const T k = detail::key_shift<T, static_cast<T>(key)>();
#ifndef XORSTR_DISABLE_OPTIMIZATION
				volatile T i = 0;
#endif
				do {
					*reinterpret_cast<T*>(str) ^= k;
					str += sizeof(T);
#ifdef XORSTR_DISABLE_OPTIMIZATION
				} while (str - static_cast<B*>(buf.v) < n);
#else
				} while (i < (const int)(n/sizeof(T)));

				if XORSTR_IFCONSTEXPR (n % sizeof(T) > 0) {
					do {
						*str++ ^= key;
					} while (str - static_cast<B*>(buf.v) < n);
				}
#endif
				return true;
			}

			return false;
		}
	public:
		string_lit<n, key, B> buf;
	};

	template<const unsigned long key, const xsize_t n, typename T>
	XORSTR_FORCEINLINE XORSTR_CONSTEXPR auto make(const T(&str)[n]) noexcept {
		constexpr const unsigned char _key = static_cast<const unsigned char>(key);
		return xstr<n, !_key ? '\x45' : _key, T>(str);
	}
}

#if _DEBUG || DEBUG
#define _x(s) s
#else
#define _x(s) _xsimpl::make<_xsimpl::detail::hash::pjw(__TIME__)+__COUNTER__>(s).dec()
#endif

#undef XORSTR_CPPVERSION
#undef XORSTR_CONSTEXPR
#undef XORSTR_ENCRYPTDEF
#undef XORSTR_FORCEINLINE
#undef XORSTR_IFCONSTEXPR
