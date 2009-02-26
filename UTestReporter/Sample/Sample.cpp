#include "TestHarness.h"

#include	<string>
using namespace std;

int main()
{
	TestResult	tr(false,false,true);
	TestRegistry::RunAllTests(tr);
	return 0;
}

TEST(TestCheck,Sample)
{
	int		expectedInt	= 1;
	CHECK((string(typeid(expectedInt).name()) == "int"));
	CHECK((string(typeid(expectedInt).name()) == "double"));
}

TEST(TestAssertTrue,Sample)
{
	int		expectedInt	= 1;
	ASSERT_TRUE((string(typeid(expectedInt).name()) == "integer"));
	ASSERT_TRUE((string(typeid(expectedInt).name()) == "int"));
}

TEST(TestAssertFalse,Sample)
{
	bool	expectedDouble	= false;
	ASSERT_FALSE( (string(typeid(expectedDouble).name()) == ""));
	ASSERT_FALSE(!(string(typeid(expectedDouble).name()) == "bool"));
}

TEST(TestCheckEqual,Sample)
{
	int		expectedInt		=  12;
	string	expectedString	= "12";
	CHECK_EQUAL(expectedInt,expectedString);
}

TEST(TestLongsEqual,Sample)
{
	long	expectedLong	=  1234;
	LONGS_EQUAL(expectedLong,1000+2*100+3*10+4);
}

TEST(TestDoublesEqual,Sample)
{
	double	dExpected		= 1234.56,
			dChecked		= 1234.06,
			dFirstTreshold	= 0.30,
			dSecondTreshold	= 0.60
	;
	DOUBLES_EQUAL(dExpected,dChecked,dFirstTreshold);
	DOUBLES_EQUAL(dExpected,dChecked,dSecondTreshold);
}

TEST (TestCyclicError,Sample)
{
	int	numberOfPasses	= 16;

	for (int i = 0; i < numberOfPasses; i++)
	{
		CHECK_EQUAL(i,i+1);
		ASSERT_TRUE(i == (i+1));
	}
}

TEST_FUNCTION(SimpleFunc)
{
	int		a	= 8;
	CHECK_EQUAL		(3,a);
	long	b	= 777677;
	LONGS_EQUAL		(777677,b);
	DOUBLES_EQUAL	(1.88587,1.9,0.01);
}

TEST(Test,Function)
{
	USE(SimpleFunc);
}
