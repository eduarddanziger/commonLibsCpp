#include "stdafx.h"

#include "src/AppPath.h"

#include <CppUnitTest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace ed::tests
{
	class AppPathInherited : public utility::AppPath
	{
		public:
			using AppPath::GetOwnProgramDataPath;
	};

	TEST_CLASS(AppPathTests)
	{
		TEST_METHOD(EmptyFileName)
		{
			std::filesystem::path ownDataPath;
			AppPathInherited::GetOwnProgramDataPath(ownDataPath, "");
			Assert::IsFalse(ownDataPath.empty()); // mostly: c:\ProgramData
		}

		TEST_METHOD(CompareOutputWithFileName)
		{
			const std::string fileNameWoExtension("meme");
			std::filesystem::path ownDataPath;
			AppPathInherited::GetOwnProgramDataPath(ownDataPath, fileNameWoExtension);
			const auto ownPath = ownDataPath.string();
			const auto ownPathLength = ownPath.length();
            const auto fileNameWoExtensionLength = fileNameWoExtension.length();
			Assert::IsTrue(fileNameWoExtensionLength < ownPathLength);
			Assert::AreEqual(ownPath.substr(ownPathLength - fileNameWoExtensionLength, fileNameWoExtensionLength), fileNameWoExtension);
		}

	};
}
