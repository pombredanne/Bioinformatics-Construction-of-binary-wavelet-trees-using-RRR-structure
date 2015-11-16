#include "stdafx.h"
#include "CppUnitTest.h"
#include <cstdint>
#include "..\WaveletTree\RRR.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTests
{
	TEST_CLASS(UnitTestRRRSelect)
	{
	public:

		TEST_METHOD(Select1)
		{
			RRR rrr(string("001010011010101101000001001010011011000101101101011"));
			Assert::AreEqual((uint64_t)14, rrr.select1(7), L"Select1 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)15, rrr.select1(8), L"Select1 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)2, rrr.select1(1), L"Select1 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)4, rrr.select1(2), L"Select1 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)23, rrr.select1(10), L"Select1 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)26, rrr.select1(11), L"Select1 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)50, rrr.select1(24), L"Select1 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)49, rrr.select1(23), L"Select1 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)47, rrr.select1(22), L"Select1 passed", LINE_INFO());
		}

		TEST_METHOD(Select0)
		{
			RRR rrr(string("001010011010101101000001001010011011000101101101011"));
			Assert::AreEqual((uint64_t)0, rrr.select0(1), L"Select0 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)1, rrr.select0(2), L"Select0 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)3, rrr.select0(3), L"Select0 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)5, rrr.select0(4), L"Select0 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)48, rrr.select0(27), L"Select0 passed", LINE_INFO());
			Assert::AreEqual((uint64_t)13, rrr.select0(8), L"Select0 passed", LINE_INFO());
		}

	};
}