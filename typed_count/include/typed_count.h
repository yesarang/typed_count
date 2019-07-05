#include <iostream>
#include <cstring>
#include <cwchar>
#include <cerrno>
#include <cassert>
#include <cstdint>

#pragma once

namespace typed_count
{

//! @defgroup unit_traits Unit traits
//! Defines traits of units such as Page,
//! All unit traits must provides unit size in bytes.
//! @{

//! Default unit traits for a type.
//!
//! It uses sizeof() and so the complete type must be visible.
//! If the real size of a unit is different from type definition,
//! need to define a separate unit_traits using template specialization.
//!
//! Usage:
//!     size_t unit_size = unit_traits<Unit>::size::value;
template <typename T>
struct unit_traits
{
	//! unit size.
	//!
	//! Unit size must be in size_t.
	enum size : std::size_t
	{
		value = sizeof(T)		//!< unit size in byte(s).
	};
};

//! empty Page type for Page unit traits.
struct Page {};

//! Page unit traits.
template <>
struct unit_traits<Page>
{
	enum size : std::size_t
	{
		value = 8 * 1024		//!< 8KB page.
	};
};

//! empty Kb type for Kb unit.
struct Kb {};

//! Kb unit traits.
template <>
struct unit_traits<Kb>
{
	enum size : std::size_t
	{
		value = 1024						//!< 1KB == 1024 bytes.
	};
};

//! empty Mb type for Mb unit.
struct Mb {};

//! Mb unit traits.
template <>
struct unit_traits<Mb>
{
	enum size : std::size_t
	{
		value = 1024 * 1024					//!< 1MB == 1024KB.
	};
};

//! empty Gb type for Gb unit.
struct Gb {};

//! Gb unit traits.
template <>
struct unit_traits<Gb>
{
	enum size : std::size_t		
	{
		value = 1024 * 1024 * 1024			//!< 1GB == 1024MB.
	};
};

//! empty Tb type for Tb unit.
struct Tb {};

//! Gb unit traits.
template <>
struct unit_traits<Tb>
{
	enum size : std::size_t		
	{
		value = size_t(1024) * 1024 * 1024 * 1024	//!< 1TB == 1024GB.
	};
};

//! @}

//! A holder of count value.
//!
//! An implementation class to store count value.
class count_holder
{
public:
	//! ctor.
	constexpr count_holder(std::size_t count) noexcept
		: count_(count)
	{}

	//! copy ctor.
	//!
	//! The default copy ctor is ok but define it explicitly for debuggability.
	constexpr count_holder(const count_holder& other) noexcept
		: count_(other.count_)
	{}

	//! copy assignment operator.
	//!
	//! The default copy assignment is ok but define it explicitly for debuggability.
	constexpr count_holder& operator =(const count_holder& other) noexcept
	{
		count_ = other.count_;

		return *this;
	}

	//! supports += operator.
	constexpr count_holder& operator +=(const count_holder& other) noexcept
	{
		count_ += other.count_;
		return *this;
	}

	//! supports -= operator.
	constexpr count_holder& operator -=(const count_holder& other) noexcept
	{
		count_ -= other.count_;
		return *this;
	}

	//! supports prefix ++ operator.
	constexpr count_holder& operator ++() noexcept
	{
		++count_;
		return *this;
	}

	//! supports postfix ++ operator.
	constexpr count_holder operator ++(int) noexcept
	{
		count_holder r(*this);
		++count_;
		return r;
	}

	//! supports prefix -- operator.
	constexpr count_holder& operator --() noexcept
	{
		--count_;
		return *this;
	}

	//! supports postfix -- operator.
	constexpr count_holder operator --(int) noexcept
	{
		count_holder r(*this);
		--count_;
		return r;
	}

	//! Converts count in one unit to count in the other unit.
	//!
	//! Divider can't be 0. So, no exception will be thrown.
	constexpr std::size_t convert(std::size_t multiplier, std::size_t divider) const noexcept
	{
		return count_ * multiplier / divider;
	}

	//! cast to size_t.
	constexpr std::size_t count() const noexcept
	{
		return count_;
	}

private:
	std::size_t count_;
};

//! type-safe count of any unit.
//!
//! type requirement: unit_traits<T> must be defined and > 0.
template <typename T>
class count_of : private count_holder
{
public:
	using traits_t = T;

	static_assert(unit_traits<traits_t>::size::value > 0);

	//! @name ctors_casts
	//! ctors and casts.
	//! @{

	//! default ctor.
	constexpr count_of() noexcept
		: count_holder(std::size_t(0))
	{}

	//! ctor.
	//!
	//! This ctor intentionally does not allow automatic conversion of size_t to count_of<T>
	//! to provide type-safety.
	constexpr explicit count_of(std::size_t count)
		: count_holder(count)
	{}

