#include <iostream>
#include <cstring>
#include <cwchar>
#include <cerrno>
#include <cassert>
#include <cstdint>

#pragma once

namespace typed_count
{

// Default traits for a type
template <typename T>
struct unit_traits
{
	enum size : std::size_t
	{
		value = sizeof(T)
	};
};

// definitions of common units like 8kb page, kb, mb, and gb.
struct page {};

template <>
struct unit_traits<page>
{
	enum size : std::size_t
	{
		value = 8 * 1024
	};
};

struct kb {};

template <>
struct unit_traits<kb>
{
	enum size : std::size_t
	{
		value = 1024
	};
};

struct mb {};

template <>
struct unit_traits<mb>
{
	enum size : std::size_t
	{
		value = 1024 * 1024
	};
};

struct gb {};

template <>
struct unit_traits<gb>
{
	enum size : std::size_t
	{
		value = 1024 * 1024 * 1024
	};
};

using byte = uint8_t;

///
/// @brief A holder of count value.
///
/// An implementation class to store count value.
///
class count_holder
{
public:
	constexpr count_holder(std::size_t count) noexcept
		: count_(count)
	{}

	/// The default copy ctor is ok but define it explicitly for debuggability.
	///
	constexpr count_holder(const count_holder& other) noexcept
		: count_(other.count_)
	{}

	/// The default copy assignment is ok but define it explicitly for debuggability.
	///
	constexpr count_holder& operator =(const count_holder& other) noexcept
	{
		count_ = other.count_;

		return *this;
	}

	constexpr count_holder& operator +=(const count_holder& other) noexcept
	{
		count_ += other.count_;
		return *this;
	}

	constexpr count_holder& operator -=(const count_holder& other) noexcept
	{
		count_ -= other.count_;
		return *this;
	}

	constexpr count_holder& operator ++() noexcept
	{
		++count_;
		return *this;
	}

	constexpr count_holder operator ++(int) noexcept
	{
		count_holder r(*this);
		++count_;
		return r;
	}

	constexpr count_holder& operator --() noexcept
	{
		--count_;
		return *this;
	}

	constexpr count_holder operator --(int) noexcept
	{
		count_holder r(*this);
		--count_;
		return r;
	}

	/// Divider can't be 0. So, no exception will be thrown.
	///
	constexpr std::size_t convert(std::size_t multiplier, std::size_t divider) const noexcept
	{
		return count_ * multiplier / divider;
	}

	constexpr std::size_t count() const noexcept
	{
		return count_;
	}

private:
	std::size_t count_;
};

/// size in any unit like size in bytes, size in wchars, and etc.
///
template <typename T>
class count_of : private count_holder
{
public:
	constexpr count_of() noexcept
		: count_holder(std::size_t(0))
	{}

	/// to avoid implicit conversion from std::size_t to count_of<> type
	///
	constexpr explicit count_of(std::size_t count)
		: count_holder(count)
	{}

	/// to support like count_of<wchar_t>::to_count_of<uint8_t>()
	///
	template <typename U>
	constexpr count_of<U> to_count_of() const
	{
		return count_of<U>(convert(unit_traits<T>::size::value, unit_traits<U>::size::value));
	}

	/// To support compatibility with existing C code or existing C++ library   
	///
	constexpr std::size_t to_size() const
	{
		return count();
	}

	constexpr int to_int() const noexcept
	{
		return static_cast<int>(count());
	}

	constexpr unsigned long to_ulong() const noexcept
	{
		return static_cast<unsigned long>(count());
	}

	/// default copy ctor is ok.
	///
	constexpr count_of(const count_of<T>& other) noexcept = default;

	/// default copy assignment is ok.
	///
	constexpr count_of<T>& operator =(const count_of<T>& other) noexcept = default;

	constexpr count_of<T>& operator +=(const count_of<T>& other) noexcept
	{
		count_holder::operator +=(other);
		return *this;
	}

	constexpr count_of<T>& operator -=(const count_of<T>& other) noexcept
	{
		count_holder::operator -=(other);
		return *this;
	}

