#pragma once
#ifndef XORSTR_FILE_HPP
#define XORSTR_FILE_HPP

#if _MSC_VER 
#define XORSTR_FORCEINLINE __forceinline
#else
#define XORSTR_FORCEINLINE __attribute__((always_inline))
#endif

#if __cplusplus > 201703L
#define XORSTR_CPP20
#define XORSTR_CONSTEXPR consteval
#else
#define XORSTR_CONSTEXPR constexpr
#endif-

#define XORSTR_KEYGEN(n) static_cast<const unsigned char>((__TIME__[7]-'0'+__COUNTER__)^n)

namespace __xostr_impl
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

		XORSTR_FORCEINLINE const char* dec() noexcept {
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
		return xstr<n, XORSTR_KEYGEN(n)>(str);
	}
}

#if _DEBUG || DEBUG
#define _x(s) s
#else
#define _x(s) __xostr_impl::make(s).dec()
#endif

#endif