	//! default copy ctor is ok.
	constexpr count_of(const count_of<T>& other) noexcept = default;

	//! casts to any count_of<U>.
	//!
	//! to support like count_of<wchar_t>::to_count_of<uint8_t>().
	//! type requirement: unit_traits<U>::size::value must be defined and > 0.
	template <typename U>
	constexpr auto to_count_of() const
	{
		using to_traits_t = std::remove_cv_t<U>;
		static_assert(unit_traits<to_traits_t>::size::value > 0);
		return count_of<to_traits_t>(convert(unit_traits<traits_t>::size::value, unit_traits<to_traits_t>::size::value));
	}

	//! casts to size_t.
	//!
	//! To support compatibility with existing C code or existing C++ library which requires size_t.
	constexpr std::size_t to_size() const noexcept
	{
		return count();
	}

	//! casts to int.
	//!
	//! To support compatibility with existing C code or existing C++ library which requires int.
	constexpr int to_int() const noexcept
	{
		return static_cast<int>(count());
	}

	//! casts to ulong.
	//!
	//! To support compatibility with existing C code or existing C++ library which requires ulong.
	constexpr unsigned long to_ulong() const noexcept
	{
		return static_cast<unsigned long>(count());
	}

	//! casts to count in bytes as size_t.
	//!
	//! Has the same effect as to_count_of<std::byte>().to_size().
	constexpr std::size_t to_byte_count() const noexcept
	{
		return to_count_of<std::byte>().to_size();
	}

	//! casts to count in bytes as int.
	//!
	//! Has the same effect as to_count_of<std::byte>().to_int().
	constexpr int to_int_byte_count() const noexcept
	{
		return to_count_of<std::byte>().to_int();
	}

	//! casts to count in bytes as ulong.
	//!
	//! Has the same effect as to_count_of<std::byte>().to_ulong().
	constexpr unsigned long to_ulong_byte_count() const noexcept
	{
		return to_count_of<std::byte>().to_ulong();
	}

	//! casts to count in wchars as size_t.
	//!
	//! Has the same effect as to_count_of<wchar_t>().to_ulong().
	constexpr std::size_t to_wchar_count() const noexcept
	{
		return to_count_of<wchar_t>().to_size();
	}

	//! casts to count in wchars as int.
	//!
	//! Has the same effect as to_count_of<wchar_t>().to_int().
	constexpr int to_int_wchar_count() const noexcept
	{
		return to_count_of<wchar_t>().to_int();
	}

	//! casts to count in wchars as ulong.
	//!
	//! Has the same effect as to_count_of<wchar_t>().to_ulong().
	constexpr unsigned long to_ulong_wchar_count() const noexcept
	{
		return to_count_of<wchar_t>().to_ulong();
	}

	//! @}

	//! @name operators
	//! overloaded operators.
	//! @{

	//! default copy assignment is ok.
	constexpr count_of<T>& operator =(const count_of<T>& other) noexcept = default;

	//! supports += operator.
	constexpr count_of<T>& operator +=(const count_of<T>& other) noexcept
	{
		count_holder::operator +=(other);
		return *this;
	}

	//! supports -= operator.
	constexpr count_of<T>& operator -=(const count_of<T>& other) noexcept
	{
		count_holder::operator -=(other);
		return *this;
	}

	//! supports prefix ++ operator.
	constexpr count_of<T>& operator ++() noexcept
	{
		count_holder::operator ++();
		return *this;
	}

	//! supports postfix ++ operator.
	constexpr count_of<T> operator ++(int) noexcept
	{
		count_of<T> ret{ *this };
		count_holder::operator ++(int());
		return ret;
	}

	//! supports prefix -- operator.
	constexpr count_of<T>& operator --() noexcept
	{
		count_holder::operator --();
		return *this;
	}

	//! supports postfix -- operator.
	constexpr count_of<T> operator --(int) noexcept
	{
		count_of<T> ret{ *this };
		count_holder::operator --(int());
		return ret;
	}

