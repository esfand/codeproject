#include "Failure.h"
#include <stdio.h>
#include <string.h> 
#include <xmlParser.h> 

#pragma warning(disable : 4996)

//	It is necessary to form two types of messages.
//	The first message is a line for sending to stdout.
//	The second message is number of lines, generated as XML
//	for sending to Unit Test Indicator.

bool	GetTemplateEntityPath	(char*);
XMLNode	TVTAddXMLLine	(XMLNode&);
void	TVTAddXMLToken	(XMLNode&,TCHAR*);

void	Substitution(
		char*	textBuffer)
{
	for (int i = 0; i < lstrlen(textBuffer); i++)
	{
		if (textBuffer[i]	==	TCHAR('"'))
			textBuffer[i]	=	TCHAR(0xB4);	//0x27;
	}
}

EasyString	ReplaceString(
			const EasyString&	source)
{
	int		length	= source.Length();
	char*	pTextBuffer	= new char [length+1];
	lstrcpy	(pTextBuffer,source.AsCharString());
	Substitution(pTextBuffer);
	EasyString
	result	= EasyString(pTextBuffer);
	delete	[] pTextBuffer;
	return	result;
}

Failure::Failure(
		const EasyString&	theTestName,
		const EasyString&	theTestGroup,
		const EasyString&	theErrorReason,
		const EasyString&	theFileName, 
		long	 			theLineNumber,
		const EasyString&	theCondition) 
:	message_	(theCondition), 
	testName_	(theTestName), 
	testGroup_	(theTestGroup),
	errorReason_(theErrorReason),
	fileName_	(theFileName), 
	lineNumber_	(theLineNumber)
{
	EasyString	false_	= "false",
				true_	= "true",
				expect_,
				detect_
	;
	if (!stricmp("ASSERT_FALSE",theErrorReason.AsCharString()))
	{
		expect_	= false_;
		detect_	= true_;
	}
	else
	{
		expect_	= true_;
		detect_	= false_;
	}

	message_	= EasyString("Expected condition is '")
				+ expect_
				+ EasyString("', ")
				+ EasyString("detected condition '")
				+ theCondition
				+ EasyString("' is '")
				+ detect_
				+ EasyString("'.")
				;

	char	templatePath	[256],
			textBuffer		[256];
	if (!GetTemplateEntityPath(templatePath)
	)	return
	;
	XMLNode	xRootNode	= XMLNode::openFileHelper(templatePath,"easyTest");
	XMLNode	xEntity	= xRootNode.getChildNode("Entity");
	XMLNode	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Expected condition is ");
		TVTAddXMLToken(xLine,expect_.AsCharString());
		TVTAddXMLToken(xLine,",");
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"detected condition");
	xLine	= TVTAddXMLLine(xEntity);

		EasyString
		theCondition_	= ReplaceString(theCondition);

		EasyString
		subst	= EasyString("«") + theCondition_ + EasyString("»");

		TVTAddXMLToken(xLine,subst.AsCharString());
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"is ");
		TVTAddXMLToken(xLine,detect_.AsCharString());
		TVTAddXMLToken(xLine,".");
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Error is fixed in line ");
		TVTAddXMLToken(xLine,ltoa(theLineNumber,textBuffer,10));
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"in file ");
		TVTAddXMLToken(xLine,theFileName.AsCharString());
		TVTAddXMLToken(xLine,".");
	
	int		size	= 0;
	LPTSTR	pText	= xRootNode.createXMLString(1,&size);
	messageXML_	= EasyString(pText);
	free(pText);
}

Failure::Failure(
		const EasyString&	theTestName, 
		const EasyString&	theTestGroup,
		const EasyString&	theErrorReason,
		const EasyString&	theFileName, 
		long				theLineNumber,
		const EasyString&	expected,
		const EasyString&	actual) 
:	testName_	(theTestName), 
	fileName_	(theFileName), 
	errorReason_(theErrorReason),
	testGroup_	(theTestGroup), 
	lineNumber_	(theLineNumber)
{
	message_	= EasyString("Expected value is '")
				+ expected.AsCharString()
				+ EasyString("' detected value is '")
				+ actual.AsCharString()
				+ EasyString("'.")
				;

	char	templatePath	[256],
			textBuffer		[256]
	;
	if (!GetTemplateEntityPath(templatePath)
	)	return
	;
	XMLNode	xRootNode	= XMLNode::openFileHelper(templatePath,"easyTest");
	XMLNode	xEntity	= xRootNode.getChildNode("Entity");
	XMLNode	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Expected value is");
	xLine	= TVTAddXMLLine(xEntity);
	EasyString
	subst	= EasyString("«") + expected + EasyString("»");
		TVTAddXMLToken(xLine,subst.AsCharString());
		TVTAddXMLToken(xLine,",");
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"detected value is");
	xLine	= TVTAddXMLLine(xEntity);
	subst	= EasyString("«") + actual + EasyString("»");
		TVTAddXMLToken(xLine,subst.AsCharString());
		TVTAddXMLToken(xLine,".");
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Error is fixed in line ");
		TVTAddXMLToken(xLine,ltoa(theLineNumber,textBuffer,10));
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"in file ");
		TVTAddXMLToken(xLine,theFileName.AsCharString());
		TVTAddXMLToken(xLine,".");

	int		size	= 0;
	LPTSTR	pText	= xRootNode.createXMLString(1,&size);
	messageXML_	= EasyString(pText);
	free(pText);
}
