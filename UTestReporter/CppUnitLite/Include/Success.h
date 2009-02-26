///////////////////////////////////////////////////////////////////////////////
//
// Success.h
//
// Failure is a class which holds information pertaining to a specific
// test failure.  The stream insertion operator is overloaded to allow easy
// display.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SUCCESS_H
#define SUCCESS_H

#include "SimpleString.h"

class Success
{
public:
	Success (
		const	EasyString&	theTestName,
		const	EasyString&	theTestGroup,
		const	EasyString&	theTestMacro,
		const	EasyString&	theFileName, 
		const	long	  		theLineNumber,
		const	EasyString&	theCondition)
	;

	Success (
		const	EasyString&	theTestName,
		const	EasyString&	theTestGroup,
		const	EasyString&	theTestMacro,
		const	EasyString&	theFileName, 
		const	long			theLineNumber,
		const	EasyString&	expected,
		const	EasyString&	actual)
	;
	EasyString	message_;
	EasyString	testName_;
	EasyString	testGroup_;
	EasyString	testMacro_;
	EasyString	fileName_;
	long			lineNumber_;
};

#endif