	constexpr count_of<T>& operator ++() noexcept
	{
		count_holder::operator ++();
		return *this;
	}

	constexpr count_of<T> operator ++(int) noexcept
	{
		count_of<T> ret{ *this };
		count_holder::operator ++(int());
		return ret;
	}

	constexpr count_of<T>& operator --() noexcept
	{
		count_holder::operator --();
		return *this;
	}

	constexpr count_of<T> operator --(int) noexcept
	{
		count_of<T> ret{ *this };
		count_holder::operator --(int());
		return ret;
	}

	/// utility function for conversion to byte count.
	///
	constexpr std::size_t to_byte_count() const noexcept
	{
		return to_count_of<byte>().to_size();
	}

	constexpr int to_int_byte_count() const noexcept
	{
		return to_count_of<byte>().to_int();
	}

	constexpr unsigned long to_ulong_byte_count() const noexcept
	{
		return to_count_of<byte>().to_ulong();
	}

	/// utility function for conversion to wchar count.
	///
	constexpr std::size_t to_wchar_count() const noexcept
	{
		return to_count_of<wchar_t>().to_size();
	}

	constexpr int to_int_wchar_count() const noexcept
	{
		return to_count_of<wchar_t>().to_int();
	}

	constexpr unsigned long ToRawUlongWcharCount() const noexcept
	{
		return to_count_of<wchar_t>().to_ulong();
	}
};

template <typename T>
std::ostream& operator <<(std::ostream& os, count_of<T> count)
{
	os << count.to_size();
	return os;
}

/// To support operator overloading for +
///
template <typename T>
count_of<T> operator +(count_of<T> lhs, count_of<T> rhs)
{
	return count_of<T>(lhs.to_size() + rhs.to_size());
}

/// To support operator overloading for -
///
template <typename T>
count_of<T> operator -(count_of<T> lhs, count_of<T> rhs)
{
	return count_of<T>(lhs.to_size() - rhs.to_size());
}

/// To support operator overloading for ==
///
template <typename T>
bool operator ==(count_of<T> lhs, count_of<T> rhs)
{
	return lhs.to_size() == rhs.to_size();
}

/// To support operator overloading for !=
///
template <typename T>
bool operator !=(count_of<T> lhs, count_of<T> rhs)
{
	return !(lhs == rhs);
}

/// To support operator overloading for <
///
template <typename T>
bool operator <(count_of<T> lhs, count_of<T> rhs) noexcept
{
	return lhs.to_size() < rhs.to_size();
}

/// To support operator overloading for <=
///
template <typename T>
bool operator <=(count_of<T> lhs, count_of<T> rhs) noexcept
{
	return lhs < rhs || lhs == rhs;
}

/// To support operator overloading for >
///
template <typename T>
bool operator >(count_of<T> lhs, count_of<T> rhs) noexcept
{
	return !(lhs <= rhs);
}

/// To support operator overloading for >=
///
template <typename T>
bool operator >=(count_of<T> lhs, count_of<T> rhs) noexcept
{
	return !(lhs < rhs);
}

/// typedefs for common units
using byte_count = count_of<byte>;
using char_count = count_of<char>;
using wchar_count = count_of<wchar_t>;
using page_count = count_of<page>;
using kb_count = count_of<kb>;
using mb_count = count_of<mb>;
using gb_count = count_of<gb>;

constexpr byte_count operator "" _bt(unsigned long long count) noexcept
{
	return byte_count(count);
}

constexpr char_count operator "" _ch(unsigned long long count) noexcept
{
	return char_count(count);
}

constexpr wchar_count operator "" _wch(unsigned long long count) noexcept
{
	return wchar_count(count);
}

constexpr page_count operator "" _pg(unsigned long long count) noexcept
{
	return page_count(count);
}

constexpr kb_count operator "" _KB(unsigned long long count) noexcept
{
	return kb_count(count);
}

constexpr mb_count operator "" _MB(unsigned long long count) noexcept
{
	return mb_count(count);
}

constexpr gb_count operator "" _GB(unsigned long long count) noexcept
{
	return gb_count(count);
}

template <typename T, unsigned long long N>
constexpr count_of<T> GetArraySize(T(&)[N]) noexcept
{
	return count_of<T>(N);
}

template <typename T>
constexpr T* operator +(T* p, count_of<T> distance) noexcept
{
	return p + distance.to_size();
}

template <typename T>
constexpr T*& operator +=(T*& p, count_of<T> distance) noexcept
{
	p += distance.to_size();
	return p;
}

template <typename T>
constexpr T* operator -(T* p, count_of<T> distance) noexcept
{
	return p - distance.to_size();
}

template <typename T>
constexpr T*& operator -=(T*& p, count_of<T> distance) noexcept
{
	p -= distance.to_size();
	return p;
}

template <typename T, std::size_t N>
struct fixed_size_array
{
	using count_t = count_of<std::remove_cv_t<T>>;

