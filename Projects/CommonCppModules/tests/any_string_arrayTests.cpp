#include "stdafx.h"

#include <algorithm>

#include "any_string_array.h"

#include <CppUnitTest.h>

using namespace std::literals::string_literals;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


constexpr char abcAnsi[] = "abc";
constexpr wchar_t abcWide[] = L"abc";

template <typename Char_>
std::basic_string<Char_> abc()
{
	return any_string_array<Char_>(abcAnsi).data();
}


namespace ed::tests
{
	TEST_CLASS(any_string_arrayTests)
	{
		TEST_METHOD(Tests)
		{
			// Check sizes
			const size_t abcAnsiLen = strlen(abcAnsi);
			const size_t abcWideLen = wcslen(abcWide);
			Assert::AreEqual(abcAnsiLen, abcWideLen);
			const size_t abcLen = abcAnsiLen;

			// Init both Ansi and Wide array with abcAnsi and check
			constexpr auto abcAnsiInAnyStringArray = any_string_array<char>(abcAnsi);
			Assert::AreEqual(abcLen + 1, abcAnsiInAnyStringArray.size());
			Assert::AreEqual('\0', abcAnsiInAnyStringArray[abcLen]);
			Assert::AreEqual(abcAnsi, abcAnsiInAnyStringArray.data());

			constexpr auto abcWideInAnyStringArray = any_string_array<wchar_t>(abcAnsi);
			Assert::AreEqual(abcLen + 1, abcWideInAnyStringArray.size());
			Assert::AreEqual(L'\0', abcWideInAnyStringArray[abcLen]);
			Assert::AreEqual(abcWide, abcWideInAnyStringArray.data());

			// via abc Template
			Assert::AreEqual(abcWide, abc<wchar_t>().c_str());
			Assert::AreEqual(abcAnsi, abc<char>().c_str());
		}
	};
}