	//! @}
};

//! @name operators
//! overloaded non-member operators.
//! @{

template <typename T>
std::ostream& operator <<(std::ostream& os, count_of<T> count)
{
	os << count.to_size();
	return os;
}

//! To support operator overloading for +
template <typename T>
count_of<T> operator +(count_of<T> lhs, count_of<T> rhs)
{
	return count_of<T>(lhs.to_size() + rhs.to_size());
}

//! To support operator overloading for -
template <typename T>
count_of<T> operator -(count_of<T> lhs, count_of<T> rhs)
{
	return count_of<T>(lhs.to_size() - rhs.to_size());
}

//! To support operator overloading for ==
template <typename T>
bool operator ==(count_of<T> lhs, count_of<T> rhs)
{
	return lhs.to_size() == rhs.to_size();
}

//! To support operator overloading for !=
template <typename T>
bool operator !=(count_of<T> lhs, count_of<T> rhs)
{
	return !(lhs == rhs);
}

//! To support operator overloading for <
template <typename T>
bool operator <(count_of<T> lhs, count_of<T> rhs) noexcept
{
	return lhs.to_size() < rhs.to_size();
}

//! To support operator overloading for <=
template <typename T>
bool operator <=(count_of<T> lhs, count_of<T> rhs) noexcept
{
	return lhs < rhs || lhs == rhs;
}

//! To support operator overloading for >
template <typename T>
bool operator >(count_of<T> lhs, count_of<T> rhs) noexcept
{
	return !(lhs <= rhs);
}

//! To support operator overloading for >=
template <typename T>
bool operator >=(count_of<T> lhs, count_of<T> rhs) noexcept
{
	return !(lhs < rhs);
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

//! @}

//! @name typedefs
//! typedefs for common units
//! @{

using byte_count = count_of<std::byte>;
using char_count = count_of<char>;
using wchar_count = count_of<wchar_t>;
using page_count = count_of<Page>;
using kb_count = count_of<Kb>;
using mb_count = count_of<Mb>;
using gb_count = count_of<Gb>;
using tb_count = count_of<Tb>;

//! @}

//! @name literals
//! literal operators
//! @{

//! byte_count literal.
constexpr byte_count operator "" _bt(unsigned long long count) noexcept
{
	return byte_count(static_cast<size_t>(count));
}

//! char_count literal.
constexpr char_count operator "" _ch(unsigned long long count) noexcept
{
	return char_count(static_cast<size_t>(count));
}

//! wchar_count literal.
constexpr wchar_count operator "" _wch(unsigned long long count) noexcept
{
	return wchar_count(static_cast<size_t>(count));
}

//! page_count literal.
constexpr page_count operator "" _pg(unsigned long long count) noexcept
{
	return page_count(static_cast<size_t>(count));
}

//! kb_count literal.
constexpr kb_count operator "" _kb(unsigned long long count) noexcept
{
	return kb_count(static_cast<size_t>(count));
}

//! mb_count literal.
constexpr mb_count operator "" _mb(unsigned long long count) noexcept
{
	return mb_count(static_cast<size_t>(count));
}

//! gb_count literal.
constexpr gb_count operator "" _gb(unsigned long long count) noexcept
{
	return gb_count(static_cast<size_t>(count));
}

//! tb_count literal.
constexpr gb_count operator "" _tb(unsigned long long count) noexcept
{
	return gb_count(static_cast<size_t>(count));
}

//! @}

template <typename T, std::size_t N>
constexpr count_of<T> array_size(T(&)[N]) noexcept
{
	return count_of<T>(N);
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

//! Safe array with data and count
//!
//! safe_array tracks data pointer and count together, which means
//! count will decrease as much as data pointer advances.
//! It forces users to access an element using typed_count.
//! It also supports pointer arithmetic, subscript operator,
//! decaying to a plan pointer, and deferencing just like a pointer.
//! safe_array does not own the pointed-to array.
//!
//! #### Usage ####
//!     const safe_array<char> pOrgData{new char], 10};
//!     char_count i = 0_ch;
//!     pOrgData[i++] = 'A';
//!     *(pOrgData + i) = 'B';
//!     safe_array<char> pCurData = pOrgData;
//!     pCurData += 2_ch;
//!     pCurData[0_ch] = 'C';
//!     ++pCurData;
//!     *pCurData = 'D';
//!     if (pOrgData)
//!     {
//!         delete[] pOrgData;
//!     }
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

//! nullptr for wchar safe array
constexpr safe_array<wchar_t> nullptr_wchar_array{};

//! type-safe string length for char type
char_count str_len_s(const char* psz)
{
	return char_count(strlen(psz));
}

//! type-safe string length for wchar_t type
wchar_count str_len_s(const wchar_t* pwsz)
{
	return wchar_count(wcslen(pwsz));
}

//! type-safe string copy for wchar_t
void str_cpy_s(const wchar_t* pwsz_src, wchar_t* pwsz_dest, wchar_count len)
{
	wcscpy_s(pwsz_dest, len.to_size(), pwsz_src);
}

//! type-safe string copy for char
void str_cpy_s(const char* psz_src, char* psz_dest, char_count len)
{
	strcpy_s(psz_dest, len.to_size(), psz_src);
}

template <typename T>
T* make_array(count_of<T> count)
{
	// new[] operator requires std::size_t argument. So we need to_size() method
	return new T[count.to_size()];
}

template <typename T>
safe_array<T> make_safe_array(count_of<T> count)
{
	// new[] operator requires std::size_t argument. So we need to_size() method
	return { new T[count.to_size()], count };
}

template <typename T>
T* alloca_array(count_of<T> count)
{
	return (T*)alloca(count.to_byte_count());
}

}

