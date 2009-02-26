#include	<windows.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<tchar.h>
#include	"TDDOwner.h"
#include	"TVTInterface.h"
#include	"xmlParser.h"

#ifndef	null
#define	null	NULL
#endif

#define	APPL_ID				TEXT("TestApp")

TCHAR*	GenerateTestName				(TCHAR*,TCHAR*,TCHAR*);
TCHAR*	GenerateMacroName				(TCHAR*,int);
LPVOID	TVTCreateXMLPacket4StartSession	(TCHAR*);
LPVOID	TVTCreateXMLPacket4FinalSession	(TCHAR*);

//	->	InterfaceUtils.cpp
bool	GetApplicationPath				(TCHAR*);
bool	GetTemplateActionPath			(TCHAR*);
bool	GetTemplateEntityPath			(TCHAR*);
bool	GetTemplateStorePath			(TCHAR*);
TCHAR*	SubstituteText					(TCHAR*,TCHAR*,TCHAR*);
XMLNode ReplaceEntry2XMLNode			(XMLNode&,TCHAR*,XMLNode&);
bool	CreateKey						(TCHAR*);
bool	ExistKey						(TCHAR*);
bool	SetKey							(TCHAR*,TCHAR*,TCHAR*);
bool	GetKeyValue						(TCHAR*,TCHAR*,TCHAR*,int);

//	Trace
void	TraceLog						(LPCTSTR,...);

COPYDATASTRUCT	sendPacket;

LPVOID	TVTCreatePacket(
		TCHAR*	entity,
		int&	len)
{
	unsigned char*	result	= null;
	int*	ptrSize	= null
	;
	TCHAR*	ptrEntity
	;
	if (!entity
	)	return	result
	;
	int	sizeofTChar	= sizeof(TCHAR); 
	len	= (int)sizeof(int)	+ ((int)_tcslen(entity)+1)*sizeofTChar;
	result	= new unsigned char [len];
	if (!result
	)	return	result
	;
	ptrSize		= (int*)&result[0];
	ptrEntity	= (TCHAR*)&result[sizeof(int)]
	;
	*ptrSize	= sizeofTChar;
	_tcscpy(ptrEntity,entity)
	;
	return	result;
}

void	TVTDeletePacket(
		LPVOID	packet)
{
	if (packet)
		delete	packet
	;
}

HWND	GetReceiverId()
{
	TCHAR	keyValue	[MAX_PATH];
	HWND	hwnd	= null;
	bool	result	= GetKeyValue(TDD_SOFTWARE,TDD_HWND_KEY,keyValue,MAX_PATH-1);
	if (!result
	)	return	hwnd
	;
//@	return	reinterpret_cast<HWND>((DWORD)atoi(keyValue));	warning C4312
	return	reinterpret_cast<HWND>((__int64)atoi(keyValue));
	;
}

DWORD	TVTSessionID()
{
	return	GetCurrentProcessId();
}

HWND	ApplicationLoaded()
{
	HWND	result	= NULL;
	result	= FindWindow(NULL,UTEST_PANEL_TITLE);
	return	result;
}

BOOL CALLBACK EnumDrawChildProc(HWND,LPARAM);

bool	ApplicationIsReady(
		HWND	hParent)
{
	bool	result	= false;
	DWORD	rc	= 0x00;
	EnumChildWindows(hParent,EnumDrawChildProc,(LPARAM)&rc);
	result	= rc ? true : false;
	return	result;
}

BOOL	CALLBACK EnumDrawChildProc(
		HWND hwndChild, LPARAM lParam)
{
	TCHAR	className	[MAX_PATH];
	DWORD*	rc	= (DWORD*)lParam;
	if (rc)
	{
		if (GetClassName(hwndChild,className,MAX_PATH))
		{
			if (!_tcsicmp(className,TEXT("SysTreeView32")))
			{
				*rc	+= 1;
			}
		}
	}
	return	TRUE;
}

