#include "stdafx.h"

#include "PrefixStream.h"

#include <CppUnitTest.h>

#include <queue>
#include <ranges>
#include <algorithm>

using namespace std::literals::string_literals;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace ed::tests
{
	TEST_CLASS(PrefixStreamTests)
	{
		TEST_METHOD(EmptyPrefix)
		{
			std::stringstream ss;
			const std::list<std::string> inputList = {"", "aa", "zz zz"};
			PrefixOStream poss("", ss);

			for (const auto& currInputLine : inputList)
			{
				poss << currInputLine << std::endl;
			}

			std::list<std::string> outputList;
			std::string line;
			while (std::getline(ss, line))
			{
				outputList.push_back(line);
			}

			Assert::IsTrue(std::ranges::equal(inputList, outputList));
		}

		TEST_METHOD(NotEmptyPrefixIncludingFirstLine)
		{
			std::stringstream ss;
			const std::list<std::string> inputList = {"", "aa", "oo ww"};
			const auto prefix = "PREFIX_"s;
			auto expectedOutputView = inputList | std::views::transform([&prefix](const auto& el) { return prefix + el; });

			PrefixOStream poss((prefix), ss, true);

			for (const auto& currInputLine : inputList)
			{
				poss << currInputLine << std::endl;
			}

			std::list<std::string> outputList;
			std::string line;
			while (std::getline(ss, line))
			{
				outputList.push_back(line);
			}

            Assert::IsTrue(
                std::ranges::equal(
                    outputList,
					expectedOutputView
				)
			);
		}

		TEST_METHOD(NotEmptyPrefixExcludingFirstLine)
		{
			std::stringstream ss;
			const std::list<std::string> inputList{"1"s, "22"s, "333"s};
			const auto prefix = "PRE_"s;
			const std::list<std::string> expectedOutputList{ "1"s, "PRE_22"s, "PRE_333"s };

			PrefixOStream poss(prefix, ss);

			for (const auto& currInputLine : inputList)
			{
				poss << currInputLine << std::endl;
			}

			std::list<std::string> outputList;
			std::string line;
			while (std::getline(ss, line))
			{
				outputList.push_back(line);
			}

			Assert::IsTrue(std::ranges::equal(expectedOutputList, outputList));
		}

		TEST_METHOD(WideStreamPrefixIncludingFirstLine)
		{
			std::wstringstream ss;
			const auto inputList = { L"süd"s, L"nrd"s, L"wst"s, L"ost"s };
			const auto prefix = L"PREFIX_"s;
			auto expectedOutputView = inputList | std::views::transform([&prefix](const auto& el) { return prefix + el; });

			PrefixOStreamBasic poss(prefix, ss, true);

			for (const auto& currInputLine : inputList)
			{
				poss << currInputLine << std::endl;
			}

			std::list<std::wstring> outputList;
			std::wstring line;
			while (std::getline(ss, line))
			{
				outputList.push_back(line);
			}

			Assert::IsTrue(
				std::ranges::equal(
					outputList,
					expectedOutputView
				)
			);
		}

	};
}
