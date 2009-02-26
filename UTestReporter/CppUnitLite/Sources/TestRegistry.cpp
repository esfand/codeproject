#include "Test.h"
#include "TestResult.h"
#include "TestRegistry.h"
#include "TVTInterface.h"
#include "xmlParser.h"

#ifndef null
#define	null	0
#endif

#define	APPL_ID	TEXT("TestApp")

TestRegistry::TestRegistry()
: testsNumber_(0), tests_(null), last_(null)
{
	bool
	result	= TVTStartConnection(APPL_ID);
}

TestRegistry::~TestRegistry()
{
//	Final connection...
	bool
	result	= TVTFinalConnection(APPL_ID);
}

void TestRegistry::AddTest (ITest *test) 
{
	Instance().Add(test);
}

void TestRegistry::RunAllTests (TestResult& result) 
{
	Instance().Run(result);
}

TestRegistry& TestRegistry::Instance () 
{
	static TestRegistry registry;
	return registry;
}

void TestRegistry::Add(ITest *test) 
{
	testsNumber_++;
	if (tests_ == null)
	{
		tests_	= test;
		last_	= test;
		TVTSendTestInfo(test->Name(),test->Group());
		return;
	}
//@	test->SetNext(tests_);
//@	tests_ = test;
	last_->SetNext(test);
	last_	= test;

//	Send Test info
	TVTSendTestInfo(test->Name(),test->Group());
}

void TestRegistry::Run (TestResult& result) 
{
	result.TestsStarted ();

	for (ITest *test = tests_; test != 0; test = test->GetNext())
	{
		test->Run(result);
		test->Analysis(result.Statistics(),result.Trace());
	}

	Finalize(result.Failures(),result.Summary());
	result.TestsEnded(testsNumber_);
	
}

LPVOID	TVTCreateXMLPacket4FinalizeSession	(TCHAR*	sessionName,int, int, int	numberOfTests,int	numberOfSuccessfulTests,int	numberOfUnsuccessfulTests);
void	TVTDeletePacket						(LPVOID	packet);
bool	GetTemplateActionPath				(TCHAR*	pApplication);

void	TestRegistry::Finalize(
		int	failureCount,
		int	summaryCount)
{
	std::string		packet	= "";
	int		numberOfTests	= 0,
	numberOfSuccessfulTests	= 0,
numberOfUnsuccessfulTests	= 0
;
	for (ITest *test = tests_; test != 0; test = test->GetNext())
	{
		numberOfTests++;
		if (test->Failed())
			numberOfUnsuccessfulTests++;
		else
			numberOfSuccessfulTests++
		;
	}
	
	LPVOID	tempPacket	= 
	TVTCreateXMLPacket4FinalizeSession(
		APPL_ID,failureCount,summaryCount,
		numberOfTests,numberOfSuccessfulTests,numberOfUnsuccessfulTests);
	bool	result	= TVTFinalize((const char*)tempPacket);
	TVTDeletePacket(tempPacket)
	;
}


LPVOID	TVTCreateXMLPacket4FinalizeSession(
	TCHAR*	sessionName,
		int	failureCount,
		int	summaryCount,
		int	numberOfTests,
		int	numberOfSuccessfulTests,
		int	numberOfUnsuccessfulTests)
{
	TCHAR*	result	= null,
		templatePath[MAX_PATH],
			tempText[MAX_PATH]	
	;
	if (!GetTemplateActionPath(templatePath)
	)	return	result
	;
//	This open and parse the XML file:  
	XMLNode	xRootNode	= XMLNode::openFileHelper(templatePath,"easyTest");
	XMLNode	xAction		= xRootNode.getChildNode("Action");
	XMLAttribute*	xAttribute	= xAction.AddAttribute(stringDup("command",0),stringDup("updateObject",	0));
	XMLNode	xAttributes	= xAction.getChildNode("Attributes");
	xAttribute	= xAttributes.AddAttribute(stringDup("object",	0),stringDup("session",		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("session",	0),stringDup(sessionName,	0));
	
	XMLNode	xEntity	= xAction.AddChild(stringDup("Entity"),0);

//	Line 'Conclusion'
	XMLNode	xLine	= TVTAddXMLLine(xEntity);
	TVTAddXMLToken(xLine,"Conclusion: ");
	TVTAddXMLToken(xLine,itoa(summaryCount,tempText,10));
	TVTAddXMLToken(xLine," checks were made. There were ");
	TVTAddXMLToken(xLine,itoa(failureCount,tempText,10));
	TVTAddXMLToken(xLine," failures.");
	
//	Line 'Statistics'
	xLine	= TVTAddXMLLine(xEntity);
	TVTAddXMLToken(xLine,"Statistics:");
//	Line # tests are executes.
	xLine	= TVTAddXMLLine(xEntity);
	TVTAddXMLToken(xLine,itoa(numberOfTests,tempText,10));
	if (numberOfTests == 1)
		TVTAddXMLToken(xLine," test is executed.");
	else
		TVTAddXMLToken(xLine," tests are executed.")
	;
	xLine	= TVTAddXMLLine(xEntity);
	if (!numberOfUnsuccessfulTests)
	{
			TVTAddXMLToken(xLine,"No errors detected.");
	}
	else
	{
			TVTAddXMLToken(xLine,itoa(numberOfSuccessfulTests,tempText,10));
			if (numberOfSuccessfulTests == 1)
				TVTAddXMLToken(xLine," test is completed successfully. ");
			else
				TVTAddXMLToken(xLine," tests are completed successfully. ")
			;
		xLine	= TVTAddXMLLine(xEntity);
			TVTAddXMLToken(xLine,itoa(numberOfUnsuccessfulTests,tempText,10));
			if (numberOfUnsuccessfulTests == 1)
				TVTAddXMLToken(xLine," test failed.");
			else
				TVTAddXMLToken(xLine," tests failed.")
			;
	}

//	Line
	int		size;
	LPTSTR	pText	= xRootNode.createXMLString(1,&size);
	result	= new TCHAR [size+1];
	if (result)
	{
		_tcscpy(result,pText);
	}
	free(pText);
	return	result;
}

//	Data transmission to program TreeViewerTest.
//	Some reasons to the occasion of the interface.
//	16.06.2005,	Mk
/*
bool	StartSession	(<session_id>);				->	TestRegistry::TestRegistry():
		connection	= StartSession();
bool	FinalSession	(<session_id>);				->	TestRegistry::~TestRegistry():
bool	SendTestIdent	(<session_id>,name+group)	->	TestRegistry::Add()
*/
//	To construct a skeleton of a tree of tests,
//	enough these three functions 

