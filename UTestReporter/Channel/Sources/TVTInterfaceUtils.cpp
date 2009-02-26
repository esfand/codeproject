#include	<Windows.h>
#include	<tchar.h>
#include	"xmlParser.h"
#include	"TDDOwner.h"

#pragma warning(disable : 4996)

#ifndef	null
#define	null	0
#endif

#include <string>
using namespace std;

//////////////////////////////////////////////////////////////////////////////
//	This file contains functions which are simultaneously used in projects
//	"EasyTest" and "EasyTestViewer"
//////////////////////////////////////////////////////////////////////////////

int		GetLinesNumberInPlainText	(const TCHAR*);
bool	ExtractLineFromPlainText	(int,TCHAR*,const TCHAR*);
int		GetMax						(int,int,int,int);
bool	GetApplicationPath			(TCHAR*);
bool	GetTemplateActionPath		(TCHAR*);
bool	GetTemplateEntityPath		(TCHAR*);
bool	GetTemplateStorePath		(TCHAR*);
bool	CreateKey					(TCHAR*);
bool	ExistKey					(TCHAR*);
bool	SetKey						(TCHAR*,TCHAR*,TCHAR*);
bool	GetKeyValue					(TCHAR*,TCHAR*,TCHAR*,int);

XMLNode	TVTAddXMLLine(
		XMLNode&	xEntity)
{
	TCHAR	textBuffer	[MAX_PATH];
	int		numberLine	= xEntity.nChildNode()+1
	;
	XMLNode	xLine	= xEntity.AddChild(stringDup("Line"),0);
	XMLAttribute*
	xLineAttribute	= xLine.AddAttribute(stringDup("number",0),
		stringDup(itoa(numberLine,textBuffer,10),0))
	;
	return	xLine;
}

void	TVTAddXMLToken(
		XMLNode&	xLine,
		TCHAR*		text)
{
	XMLNode
	xToken	= xLine.AddChild(stringDup("Token"),0);
		XMLAttribute*
		xTokenAttribute	= xToken.AddAttribute(stringDup("text",0),stringDup(text,0));
}

bool	GetApplicationPath(
		TCHAR*	pApplication)
{
	bool	result	= false;
	if (!pApplication
	)	return	result
	;
	pApplication[0]	= TCHAR(0);
	result	= GetKeyValue(TDD_COMPANY,TDD_LAUNCHER_KEY,pApplication,MAX_PATH-1);
	return	result;
}

bool	GetTemplateActionPath(
		TCHAR*	pApplication)
{
	bool	result	= false;
	TCHAR	tPath	[MAX_PATH],
			tFile	[MAX_PATH]
	;
	if (!pApplication
	)	return	result
	;
	pApplication[0]	= TCHAR(0);
	result	= GetKeyValue(TDD_COMPANY,TDD_TEMPLATES_KEY,tPath,MAX_PATH-1);
	if (!result
	)	return	result;
	result	= GetKeyValue(TDD_TEMPLATES,TDD_ACTION_KEY,tFile,MAX_PATH-1);
	if (!result
	)	return	result;
	lstrcpy( pApplication,tPath);
	lstrcpy(&pApplication[lstrlen(pApplication)],TEXT("\\"));
	lstrcpy(&pApplication[lstrlen(pApplication)],tFile)
	;
	return	result;
}

bool	GetTemplateEntityPath(
		TCHAR*	pApplication)
{
	bool	result	= false;
	TCHAR	tPath	[MAX_PATH],
			tFile	[MAX_PATH]
	;
	if (!pApplication
	)	return	result
	;
	pApplication[0]	= TCHAR(0);
	result	= GetKeyValue(TDD_COMPANY,TDD_TEMPLATES_KEY,tPath,MAX_PATH-1);
	if (!result
	)	return	result;
	result	= GetKeyValue(TDD_TEMPLATES,TDD_ENTITY_KEY,tFile,MAX_PATH-1);
	if (!result
	)	return	result;
	lstrcpy( pApplication,tPath);
	lstrcpy(&pApplication[lstrlen(pApplication)],TEXT("\\"));
	lstrcpy(&pApplication[lstrlen(pApplication)],tFile)
	;
	return	result;
}