bool	WaitForStart(
		DWORD	timeLimit)	//	msec
{
	bool	result	= false;
	HWND	hAppWnd	= null,
			hRecWnd	= null
	;	
	DWORD	startTime	= GetTickCount();

	while (true)
	{
		hAppWnd	= ApplicationLoaded();
		if (hAppWnd)
		{
			result	= ApplicationIsReady(hAppWnd);
			if (result)
			{
				break;
			}
		}
		if ((GetTickCount() - startTime) > timeLimit)
		{
			break;
		}
		Sleep(0);
	}

	if (!result
	)	return	result
	;

	result	= false
	;
	while (true)
	{
		hRecWnd	= GetReceiverId();
		if (hRecWnd)
		{
			result	= true;
			break;
		}

		if ((GetTickCount() - startTime) > timeLimit)
		{
			break;
		}
		Sleep(0);
	}
	return	result;
}

bool	TVTStartConnection(
		TCHAR*	sessionName)
{
	bool	result	= false;
	TCHAR	application	[MAX_PATH];
	int		packetLength	= 0
	;
	if (!GetApplicationPath(application)
	)	return	result
	;
	HINSTANCE hInstance = ::ShellExecute(
		0,TEXT("open"),application,0,0,SW_SHOWNORMAL);
	if (hInstance <= (HINSTANCE)HINSTANCE_ERROR)
	{
		TraceLog("Failed to load application...");
		return FALSE;
	}

	WaitForStart(2000);	//	Wait... 

	HWND
	receiver	= GetReceiverId();
	if (!receiver
	)	return	result
	;

	LPVOID	tempPacket	= TVTCreateXMLPacket4StartSession(sessionName);
	LPVOID	packet	= TVTCreatePacket((TCHAR*)tempPacket,packetLength);
	if (!packet
	)	return	result
	;

	sendPacket.dwData	= TVT_START;
	sendPacket.cbData	= packetLength;
	sendPacket.lpData	= packet;

	LRESULT	rc	= SendMessage(
		receiver,WM_COPYDATA,static_cast<WPARAM>(TVTSessionID()),reinterpret_cast<LPARAM>(reinterpret_cast<LPVOID>(&sendPacket)))
	;
	result	= (rc == TRUE) ? true : false
	;
	TVTDeletePacket(packet);
	TVTDeletePacket(tempPacket);

	return	result;
}

bool	TVTFinalConnection(
		TCHAR*	sessionName)
{
	bool	result	= false;
	int		packetLength	= 0
	;
	HWND	receiver	= GetReceiverId();
	if (!receiver
	)	return	result
	;
	LPVOID	tempPacket	= TVTCreateXMLPacket4FinalSession(sessionName);
	LPVOID	packet	= TVTCreatePacket((TCHAR*)tempPacket,packetLength);
	if (!packet
	)	return	result
	;

	sendPacket.dwData	= TVT_FINAL;
	sendPacket.cbData	= packetLength;
	sendPacket.lpData	= packet;

	LRESULT	rc	= SendMessage(
		receiver,WM_COPYDATA,static_cast<WPARAM>(TVTSessionID()),reinterpret_cast<LPARAM>(reinterpret_cast<LPVOID>(&sendPacket)))
	;
	result	= (rc == TRUE) ? true : false
	;
	TVTDeletePacket(packet)
	;
	TVTDeletePacket(tempPacket)
	;
	return	result
	;
}

bool	TVTFinalize(
		const char* text)
{
	bool	result	= false;
	int		packetLength	= 0
	;
	HWND	receiver	= GetReceiverId();
	if (!receiver
	)	return	result
	;
	LPVOID	packet	= TVTCreatePacket((TCHAR*)text,packetLength);
	if (!packet
	)	return	result
	;

	sendPacket.dwData	= TVT_FINALIZE;
	sendPacket.cbData	= packetLength;
	sendPacket.lpData	= packet;

	LRESULT	rc	= SendMessage(
		receiver,WM_COPYDATA,static_cast<WPARAM>(TVTSessionID()),reinterpret_cast<LPARAM>(reinterpret_cast<LPVOID>(&sendPacket)))
	;
	result	= (rc == TRUE) ? true : false
	;
	TVTDeletePacket(packet)
	;
	return	result
	;
}

