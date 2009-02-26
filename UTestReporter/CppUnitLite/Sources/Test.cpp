

#include "Test.h"
#include "TestRegistry.h"
#include "TestResult.h"
#include "Failure.h"
#include "Success.h"
#include "TVTInterface.h"

#ifndef	null
#define	null	0
#endif

ITest::ITest (const EasyString& testName, const EasyString& testGroup)
:	name_				(testName),
	group_				(testGroup),
	next_				(null),
	error_				(false),
	continue_if_error_	(true),
	check_				(0),
	assertTrue_			(0),
	assertFalse_		(0),
	checkEqual_			(0),
	longsEqual_			(0),
	doublesEqual_		(0),
	failEqual_			(0)
{
	test_	= this;
	TestRegistry::AddTest(this);
}

ITest::~ITest()
{
	int	x	= 0;
}

ITest *ITest::GetNext() const
{
	return next_;
}

void ITest::SetNext(ITest *test)
{	
	next_ = test;
}

bool ITest::Check(long expected, long actual, TestResult& result, const EasyString& fileName, long lineNumber)
{
	result.AddCheckPoint();
	if (expected == actual)
	{
		result.AddSuccess(
			Success(name_,group_,"Check",
			StringFrom(__FILE__),__LINE__,
			StringFrom(expected),StringFrom(actual)));	
		return true;
	}
	result.AddFailure(
		Failure (
			name_,
			group_,
			"?",
			StringFrom (__FILE__), 
			__LINE__, 
			StringFrom (expected), 
			StringFrom (actual)));

	return false;

}

bool ITest::Check(const EasyString& expected, const EasyString& actual, TestResult& result, const EasyString& fileName, long lineNumber)
{
	result.AddCheckPoint();
	if (expected == actual)
	{
		result.AddSuccess(
			Success(name_,group_,"Check",
					StringFrom(__FILE__),__LINE__,
					expected,actual)
		);
		return true;
	}
	result.AddFailure (
		Failure(
			name_,
			group_,
			"?",
			StringFrom (__FILE__), 
			__LINE__, 
			expected, 
			actual));
	return false;
}

void	OutStatLine(
		const EasyString& macroName,
		const int	macroCount)
{
	if (macroCount > 0)
		fprintf(stdout,"Macro '%s' is used %d times.\n",macroName,macroCount)
	;
}

void	ITest::Statistics(
		const bool	enable)
{
	if (enable == false
	)	return
	;
	OutStatLine	("CHECK",		check_			);
	OutStatLine	("ASSERT_TRUE",	assertTrue_		);
	OutStatLine	("ASSERT_FALSE",assertFalse_	);
	OutStatLine	("CHECK_EQUAL",	checkEqual_		);
	OutStatLine	("LONGS_EQUAL",	longsEqual_		);
	OutStatLine	("DOUBLES_EQUAL",doublesEqual_	);
	OutStatLine	("FAIL",		failEqual_		);
}