bool	GetTemplateStorePath(
		TCHAR*	pApplication)
{
	bool	result	= false;
	TCHAR	tPath	[MAX_PATH],
			tFile	[MAX_PATH]
	;
	if (!pApplication
	)	return	result
	;
	pApplication[0]	= TCHAR(0);
	result	= GetKeyValue(TDD_COMPANY,TDD_TEMPLATES_KEY,tPath,MAX_PATH-1);
	if (!result
	)	return	result;
	result	= GetKeyValue(TDD_TEMPLATES,TDD_STORE_KEY,tFile,MAX_PATH-1);
	if (!result
	)	return	result;
	lstrcpy( pApplication,tPath);
	lstrcpy(&pApplication[lstrlen(pApplication)],TEXT("\\"));
	lstrcpy(&pApplication[lstrlen(pApplication)],tFile)
	;
	return	result;
}

TCHAR*	SubstituteText(
		TCHAR*	pSourceText,
		TCHAR*	pSimpleText,
		TCHAR*	pSubstitutedText)
{
	TCHAR*	result	= null;
	int		length	= (int)_tcslen(pSourceText)
					- (int)_tcslen(pSimpleText)
					+ (int)_tcslen(pSubstitutedText)
	;
	TCHAR*	pPtr	= _tcsstr(pSourceText,pSimpleText);
	if (!pPtr)
		return	result
	;
	result	= new TCHAR [length+1];
	memset	(result,0x00,(length+1)*sizeof(TCHAR));
   _tcsncpy	(result,pSourceText,(pPtr-pSourceText));
   _tcscpy	(&result[_tcslen(result)],pSubstitutedText);

   pPtr	+= _tcslen(pSimpleText);
   _tcscpy	(&result[_tcslen(result)],pPtr);

	return	result;
}

XMLNode ReplaceEntry2XMLNode(
		XMLNode&	xRootNode,
		TCHAR*		pEntry,
		XMLNode&	xSinkEntity)
{
	XMLNode		result;
	XMLResults	xResults;
	int		size	= 0;
	LPTSTR	pEntity	= xSinkEntity.createXMLString(1,&size);
	if (!pEntity
	)	return	result
	;
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
		free(pEntity);
		return	result;
	}

	result	= XMLNode::parseString(pResultText,"EasyTest",&xResults);

	delete	[] pResultText;
	free(pText);
	free(pEntity);
	return	result;
}

bool	GetKeyValue(
		TCHAR*	pKey,
		TCHAR*	pName,
		TCHAR*	pValue,
		int		len)
{
	bool	result		= false;
	TCHAR	pClassName	[MAX_PATH],
			pAuxValue	[MAX_PATH],
			pData		[MAX_PATH];
	LONG	rc			= ~ERROR_SUCCESS;
	HKEY	hSubKey		= NULL;
	DWORD	iValue		= 0,		//	Index value for RegEnumValue
			dwType		= 0,
			nValue		= 0,
			nData		= 0,
		dwcClassLen		= MAX_PATH,	//	Length of class string.
		dwcSubKeys		= 0,		//	Number of sub keys.
		dwcMaxSubKey	= 0,		//	Longest sub key size.
		dwcMaxClass		= 0,		//	Longest class string.
		dwcValues		= 0,		//	Number of values for this key.
		dwcMaxValueName	= 0,		//	Longest Value name.
		dwcMaxValueData	= 0,		//	Longest Value data.
		dwcSecDesc		= 0;		//	Security descriptor.
	FILETIME
			ftLastWriteTime			//	Last write time.
	;
	if (!pKey
	||	!_tcslen(pKey)
	||	!pName
	||	!_tcslen(pName)
	||	!pValue)
		return	result
	;
	pValue	[0] = TCHAR(0)
	;
	rc	= RegOpenKeyEx(
				HKEY_LOCAL_MACHINE,	// handle of open key 
	   (LPCTSTR)pKey,				// address of name of subkey to open 
				0,
				KEY_QUERY_VALUE,
				&hSubKey);			// address of handle of open key 
	
	if (rc != ERROR_SUCCESS)
		return	result
	;
	rc = RegQueryInfoKey(
		hSubKey,		// Key handle.
			pClassName,		// Buffer for class name.
				&dwcClassLen,	// Length of class string.
					NULL,			// Reserved.
						&dwcSubKeys,	// Number of sub keys.
							&dwcMaxSubKey,	// Longest sub key size.
								&dwcMaxClass,	// Longest class string.
						&dwcValues,			// Number of values for this key.
					&dwcMaxValueName,	// Longest Value name.
				&dwcMaxValueData,	// Longest Value data.
			&dwcSecDesc,		// Security descriptor.
		&ftLastWriteTime);	// Last write time.

	if (!dwcValues)
	{
		RegCloseKey (hSubKey);
		return	result;
	}

	while (true)
	{
		nValue	= sizeof (pAuxValue);
		nData	= sizeof (pData);
		rc	= RegEnumValue(
			hSubKey,
				iValue++,
					pAuxValue,
						&nValue,
							NULL,
								&dwType,
							(LPBYTE)pData,
										&nData)
		;
		if (rc == ERROR_NO_MORE_ITEMS)
			break
		;
		if (dwType == REG_SZ)
		{
			if (!_tcsicmp(pAuxValue,pName))
			{
				if (len >= (int)_tcslen(pData))
				{
					_tcscpy(pValue,pData);
					result	= true;
				}
				break;
			}
		}
	}

	RegCloseKey (hSubKey);

	return	result
	;
}

