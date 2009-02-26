///////////////////////////////////////////////////////////////////////////////
// FAILURE.H
// Failure is a class which holds information pertaining to a specific
// test failure.  The stream insertion operator is overloaded to allow easy
// display.
///////////////////////////////////////////////////////////////////////////////

#ifndef FAILURE_H
#define FAILURE_H

#include "SimpleString.h"

class Failure
{
public:
	Failure (const EasyString&	theTestName,
			 const EasyString&	theTestGroup, 
			 const EasyString&	theFileName, 
			 const EasyString&	theErrorReason,
			 long	  			theLineNumber,
			 const EasyString&	theCondition)
	;
	Failure (const EasyString&	theTestName,
			 const EasyString&	theTestGroup,
			 const EasyString&	theErrorReason,
			 const EasyString&	theFileName, 
			 long				theLineNumber,
			 const EasyString&	expected,
			 const EasyString&	actual)
	;
	EasyString	message_;
	EasyString	messageXML_;
	EasyString	testName_;
	EasyString	testGroup_;
	EasyString	fileName_;
	EasyString	errorReason_;
	long		lineNumber_;
};

#endif
