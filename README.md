# typed_count
Type-safe unit count template library

This library provides type-safe count of various units like char, wchar, Page,
Kb, Mb, Gb, Tb, an etc.

## Motivation
We need to count various units like count of bytes, count of wchars, count of buffers,
count of pages, and etc. But they are all expressed as any integral type and
C++ type system does not block you converting an integral type to another integral type
like casting unsigned long to int and passing count of one unit to the parameter
for count of the other unit. So, there are chances that someone makes mistakes.
This module provides a template class which can represent count of any unit but provides type safety
for count of different units.

## Usage

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
  }