bool	TVTInsertTest(
		TCHAR* testName)
{
	bool	result	= false;
	int		packetLength	= 0
	;
	if (!testName
	||	!_tcslen(testName)
	)	return	result
	;
	HWND	receiver	= GetReceiverId();
	if (!receiver
	)	return	result
	;
	LPVOID	packet	= TVTCreatePacket(testName,packetLength);
	if (!packet
	)	return	result
	;
	sendPacket.dwData	= TVT_INSERT_TEST;
	sendPacket.cbData	= packetLength;
	sendPacket.lpData	= packet;

	LRESULT	rc	= SendMessage(
		receiver,WM_COPYDATA,static_cast<WPARAM>(TVTSessionID()),reinterpret_cast<LPARAM>(reinterpret_cast<LPVOID>(&sendPacket)))
	;
	result	= (rc == TRUE) ? true : false
	;
	TVTDeletePacket(packet)
	;
	return	result;
}

TCHAR*	TVTCreateXMLPacket4InsertMacro(
		TCHAR*	testSession,
		TCHAR*	testName,
		TCHAR*	testGroup,
		TCHAR*	testMacro,
		int		lineNumber)
{
	TCHAR*	result	= null,
			templatePath[MAX_PATH],
			textBuffer	[MAX_PATH]
	;
	if (!testSession
	||	!_tcslen(testSession)
	||	!testName
	||	!_tcslen(testName)
	||	!testGroup
	||	!_tcslen(testGroup)
	||	!testMacro
	||	!_tcslen(testMacro)
	)	return	result
	;
	if (!GetTemplateActionPath(templatePath)
	)	return	result
	;
	XMLNode	xRootNode	= XMLNode::openFileHelper(templatePath,"easyTest");
	XMLNode	xAction		= xRootNode.getChildNode("Action");
	XMLAttribute*	xAttribute	= xAction.AddAttribute(stringDup("command",0),stringDup("insertObject",	0));
	XMLNode	xAttributes	= xAction.getChildNode("Attributes");
	xAttribute	= xAttributes.AddAttribute(stringDup("object",	0),stringDup("macro",		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("session",	0),stringDup(testSession,	0));
	xAttribute	= xAttributes.AddAttribute(stringDup("test",	0),stringDup(testName,		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("group",	0),stringDup(testGroup,		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("macro",	0),stringDup(testMacro,		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("line",	0),stringDup(itoa(lineNumber,textBuffer,10),0));
	
	int		size;
	LPTSTR	pText	= xRootNode.createXMLString(1,&size);
	result	= new TCHAR [size+1];
	if (result)
	{
		_tcscpy(result,pText);
	}
	free(pText);

	return	result
	;
}

XMLNode ReplaceEntry2XMLNode2(
		XMLNode&	xRootNode,
		TCHAR*		pEntry,
		TCHAR*		pEntity)
{
	XMLNode		result;
	XMLResults	xResults;
	int		size	= 0;
	LPTSTR	pText	= xRootNode.createXMLString(1,&size);
	if (!pText)
	{
		free(pText);
		return	result;
	}

	TCHAR*	pResultText	= SubstituteText(pText,pEntry,pEntity);
	if (!pResultText)
	{
		free(pText);
		return	result;
	}

	result	= XMLNode::parseString(pResultText,"EasyTest",&xResults);

	delete	[] pResultText;
	free(pText);
	return	result;
}

TCHAR*	TVTCreateXMLPacket4UpdateMacro(
		TCHAR*	testSession,
		TCHAR*	testName,
		TCHAR*	testGroup,
		TCHAR*	testMacro,
		int		lineNumber,
		TCHAR*	testEntity,
		TCHAR*	testResult)
{
	TCHAR*	result	= null,
			templatePath[MAX_PATH],
			textBuffer	[MAX_PATH]
	;
	XMLResults	xResults
	;
	if (!testSession
	||	!_tcslen(testSession)
	||	!testName
	||	!_tcslen(testName)
	||	!testGroup
	||	!_tcslen(testGroup)
	||	!testMacro
	||	!_tcslen(testMacro)
	||	!testEntity
	||	!_tcslen(testEntity)
	||	!testResult
	||	!_tcslen(testResult)
	)	return	result
	;
	if (!GetTemplateActionPath(templatePath)
	)	return	result
	;
	XMLNode	xRootNode	= XMLNode::openFileHelper(templatePath,"easyTest");
	XMLNode	xAction		= xRootNode.getChildNode("Action");
	XMLAttribute*
			xAttribute	= xAction.AddAttribute(stringDup("command",0),stringDup("updateObject",	0));
	XMLNode	xAttributes	= xAction.getChildNode("Attributes");
	xAttribute	= xAttributes.AddAttribute(stringDup("object",	0),stringDup("macro",		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("session",	0),stringDup(testSession,	0));
	xAttribute	= xAttributes.AddAttribute(stringDup("test",	0),stringDup(testName,		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("group",	0),stringDup(testGroup,		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("macro",	0),stringDup(testMacro,		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("line",	0),stringDup(itoa(lineNumber,textBuffer,10),0));
	xAttribute	= xAttributes.AddAttribute(stringDup("result",	0),stringDup(testResult,	0));

	XMLNode	xEntity	= xAction.AddChild(stringDup("Entity"),0);

	XMLNode	sink	= XMLNode::parseString(testEntity,"EasyTest",&xResults);
	XMLNode	xSinkEntity	= sink.getChildNode("Entity");

	XMLNode 
	xResultNode	= ReplaceEntry2XMLNode(xRootNode,"<Entity/>",xSinkEntity);

	int		size;
	LPTSTR	pText	= xResultNode.createXMLString(1,&size);

	result	= new TCHAR [size+1];
	if (result)
	{
		_tcscpy(result,pText);
	}
	free(pText);

	return	result
	;
}

TCHAR*	TVTCreateXMLPacket4UpdateTest(
		TCHAR*	testSession,
		TCHAR*	testName,
		TCHAR*	testGroup,
		TCHAR*	testMessage,
		TCHAR*	testResult,
		int		check,
		int		assertTrue,
		int		assertFalse,
		int		checkEqual,
		int		longsEqual,
		int		doublesEqual,
		int		failEqual)
{
	TCHAR*	result	= null,
			templatePath[MAX_PATH],
			textBuffer	[MAX_PATH]
	;
	int	checksNumber	= 0
	;
	if (!testSession
	||	!_tcslen(testSession)
	||	!testName
	||	!_tcslen(testName)
	||	!testGroup
	||	!_tcslen(testGroup)
	||	!testMessage
	||	!_tcslen(testMessage)
	||	!testResult
	||	!_tcslen(testResult)
	)	return	result
	;
	if (!GetTemplateActionPath(templatePath)
	)	return	result
	;
	XMLNode	xRootNode	= XMLNode::openFileHelper(templatePath,"easyTest");
	XMLNode	xAction		= xRootNode.getChildNode("Action");
	XMLAttribute*	xAttribute	= xAction.AddAttribute(stringDup("command",0),stringDup("updateObject",		0));
	XMLNode	xAttributes	= xAction.getChildNode("Attributes");
	xAttribute	= xAttributes.AddAttribute(stringDup("object",	0),stringDup("test",		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("session",	0),stringDup(testSession,	0));
	xAttribute	= xAttributes.AddAttribute(stringDup("test",	0),stringDup(testName,		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("group",	0),stringDup(testGroup,		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("result",	0),stringDup(testResult,	0));
	XMLNode	xEntity	= xAction.AddChild(stringDup("Entity"),0);
	
//	Line 'Statistics'
	XMLNode
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Result: ");
		TVTAddXMLToken(xLine,testMessage);
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Conclusion: ");
		
		checksNumber	= assertTrue+assertFalse+checkEqual+longsEqual+doublesEqual+failEqual;
		
		TVTAddXMLToken(xLine,itoa(checksNumber,textBuffer,10));
		if (checksNumber == 1)
			TVTAddXMLToken(xLine," check was made.");
		else
			TVTAddXMLToken(xLine," checks were made.")
	;
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Statistics:")
	;
	if (check)
	{
		xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Macro\tCHECK\t\twas used ");
		TVTAddXMLToken(xLine,itoa(check,textBuffer,10));
		TVTAddXMLToken(xLine," times.");
	}

	if (assertTrue)
	{
		xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Macro\tASSERT_TRUE\twas used ");
		TVTAddXMLToken(xLine,itoa(assertTrue,textBuffer,10));
		TVTAddXMLToken(xLine," times.");
	}

	if (assertFalse)
	{
		xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Macro\tASSERT_FALSE\twas used ");
		TVTAddXMLToken(xLine,itoa(assertFalse,textBuffer,10));
		TVTAddXMLToken(xLine," times.");
	}
	
	if (checkEqual)
	{
		xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Macro\tCHECK_EQUALS\twas used ");
		TVTAddXMLToken(xLine,itoa(checkEqual,textBuffer,10));
		TVTAddXMLToken(xLine," times.");
	}

	if (longsEqual)
	{
		xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Macro\tLONGS_EQUAL\twas used ");
		TVTAddXMLToken(xLine,itoa(longsEqual,textBuffer,10));
		TVTAddXMLToken(xLine," times.");
	}

	if (doublesEqual)
	{
		xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Macro\tDOUBLES_EQUAL\twas used ");
		TVTAddXMLToken(xLine,itoa(doublesEqual,textBuffer,10));
		TVTAddXMLToken(xLine," times.");
	}

	if (failEqual)
	{
		xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,"Macros\tFAIL_EQUAL\twas used ");
		TVTAddXMLToken(xLine,itoa(failEqual,textBuffer,10));
		TVTAddXMLToken(xLine," times.");
	}

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

bool	TVTUpdateTest(
		TCHAR*	testSession,
		TCHAR*	testName,
		TCHAR*	testGroup,
		TCHAR*	testMessage,
		TCHAR*	testResult,
		int		check,
		int		assertTrue,
		int		assertFalse,
		int		checkEqual,
		int		longsEqual,
		int		doublesEqual,
		int		failEqual
	)
{
	bool	result	= false;
	TCHAR*	pPacket	= null;
	int		packetLength	= 0
	;
	if (!testSession
	||	!_tcslen(testSession)
	||	!testName
	||	!_tcslen(testName)
	||	!testMessage
	||	!_tcslen(testMessage)
	||	!testResult
	||	!_tcslen(testResult)
	)	return	result
	;
	HWND	receiver	= GetReceiverId();
	if (!receiver
	)	return	result
	;
	pPacket	= TVTCreateXMLPacket4UpdateTest(
		testSession,testName,testGroup,testMessage,testResult,
		check,assertTrue,assertFalse,checkEqual,longsEqual,doublesEqual,failEqual);

	LPVOID	packet	= TVTCreatePacket(pPacket,packetLength);
	if (!packet
	)	return	result
	;
	sendPacket.dwData	= TVT_UPDATE_TEST;
	sendPacket.cbData	= packetLength;
	sendPacket.lpData	= packet;

	LRESULT	rc	= SendMessage(
		receiver,WM_COPYDATA,static_cast<WPARAM>(TVTSessionID()),reinterpret_cast<LPARAM>(reinterpret_cast<LPVOID>(&sendPacket)))
	;
	result	= (rc == TRUE) ? true : false
	;
	TVTDeletePacket(packet)
	;
	delete	[]	pPacket
	;
	return	result;
}

bool	TVTInsertMacro(
		TCHAR*	testSession,
		TCHAR*	testName,
		TCHAR*	testGroup,
		TCHAR*	testMacro,
		int		lineNumber)
{
	bool	result	= false;
	TCHAR*	pPacket	= null;
	int		packetLength	= 0
	;
	if (!testName
	||	!_tcslen(testName)
	||	!testMacro
	||	!_tcslen(testMacro)
	)	return	result
	;
	HWND	receiver	= GetReceiverId();
	if (!receiver
	)	return	result
	;
	pPacket	= TVTCreateXMLPacket4InsertMacro(
		testSession,testName,testGroup,testMacro,lineNumber);

	LPVOID	packet	= TVTCreatePacket(pPacket,packetLength);
	if (!packet
	)	return	result
	;
	sendPacket.dwData	= TVT_INSERT_MACRO;
	sendPacket.cbData	= packetLength;
	sendPacket.lpData	= packet;

	LRESULT	rc	= SendMessage(
		receiver,WM_COPYDATA,static_cast<WPARAM>(TVTSessionID()),reinterpret_cast<LPARAM>(reinterpret_cast<LPVOID>(&sendPacket)))
	;
	result	= (rc == TRUE) ? true : false
	;
	TVTDeletePacket(packet)	//@@@@@
	;
	delete	pPacket
	;
	return	result;
}

bool	TVTUpdateMacro(
		TCHAR*	testSession,
		TCHAR*	testName,
		TCHAR*	testGroup,
		TCHAR*	testMacro,
		int		lineNumber,
		TCHAR*	testEntity,
		TCHAR*	testResult)
{
	bool	result	= false;
	TCHAR*	pPacket	= null;
	int		packetLength	= 0;
	HWND	receiver= GetReceiverId();
	if (!receiver
	)	return	result
	;
	pPacket	= TVTCreateXMLPacket4UpdateMacro(
		testSession,testName,testGroup,testMacro,lineNumber,testEntity,testResult);
	if (!pPacket
	)	return	result
	;

	LPVOID	packet	= TVTCreatePacket(pPacket,packetLength);
	if (!packet
	)	return	result
	;
	sendPacket.dwData	= TVT_UPDATE_MACRO;
	sendPacket.cbData	= packetLength;
	sendPacket.lpData	= packet;

	LRESULT	rc	= SendMessage(
		receiver,WM_COPYDATA,static_cast<WPARAM>(TVTSessionID()),reinterpret_cast<LPARAM>(reinterpret_cast<LPVOID>(&sendPacket)))
	;
	result	= (rc == TRUE) ? true : false
	;
	TVTDeletePacket(packet)
	;
	delete	pPacket;
	result	= true;
	return	result;
}

void	TVTSendTestSuccess(
		char*	testName,
		char*	groupName,
		int		check,
		int		assertTrue,
		int		assertFalse,
		int		checkEqual,
		int		longsEqual,
		int		doublesEqual,
		int		failEqual)
{
	bool
	result	= TVTUpdateTest(APPL_ID,testName,groupName,
		TEXT("Test is executed successfully."),TEXT("Success"),
			check,assertTrue,assertFalse,checkEqual,longsEqual,doublesEqual,failEqual)
	;
}

void	TVTSendTestError(
		char*	testName,
		char*	groupName,
		int		check,
		int		assertTrue,
		int		assertFalse,
		int		checkEqual,
		int		longsEqual,
		int		doublesEqual,
		int		failEqual)
{
	bool
	result	= TVTUpdateTest(APPL_ID,testName,groupName,
		TEXT("Test failed."),TEXT("Error"),
			check,assertTrue,assertFalse,checkEqual,longsEqual,doublesEqual,failEqual)
	;
}

#define	SEPARATOR	TEXT("/")
#define	POINT		TEXT(".")
#define	MAX_UINT	TEXT("4294967295")
#define	WIDGETS		TEXT(" @[]")

TCHAR*	GenerateTestName(
		TCHAR*	applicationName,
		TCHAR*	testName,
		TCHAR*	testGroup)
{
	TCHAR*	result	= null;

	if (!applicationName
	||	!testName
	||	!testGroup
	)	return	result
	;
	int
	lenTestName	=	static_cast<int>(_tcslen(applicationName))
				+	static_cast<int>(_tcslen(SEPARATOR))
				+	static_cast<int>(_tcslen(testName))
				+	static_cast<int>(_tcslen(POINT))
				+	static_cast<int>(_tcslen(testGroup))
	;
	result	= new TCHAR	[lenTestName+1];
	if (result)
	{
		_tcscpy( result,applicationName);
		_tcscpy(&result[_tcslen(result)],TEXT("/"));
		_tcscpy(&result[_tcslen(result)],testName);
		_tcscpy(&result[_tcslen(result)],TEXT("."));
		_tcscpy(&result[_tcslen(result)],testGroup);	
	}
	return	result;
}

TCHAR*	GenerateMacroName(
		TCHAR*	reason,
		int		line)
{
	TCHAR*	result	= null;
	long
	lenMacroName	=	static_cast<int>(_tcslen(reason))
					+	static_cast<int>(_tcslen(WIDGETS))
					+	static_cast<int>(_tcslen(MAX_UINT))*2
					+	1
					;
	result	= new TCHAR [lenMacroName];
	if (result)
	{
		wsprintf(result,TEXT("%s @[%d]"),reason,line);
	}
	return	result;
}

void	TVTSendTestInfo(
		char*	testName,
		char*	testGroup)
{
	TCHAR	templatePath[MAX_PATH]
	;
	if (!GetTemplateActionPath(templatePath)
	)	return
	;
	XMLNode	xRootNode	= XMLNode::openFileHelper(templatePath,"easyTest");
	XMLNode	xAction		= xRootNode.getChildNode("Action");
	XMLAttribute*	xAttribute	= xAction.AddAttribute(stringDup("command",0),stringDup("insertObject",		0));
	XMLNode	xAttributes	= xAction.getChildNode("Attributes");
	xAttribute	= xAttributes.AddAttribute(stringDup("object",	0),stringDup("test",	0));
	xAttribute	= xAttributes.AddAttribute(stringDup("session",	0),stringDup(APPL_ID,	0));
	xAttribute	= xAttributes.AddAttribute(stringDup("test",	0),stringDup(testName,	0));
	xAttribute	= xAttributes.AddAttribute(stringDup("group",	0),stringDup(testGroup,	0));
	int		size;
	LPTSTR	pText	= xRootNode.createXMLString(1,&size);
	bool
	result	= TVTInsertTest(pText)
	;
	free(pText)
	;
}

void	TVTSendTestFailure(
		TCHAR*	testName,
		TCHAR*	testGroup,
		TCHAR*	errorReason,
		TCHAR*	errorMessage,
		TCHAR*	fileName,
		int		lineNumber)
{
	TCHAR*	complexTestName	= GenerateTestName(APPL_ID,testName,testGroup);
	if (!complexTestName
	)	return
	;

	TCHAR*	complexMacroName	= GenerateMacroName(errorReason,lineNumber);
	if (!complexMacroName)
	{
		delete	[]	complexTestName;
		return;
	}

	bool
	result	= TVTInsertMacro(APPL_ID,testName,testGroup,errorReason,lineNumber);
	result	= TVTUpdateMacro(
		APPL_ID,testName,testGroup,errorReason,lineNumber,errorMessage,TEXT("Error"));

	delete	[]	complexMacroName;

}

LPVOID	TVTCreateXMLPacket4StartSession(
		TCHAR*	sessionName)
{
	TCHAR	templatePath	[MAX_PATH];
	TCHAR*	result	= null
	;
	if (!GetTemplateActionPath(templatePath)
	)	return	result
	;
	XMLNode	xRootNode	= XMLNode::openFileHelper(templatePath,"easyTest");
	XMLNode	xAction		= xRootNode.getChildNode("Action");
	XMLAttribute*	xAttribute	= xAction.AddAttribute(stringDup("command",0),stringDup("insertObject",		0));
	XMLNode	xAttributes	= xAction.getChildNode("Attributes");
	xAttribute	= xAttributes.AddAttribute(stringDup("object",	0),stringDup("session",		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("session",	0),stringDup(sessionName,	0));
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

LPVOID	TVTCreateXMLPacket4FinalSession(
		TCHAR*	sessionName)
{
	TCHAR	templatePath	[MAX_PATH];
	TCHAR*	result	= null
	;
	if (!GetTemplateActionPath(templatePath)
	)	return	result
	;
	XMLNode	xRootNode	= XMLNode::openFileHelper(templatePath,"easyTest");
	XMLNode	xAction		= xRootNode.getChildNode("Action");
	XMLAttribute*	xAttribute	= xAction.AddAttribute(stringDup("command",0),stringDup("deleteObject",		0));
	XMLNode	xAttributes	= xAction.getChildNode("Attributes");
	xAttribute	= xAttributes.AddAttribute(stringDup("object",	0),stringDup("session",		0));
	xAttribute	= xAttributes.AddAttribute(stringDup("session",	0),stringDup(sessionName,	0));
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

///////////////////////////////////////////////////////////////////////////
//	Trace
///////////////////////////////////////////////////////////////////////////
void	WriteInfo(
		TCHAR	*pFile,
		TCHAR	*pText)
{
	SYSTEMTIME
			m_CurrentTime
	;
	FILE	*pDestFile
			= NULL
	;
	MEMORYSTATUS
			MemoryStatus
	;

	if (!pFile
	||	!_tcslen(pFile)
	||	!pText
	||	!_tcslen(pText)
	)	return
	;
	GlobalMemoryStatus (&MemoryStatus);
	pDestFile	= _tfopen(pFile, _T("a+"));
	if (!pDestFile)
		return
	;
	GetLocalTime(&m_CurrentTime);

	_ftprintf(pDestFile,_T("%2.2d/%2.2d/%4.4d %02d:%02d:%02d,%03d [%d] == %s\n"),
		m_CurrentTime.wMonth,	m_CurrentTime.wDay,		m_CurrentTime.wYear,
		m_CurrentTime.wHour,	m_CurrentTime.wMinute,	m_CurrentTime.wSecond,
		m_CurrentTime.wMilliseconds, MemoryStatus.dwAvailPhys/1024,
		pText
	);

	fclose (pDestFile);
}

#define	MAX_BUFF_LEN	4096
bool	__bLogFile	= false;	//!!!true
char	__tLogFile	[MAX_PATH];
void	TraceLog(
		LPCTSTR	lpszFormat, ...)
{
	char	tBuffer	[MAX_BUFF_LEN];
	int		rc	= 0x00
	;	
	if (!__bLogFile
	)	return
	;
	va_list	args;
	va_start (args,lpszFormat);

	rc	= _vstprintf(tBuffer,lpszFormat,args);
	if (rc < MAX_BUFF_LEN)
	{
		_tcscpy	(__tLogFile,"Trace.log");
		WriteInfo	(__tLogFile,tBuffer);
	}
	va_end(args);
}
///////////////////////////////////////////////////////////////////////////
