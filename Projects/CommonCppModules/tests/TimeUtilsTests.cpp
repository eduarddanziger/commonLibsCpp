#include "stdafx.h"

#include "TimeUtils.h"

#include <CppUnitTest.h>

#include <chrono>

using namespace std::literals::chrono_literals;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ed::tests
{
	constexpr size_t TIME_STR_LENGTH =
		  4 + 1 + 2 + 1 + 2
		+ 1
		+ 2 + 1 + 2 + 1 + 2
		+ 1
		+ 6;
	constexpr size_t OFFSET_TO_MICROSECONDS_FRACTION = TIME_STR_LENGTH - (1 + 6);

	constexpr size_t OFFSET_TO_SECONDS_FRACTION = OFFSET_TO_MICROSECONDS_FRACTION - (1 + 2);

	constexpr size_t OFFSET_TO_MINUTES_FRACTION = OFFSET_TO_SECONDS_FRACTION - (1 + 2);

	constexpr size_t OFFSET_TO_HOURS_FRACTION = OFFSET_TO_MINUTES_FRACTION - (1 + 2);


	TEST_CLASS(TimeUtilsTests)
	{
		TEST_METHOD(CompareMicrosecondsHighResFraction)
		{
			const auto systemTimeNow = chr::high_resolution_clock::now();
			const auto str = systemTimeAsStringWithLocalTime(systemTimeNow);
			Assert::AreEqual(str.length(), TIME_STR_LENGTH);

			const auto systemTimeNowRoundedToSeconds = chr::time_point_cast<chr::seconds>(systemTimeNow);
			const auto strRoundedToSeconds = systemTimeAsStringWithLocalTime(systemTimeNowRoundedToSeconds);
			Assert::AreEqual(strRoundedToSeconds.substr(0, OFFSET_TO_MICROSECONDS_FRACTION), str.substr(0, OFFSET_TO_MICROSECONDS_FRACTION));
			Assert::AreEqual(strRoundedToSeconds.substr(OFFSET_TO_MICROSECONDS_FRACTION, 7), std::string(".000000"));

			const auto systemTimeNowWith11Microseconds = systemTimeNowRoundedToSeconds + chr::microseconds(11);
			const auto strWith11Microseconds = systemTimeAsStringWithLocalTime(systemTimeNowWith11Microseconds);
			Assert::AreEqual(strWith11Microseconds.substr(0, OFFSET_TO_MICROSECONDS_FRACTION), strWith11Microseconds.substr(0, OFFSET_TO_MICROSECONDS_FRACTION));
			Assert::AreEqual(strWith11Microseconds.substr(OFFSET_TO_MICROSECONDS_FRACTION, 7), std::string(".000011"));
		}

		TEST_METHOD(CompareMicrosecondsSysClockFraction)
		{
			const auto systemTimeNow = chr::system_clock::now();
			const auto str = systemTimeAsStringWithLocalTime(systemTimeNow);
			Assert::AreEqual(str.length(), TIME_STR_LENGTH);

			const auto systemTimeNowRoundedToSeconds = chr::time_point_cast<chr::seconds>(systemTimeNow);
			const auto strRoundedToSeconds = systemTimeAsStringWithLocalTime(systemTimeNowRoundedToSeconds);
			Assert::AreEqual(strRoundedToSeconds.substr(0, OFFSET_TO_MICROSECONDS_FRACTION), str.substr(0, OFFSET_TO_MICROSECONDS_FRACTION));
			Assert::AreEqual(strRoundedToSeconds.substr(OFFSET_TO_MICROSECONDS_FRACTION, 7), std::string(".000000"));

			const auto systemTimeNowWith11Microseconds = systemTimeNowRoundedToSeconds + chr::microseconds(11);
			const auto strWith11Microseconds = systemTimeAsStringWithLocalTime(systemTimeNowWith11Microseconds);
			Assert::AreEqual(strWith11Microseconds.substr(0, OFFSET_TO_MICROSECONDS_FRACTION), strWith11Microseconds.substr(0, OFFSET_TO_MICROSECONDS_FRACTION));
			Assert::AreEqual(strWith11Microseconds.substr(OFFSET_TO_MICROSECONDS_FRACTION, 7), std::string(".000011"));
		}

		TEST_METHOD(CheckTimeHighResClockCreatedFromSeconds)
		{
            constexpr chr::high_resolution_clock::time_point s54{ 54s };
			const auto strS54 = systemTimeAsStringWithLocalTime(s54);
			Assert::AreEqual(strS54.substr(OFFSET_TO_SECONDS_FRACTION, 3), std::string(":54"));
		}

		TEST_METHOD(CheckTimeSysClockCreatedFromSeconds)
		{
			constexpr chr::system_clock::time_point s54{ 54s };
			const auto strS54 = systemTimeAsStringWithLocalTime(s54);
			Assert::AreEqual(strS54.substr(OFFSET_TO_SECONDS_FRACTION, 3), std::string(":54"));
		}

		TEST_METHOD(CheckTimeSysClockCreatedFromSecondsAsWideString)
		{
			constexpr chr::system_clock::time_point s54{ 54s };
			const auto strS54 = systemTimeAsWideStringWithLocalTime(s54);
			Assert::AreEqual(strS54.substr(OFFSET_TO_SECONDS_FRACTION, 3), std::wstring(L":54"));
		}

		TEST_METHOD(CheckLocalTimeAsString)
		{
			const auto sysClockTimeStr = getLocalTimeAsString();

			SYSTEMTIME st;
			GetLocalTime(&st);
			const auto win32TimeStr = std::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}"
				, st.wYear
				, st.wMonth
				, st.wDay
				, st.wHour
				, st.wMinute
				, st.wSecond);

			Assert::AreEqual(sysClockTimeStr.substr(0, OFFSET_TO_MICROSECONDS_FRACTION), win32TimeStr);
		}

		TEST_METHOD(CheckLocalTimeAsStringWithTasDelimiter)
		{
			const auto sysClockTimeStr = getLocalTimeAsString("T");

			SYSTEMTIME st;
			GetLocalTime(&st);
			const auto win32TimeStr = std::format("{:04d}-{:02d}-{:02d}T{:02d}:{:02d}:{:02d}"
				, st.wYear
				, st.wMonth
				, st.wDay
				, st.wHour
				, st.wMinute
				, st.wSecond);

			Assert::AreEqual(sysClockTimeStr.substr(0, OFFSET_TO_MICROSECONDS_FRACTION), win32TimeStr);
		}
	};
}