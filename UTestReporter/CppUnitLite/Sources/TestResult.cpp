#include "TestResult.h"
#include "Failure.h"
#include "Success.h"
#include <stdio.h>
#include "TVTInterface.h"

#ifndef	null
#define	null	0
#endif

#define	APPL_ID	TEXT("TestApp")

TestResult::TestResult()
	:	failureCount_	(0),
		allCheckCount_	(0),
		successCount_	(0),
		debug_		(false),
		statistics_	(false),
		trace_		(false)
{
}

TestResult::TestResult(
	const bool debugMode, const bool statMode, const bool traceMode)
	:	failureCount_	(0),
		allCheckCount_	(0),
		successCount_	(0),
		debug_	(debugMode),
	statistics_	(statMode),
		trace_	(traceMode)
{
}

void TestResult::TestsStarted () 
{
}

//	The test  is executed successfully
void TestResult::AddSuccess (const Success& success) 
{
	if (debug_ == false
	)	return
	;
	fprintf (stdout, "Test(%s,%s)\t[%s]\tis executed successfully: ",
		success.testName_,success.testGroup_,success.testMacro_);
	fprintf (stdout, "%s%s%s%s%ld%s%s\n",
		"\"",
		success.message_.AsCharString (),
		"\" " ,
		"line ",
		success.lineNumber_,
		" in ",
		success.fileName_.AsCharString ());

}

void TestResult::AddFailure (const Failure& failure) 
{
	fprintf (stdout, "[!] Test(%s,%s) is failed: ",failure.testName_,failure.testGroup_);
	fprintf (stdout, "%s%s%s%s%ld%s%s\n",
		"\"",
		failure.message_.AsCharString (),
		"\" " ,
		"line ",
		failure.lineNumber_,
		" in ",
		failure.fileName_.AsCharString ());
		
	failureCount_++;

//////////////////////////////////////////////////////////////////////////////
	TVTSendTestFailure(
		failure.testName_	.AsCharString(),
		failure.testGroup_	.AsCharString(),
		failure.errorReason_.AsCharString(),
		failure.messageXML_	.AsCharString(),
		failure.fileName_	.AsCharString(),
		failure.lineNumber_)
	;
}

void TestResult::TestsEnded(
		int testsNumber) 
{
	fprintf (stdout, "Statistics: %ld checks in %ld tests were made. ",allCheckCount_,testsNumber);
	if (failureCount_ > 0)
		fprintf (stdout, "There were %ld failures.\n", failureCount_);
	else
		fprintf (stdout, "There were no test failures.\n");
}

