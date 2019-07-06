#include "typed_count.h"

using namespace std;
using namespace typed_count;

int main()
{
	const wchar_t* pwsz = L"ABCD";
	const char* psz = "abcd";

	// wszlen type is wchar_count
	auto wszlen = str_len_s(pwsz);
	// szlen type is char_count
	auto szlen = str_len_s(psz);

	// provides make_array() template function, + operator and _wch, _ch literal operators.
	// new [] takes only size_t. Need to provide a separate function make_array() to allocate an array.
	auto pNewWsz = make_array(wszlen + 1_wch);		
	auto pNewSz = make_array(szlen + 1_ch);			

	// This must not compile since szlen is not the type which str_cpy_s(... wchar_count) requires
	// str_cpy_s(pwsz, pNewWsz, szlen);

	// str_cpy_s() is a type-safe version of strcpy() and wcscpy().
	str_cpy_s(pwsz, pNewWsz, 1_wch + wszlen);
	printf("%ls\n", pNewWsz);

	// This must not compile since wszlen is not the type which str_cpy_s(... char_count) requires.
	// str_cpy_s(psz, pNewSz, wszlen);

	str_cpy_s(psz, pNewSz, szlen + 1_ch);
	printf("%s\n", pNewSz);

	// This must not compile since they are different types and we blocked comparisons between them.
	// wszlen == szlen

	// conversion of wchar count to char count. This will print 8.
	cout << "pwsz in bytes = " << wszlen.to_count_of<char>() << endl;
	// Also provides a convenient method to byte count in size_t.
	assert(wszlen.to_count_of<byte>().to_size() == wszlen.to_byte_count());

	// conversion of char count to wchar count. This will print 2.
	cout << "psz in wchar = " << szlen.to_count_of<wchar_t>() << endl;
	// Also provides a convenient method to wchar count in size_t.
	assert(szlen.to_count_of<wchar_t>().to_size() == szlen.to_wchar_count());

	constexpr page_count no_of_pages{ 128 };
	// supports operator <<.
	cout << "pages = " << no_of_pages << endl;
	// conversions to other units from pages.
	cout << "pages to kb = " << no_of_pages.to_count_of<Kb>() << endl;
	cout << "pages to mb = " << no_of_pages.to_count_of<Mb>() << endl;
	cout << "pages to bytes = " << no_of_pages.to_count_of<byte>() << endl;

	// safe array for constant string.
	safe_array<const wchar_t> cwsz{ L"EFGHI", wcslen(L"EFGHI") + 1 };
	// safe array for non-constant string.
	safe_array<wchar_t> wsz{ new wchar_t[6], 6 };

	// constant string count and non-constant string count are comparable.
	assert(cwsz.count() == wsz.count());

	// std::array only takes size_t. Need to provide a template class which takes count_of.
	fixed_size_array<const wchar_t, 6> cwsz2{ L"EFGHI" };
	// fixed_size_array can be converted to safe_array.
	safe_array<const wchar_t> wsz2{ cwsz2 };

	// both fixed_size_array and safe_array can be converted to a plain pointer.
	assert(wcscmp(cwsz2, wsz2) == 0 && wcscmp(L"EFGHI", wsz2) == 0);
	// both fixed_size_array and safe_array provides count() member function.
	assert(cwsz2.count() == wsz2.count() && wsz2.count() == 6_wch);

	// can define constant safe_array if you don't plan to modify the pointer itself.
	const safe_array<char> pOrgData{new char[10], 10};
	char_count i = 0_ch;
	// safe_array provides subscript operator for count_of.
	pOrgData[i++] = 'A';
	// safe_array provides pointer arithmetic with count_of and dereferencing.
	*(pOrgData + i) = 'B';
	// better to define mutable safe_array if you plan to modify the pointer.
	safe_array<char> pCurData = pOrgData;
	// provides += for mutable safe_array. pointer advances by 2 elements and count
	// decreases by 2 internally.
	pCurData += 2_ch;
	pCurData[0_ch] = 'C';
	// provides ++ for mutable safe_array.
	++pCurData;
	// pOrgData now becomes "ABCD";
	*pCurData = 'D';
	// supports decaying to pointer.
	if (pOrgData)
	{
		delete[] pOrgData;
	}

	fixed_size_array<const char, 5> name{"ABCD"};
	// provides make_safe_array() template function which returns a safe array
	// instead of a plain pointer.
	auto pNameCopy = make_safe_array(str_len_s(name) + 1_ch);
	for (i = 0_ch; i < pNameCopy.count() && name[i] != '\0'; ++i)
	{
		pNameCopy[i] = name[i];
	}
	pNameCopy[i] = '\0';
	cout << pNameCopy.data() << endl;
}

