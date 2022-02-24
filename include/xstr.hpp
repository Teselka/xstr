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

#if _MSC_VER 
#define XORSTR_FORCEINLINE __forceinline
#else
#define XORSTR_FORCEINLINE __attribute__((always_inline))
#endif

#if XORSTR_CPPVERSION >= 20
#define XORSTR_CONSTEXPR consteval
#else
#define XORSTR_CONSTEXPR constexpr
#define XORSTR_ENCRYPTDEF(type) template<> struct xstr<type, 0> { XORSTR_FORCEINLINE XORSTR_CONSTEXPR xstr(const type* str, type* dst, const unsigned char key) noexcept { dst[0] = str[0] ^ key; } }
#endif

namespace __xorstr_impl
{
	namespace detail {
#if XORSTR_CPPVERSION < 20
		template<typename T, const unsigned long n>
		struct xstr {
			XORSTR_FORCEINLINE XORSTR_CONSTEXPR xstr(const T* str, T* dst, const unsigned char key) noexcept {
				dst[n] = str[n] ^ key;

				detail::xstr<T, n-1>(str, dst, key);
			}
		};

		XORSTR_ENCRYPTDEF(char);
		XORSTR_ENCRYPTDEF(wchar_t);
#endif

		namespace hash {
			template<typename T, unsigned long n>
			XORSTR_FORCEINLINE static XORSTR_CONSTEXPR unsigned long pjw(const T(&s)[n]) noexcept {
				unsigned long h{}, high{};
				unsigned long i = 0;
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

	template<const unsigned long n, const unsigned char key, typename T>
	struct string_lit {
		XORSTR_FORCEINLINE XORSTR_CONSTEXPR string_lit(const T(&str)[n]) noexcept {
#if XORSTR_CPPVERSION >= 20
			for (unsigned long i = 0; i < n; i++)
				v[i] = str[i] ^ key;
#else
			detail::xstr<T, n-1>(str, v, key);
#endif 
		}

		T v[n];
	};

	template<const unsigned long n, const unsigned char key, typename B>
	class xstr
	{
	public:
		template<typename T>
		XORSTR_CONSTEXPR xstr(const T(&str)[n]) noexcept : buf(str) {}

		XORSTR_FORCEINLINE const B* dec() noexcept {
			B* str = buf.v;	

#ifndef XORSTR_DISABLE_BIGSTR_OPT
			constexpr unsigned long size = n;
			if (size >= sizeof(unsigned long) && size >= 64) {
				unsigned long s=0, it=0;
				constexpr unsigned long _key = (key << 24) + (key << 16) + (key << 8) + key;
				do {
					reinterpret_cast<unsigned long*>(str)[it++] ^= _key;
					s += sizeof(unsigned long);
				} while (s < n);

				str += s;
			}
			
			if ((size % sizeof(unsigned long)) > 0 || size < 64) {
#endif
				do {
					*str++ ^= key;
				} while (str - static_cast<B*>(buf.v) < n);

#ifndef XORSTR_DISABLE_BIGSTR_OPT 
			}
#endif
			return buf.v;
		}
	private:
		string_lit<n, key, B> buf;
	};

	template<const unsigned long key, const unsigned long n, typename T>
	XORSTR_FORCEINLINE static XORSTR_CONSTEXPR auto make(const T(&str)[n]) noexcept {
		constexpr const unsigned char _key = static_cast<const unsigned char>(key);
		return xstr<n, !_key ? '\x45' : _key, T>(str);
	}
}

#if _DEBUG || DEBUG
#define _x(s) s
#else
#define _x(s) __xorstr_impl::make<__xorstr_impl::detail::hash::pjw(__TIME__)+__COUNTER__>(s).dec()
#endif

#undef XORSTR_CPPVERSION
#undef XORSTR_CONSTEXPR
#undef XORSTR_ENCRYPTDEF
#undef XORSTR_FORCEINLINE

#endif
