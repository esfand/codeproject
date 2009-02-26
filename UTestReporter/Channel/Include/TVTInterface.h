#ifndef	TVT_INTERFACE_H
#define	TVT_INTERFACE_H

#include	<Windows.h>
#include	<tchar.h>

#include	"xmlParser.h"

#pragma warning(disable : 4996)

#define	TVT_START			0x01
#define	TVT_FINAL			0x02
#define	TVT_INSERT_TEST		0x03
#define	TVT_INSERT_MACRO	0x04
#define	TVT_UPDATE_TEST		0x05
#define	TVT_UPDATE_MACRO	0x06
#define	TVT_FINALIZE		0x07

DWORD	TVTSessionID		(void);
LPVOID	TVTCreatePacket		(TCHAR*,int&);
bool	TVTStartConnection	(TCHAR*);
bool	TVTFinalConnection	(TCHAR*);
bool	TVTFinalize			(const char*);
bool	TVTInsertTest		(TCHAR*);
bool	TVTInsertMacro		(TCHAR*,TCHAR*,TCHAR*,TCHAR*,int);
bool	TVTUpdateTest		(TCHAR*,TCHAR*,TCHAR*,TCHAR*,TCHAR*);
bool	TVTUpdateMacro		(TCHAR*,TCHAR*,TCHAR*,TCHAR*);
void	TVTSendTestInfo		(char*,char*);
void	TVTSendTestSuccess	(char*,char*,int,int,int,int,int,int,int);
void	TVTSendTestError	(char*,char*,int,int,int,int,int,int,int);
void	TVTSendTestFailure	(TCHAR*,TCHAR*,TCHAR*,TCHAR*,TCHAR*,int);
XMLNode	TVTAddXMLLine		(XMLNode&);
void	TVTAddXMLToken		(XMLNode&,TCHAR*);

#include <string>
using namespace std;

std::string	TransformXML2PlainText(std::string&	string);
std::string	TransformXML2PlainText(XMLNode&	entityNode);

class TestTask
{
public:

	TestTask()
	{
		TCHAR	activityEvent	[MAX_PATH];
		_stprintf(activityEvent,TEXT("ActivityEvent_Task_%8.8x"),TVTSessionID());
		hActivityEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("FaceCodeIsActive"));
	}

	~TestTask()
	{
		if (hActivityEvent)
			CloseHandle(hActivityEvent)
		;
	}
private:
//	Data
	HANDLE	hActivityEvent;
};

//	Data transmission to program TreeViewerTest. Some reasons to the
//	occasion of the interface.
//	16.06.2005,	Mk
//DWORD	TVTSessionID()
//{
//	return	GetCurrentProcessId();
//}

//	To construct a skeleton of a tree of tests,
//	enough these three functions 

#endif
