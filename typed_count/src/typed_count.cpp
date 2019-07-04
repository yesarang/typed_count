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

	auto pNewWsz = construct_array(wszlen + 1_wch);
	auto pNewSz = construct_array(szlen + 1_ch);

	// This must not compile since szlen is not the type which str_cpy_s(... wchar_count) requires
	// str_cpy_s(pwsz, pNewWsz, szlen);

	str_cpy_s(pwsz, pNewWsz, 1_wch + wszlen);
	printf("%ls\n", pNewWsz);

	// This must not compile since wszlen is not the type which str_cpy_s(... char_count) requires.
	// str_cpy_s(psz, pNewSz, wszlen);

	str_cpy_s(psz, pNewSz, szlen + 1_ch);
	printf("%s\n", pNewSz);

	// This must not compile since they are different types and we blocked comparisons between them.
	// wszlen == szlen

	// This will print 8.
	cout << "pwsz in bytes = " << wszlen.to_count_of<char>() << endl;
	assert(wszlen.to_count_of<byte>().to_size() == wszlen.to_byte_count());

	// This will print 2.
	cout << "psz in wchar = " << szlen.to_count_of<wchar_t>() << endl;
	assert(szlen.to_count_of<wchar_t>().to_size() == szlen.to_wchar_count());

	constexpr page_count no_of_pages{ 128 };
	cout << "pages = " << no_of_pages << endl;
	cout << "pages to kb = " << no_of_pages.to_count_of<kb>() << endl;
	cout << "pages to mb = " << no_of_pages.to_count_of<mb>() << endl;
	cout << "pages to bytes = " << no_of_pages.to_count_of<byte>() << endl;

	safe_array<const wchar_t> cwsz{ L"EFGHI", wcslen(L"EFGHI") + 1 };
	safe_array<wchar_t> wsz{ new wchar_t[6], 6 };

	assert(cwsz.count() == wsz.count());

	fixed_size_array<const wchar_t, 6> cwsz2{ L"EFGHI" };
	safe_array<const wchar_t> wsz2{ cwsz2 };

	assert(wcscmp(cwsz2, wsz2) == 0 && wcscmp(L"EFGHI", wsz2) == 0);
	assert(cwsz2.count() == wsz2.count() && wsz2.count() == 6_wch);
}

