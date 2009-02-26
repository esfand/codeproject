#include "Success.h"
#include <stdio.h>
#include <string.h> 

#pragma warning(disable : 4996)

Success::Success(
	const	EasyString&	theTestName,
	const	EasyString&	theTestGroup,
	const	EasyString&	theTestMacro,
	const	EasyString&	theFileName, 
	const	long	  		theLineNumber,
	const	EasyString&	theCondition
	)
:	testName_	(theTestName)	,
	testGroup_	(theTestGroup)	,
	testMacro_	(theTestMacro)	,
	message_	(theCondition)	,
	fileName_	(theFileName)	, 
	lineNumber_	(theLineNumber)
{
}

Success::Success(
	const	EasyString&	theTestName,
	const	EasyString&	theTestGroup,
	const	EasyString&	theTestMacro,
	const	EasyString&	theFileName, 
	const	long	  		theLineNumber,
	const	EasyString&	expected,
	const	EasyString&	actual
	)
:	testName_	(theTestName)	,
	testGroup_	(theTestGroup)	,
	testMacro_	(theTestMacro)	,
	fileName_	(theFileName)	, 
	lineNumber_	(theLineNumber)
{
	char *part1 = " ";
	char *part3 = " == ";

	char *stage = new char [strlen (part1) 
					+ expected.Length () 
					+ strlen (part3)
					+ actual.Length ()
					+ 1];

	sprintf(stage, "%s%s%s%s", 
		part1, 
		expected.AsCharString(), 
		part3, 
		actual.AsCharString());

	message_ = EasyString(stage);

	delete [] stage;
}
