#include "pch.h"
#include "CppUnitTest.h"
#include "../minimalist_malloc/malloc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			custom_malloc(10);
		}
	};
}