	T elems[N];

	constexpr T& operator[](count_t idx) const
	{
		return const_cast<T&>(elems[idx.to_size()]);
	}

	constexpr count_t count() const noexcept
	{
		return count_t(N);
	}

	constexpr operator T* const() const noexcept
	{
		return const_cast<T*>(&elems[0]);
	}
};

template <typename T>
class safe_array
{
	using count_t = count_of<std::remove_cv_t<T>>;

	T* pElems_;
	count_t count_;

public:
	constexpr safe_array() noexcept
		: safe_array(nullptr, 0)
	{}

	constexpr safe_array(T* pElems, std::size_t count) noexcept
		: safe_array(pElems, count_t(count))
	{}

	template <std::size_t N>
	constexpr safe_array(const fixed_size_array<T, N>& fixed_array)
		: safe_array(fixed_array, fixed_array.count())
	{}

	constexpr safe_array(T* pElems, const count_t& count) noexcept
		: pElems_(pElems), count_(count)
	{}

	constexpr T& operator[](count_t idx) const
	{
		return pElems_[idx.to_size()];
	}

	constexpr count_t count() const noexcept
	{
		return count_;
	}

	constexpr operator bool() const noexcept
	{
		return pElems_ && count_ > count_t(0);
	}

	safe_array<T>& operator +=(count_t distance) noexcept
	{
		pElems_ += distance.to_size();
		count_ -= distance;
		return *this;
	}

	safe_array<T> operator +(count_t distance) const noexcept
	{
		return safe_array<T>(pElems_ + distance.to_size(), count_ - distance);
	}

	safe_array<T>& operator ++() noexcept
	{
		*this += count_t(1);
		return *this;
	}

	safe_array<T> operator ++(int) noexcept
	{
		safe_array<T> t(*this);
		*this += count_t(1);
		return t;
	}

	T& operator *() const noexcept
	{
		return *pElems_;
	}

	constexpr operator T* const() const noexcept
	{
		return const_cast<T*>(pElems_);
	}
};

constexpr safe_array<wchar_t> nullptr_wchar_array{};

/// type-safe string length for char type
///
char_count str_len_s(const char* psz)
{
	return char_count(strlen(psz));
}

/// type-safe string length for wchar_t type
///
wchar_count str_len_s(const wchar_t* pwsz)
{
	return wchar_count(wcslen(pwsz));
}

/// type-safe string copy for wchar_t
///
void str_cpy_s(const wchar_t* pwsz_src, wchar_t* pwsz_dest, wchar_count len)
{
	wcscpy_s(pwsz_dest, len.to_size(), pwsz_src);
}

/// type-safe string copy for char
///
void str_cpy_s(const char* psz_src, char* psz_dest, char_count len)
{
	strcpy_s(psz_dest, len.to_size(), psz_src);
}

template <typename T>
T* construct_array(count_of<T> count)
{
	// new[] operator requires std::size_t argument. So we need to_size() method
	return new T[count.to_size()];
}

template <typename T>
T* alloca_array(count_of<T> count)
{
	return (T*)alloca(count.to_byte_count());
}

}