bool	SetKey(
		TCHAR	*pKey,
		TCHAR	*pValue,
		TCHAR	*pData)
{
	bool	result	= false;
	HKEY	hSubKey	= NULL;
	LONG	rc		= ~ERROR_SUCCESS
	;
	if (!pKey
	||	!_tcslen(pKey)
	||	!pValue
	||	!_tcslen(pValue)
	||	!pData
	||	!_tcslen(pData)
	)
	{
		return	result;
	}
	rc	= RegOpenKeyEx(
				HKEY_LOCAL_MACHINE,	// handle of open key 
	   (LPCTSTR)pKey,				// address of name of subkey to open 
				0,
				KEY_SET_VALUE,
				&hSubKey);			// address of handle of open key 
	
	if (rc != ERROR_SUCCESS)
	{
		return	result;
	}

	rc	= RegSetValueEx(
		hSubKey,
		pValue,
		NULL,
		REG_SZ,
		(BYTE * const)pData,
		(lstrlen(pData)+1)*sizeof(TCHAR))
	;
	RegCloseKey (hSubKey)
	;
	if (rc == ERROR_SUCCESS)
		result	= true
	;
	return	result
	;
}

bool	ExistKey(
		TCHAR*	pKey)
{
	bool	result	= false;
	HKEY	hSubKey	= null;
	LONG	rc		= ~ERROR_SUCCESS
	;
	if (!pKey
	||	!_tcslen(pKey)
	)	return	result
	;
	rc	= RegOpenKeyEx(
				HKEY_LOCAL_MACHINE,	// handle of open key 
	   (LPCTSTR)pKey,				// address of name of subkey to open 
				0,
				KEY_ALL_ACCESS,
				&hSubKey);			// address of handle of open key 
	
	if (rc != ERROR_SUCCESS
	)	return	result
	;
	RegCloseKey (hSubKey)
	;
	result	= true
	;
	return	result
	;
}

bool	CreateKey(
		TCHAR*	pKey)
{
	bool	result	= false;
	HKEY	hSubKey	= null;
	DWORD	dwDisp	= 0x00;
	LONG	rc		= ~ERROR_SUCCESS
	;
	if (!pKey
	||	!_tcslen(pKey)
	)	return	result
	;
	rc	= RegCreateKeyEx(
			HKEY_LOCAL_MACHINE, 
			pKey,0, "",
			REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,
			NULL,&hSubKey,&dwDisp)
	;
	if (rc != ERROR_SUCCESS
	)	return	result
	;
	RegCloseKey (hSubKey)
	;	
	result	= true
	;
	return	result
	;
}

