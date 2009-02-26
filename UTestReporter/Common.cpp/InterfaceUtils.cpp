#include	<Windows.h>
#include	<tchar.h>
#include	"TestObject.h"
#include	"xmlParser.h"
#include	"TDDOwner.h"

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
int		GetReportLinesNumber		(TestObject&);
XMLNode	TVTAddXMLLine				(XMLNode&);
void	TVTAddXMLToken				(XMLNode&,TCHAR*);

int	GetReportLinesNumber(
	TestObject&	object)
{
	int		result		= 0,
		linesInImage	= 0,
		linesInType		= 0,
		linesInIdent	= 0,
		linesInEntity	= 0
	;
	linesInImage	= GetLinesNumberInPlainText(object.Image ().c_str());
	linesInType		= GetLinesNumberInPlainText(object.Type	 ().c_str());
	linesInIdent	= GetLinesNumberInPlainText(object.Ident ().c_str());
	linesInEntity	= GetLinesNumberInPlainText(object.Entity().c_str());
	;
	result	= GetMax(linesInImage,linesInType,linesInIdent,linesInEntity)
	;
	return	result;
}

int	GetLinesNumberInPlainText(
	const TCHAR* pPlainTextBuffer)
{
	int		result	= 0,
			length	= 0
	;
	if (!pPlainTextBuffer
	)
	return	result
	;
	length	= static_cast<int>(_tcslen(pPlainTextBuffer));
	if (!length
	)
	return	result
	;
	for (int i = 0; i < length; i++)
	{
		if (pPlainTextBuffer[i] == TCHAR('\r'))
			result	+= 1;
	}
	
	result	+= 1;

	return	result;
}

int	GetMax	(int a, int	b, int c, int d)
{
	int	result	= 0;
	result	= max(a,max(b,max(c,d)));
	return	result;
}

bool	ExtractLineFromPlainText(
		int		lineNumber,
		TCHAR*	pExtractedLine,
		const
		TCHAR*	pPlainTextBuffer)
{
	bool	result	= false
	;
	TCHAR*	textBuffer	= null;
	int		length	=  0,
			index	= -1,
			idx		=  0
	;
	if (!pPlainTextBuffer
	||	!pExtractedLine
	)
	return	result
	;
	pExtractedLine[0]	= TCHAR(0)
	;
	length	= static_cast<int>(_tcslen(pPlainTextBuffer));
	if (!length
	)
	return	result
	;
//	Allocate memory
	textBuffer	= new TCHAR [length+1];
	if (!length
	)	return	result
	;
	memset (textBuffer,0x00,(length+1)*sizeof(TCHAR));
	for (int i = 0; i < length; i++)
	{
		if (pPlainTextBuffer[i] == TCHAR('\r'))
		{
			index++;
			if (index == lineNumber)
			{
				result	= true;
				break;
			}
		//	Clear buffer
			memset (textBuffer,0x00,(length+1)*sizeof(TCHAR));
			idx	= 0;
		}
		else
		{
			if (pPlainTextBuffer[i] != TCHAR('\n'))
				textBuffer[idx++]	= pPlainTextBuffer[i];
		}
	}

	index++;

	if (result)
		lstrcpy(pExtractedLine,textBuffer);
	else
	{
		if (index == lineNumber)
		{
			lstrcpy(pExtractedLine,textBuffer);
			result	= true;
		}
	}

	delete	textBuffer;

	return	result;
}

std::string	TransformXML2PlainText(
			XMLNode&	entityNode)
{
	std::string result	= "";
	LPTSTR		pText	= null;
	int	linesNumber	= entityNode.nChildNode();
	for (int i = 0; i < linesNumber; i++)
	{
	//	Get node of the line
		XMLNode	xSinkLine	= entityNode.getChildNode(i);
		int	tokensNumber	= xSinkLine.nChildNode();
		for (int j = 0; j < tokensNumber; j++)
		{
			XMLNode	xSinkToken	= xSinkLine.getChildNode(j);
			pText	= xSinkToken.getAttribute("text");
			if (pText)
				result += pText;
			else
				result += ""
			;
		}
		if (i < (linesNumber-1))
			result += "\r\n"
		;
	}
	return	result;
}

XMLNode	TransformPlainText2XML(
		std::string&	plainText)
{
	XMLNode	result;
	TCHAR	entityHelper[MAX_PATH],
			*textBuffer	= null,
		*plainTextBuffer= null,
			*pPtr		= null;
	bool	rc			= false;
	int		idx			= 0,
			length		= static_cast<int>(plainText.length())
	;
	if (!length
	)	return	result
	;
	plainTextBuffer	= (TCHAR*)plainText.c_str();
	if (!plainTextBuffer
	)	return	result
	;
//	Allocate memory
	textBuffer	= new TCHAR [length+1];
	if (!length
	)	return	result
	;
	rc	= GetTemplateEntityPath(entityHelper);
	if (!rc
	)	return	result
	;
	result	= XMLNode::openFileHelper(entityHelper,"easyTest")
	;
	XMLNode
	xEntity	= result.getChildNode("Entity");

	memset (textBuffer,0x00,(length+1)*sizeof(TCHAR));
	for (int i = 0; i < length; i++)
	{
		if (plainTextBuffer[i] == TCHAR('\r'))
		{
			XMLNode
			xLine	= TVTAddXMLLine(xEntity);
			TVTAddXMLToken(xLine,textBuffer);
		//	Clear buffer
			memset (textBuffer,0x00,(length+1)*sizeof(TCHAR));
			idx	= 0;
		}
		else
		{
			if (plainTextBuffer[i] != TCHAR('\n'))
				textBuffer[idx++]	= plainTextBuffer[i];
		}
	}
	XMLNode
	xLine	= TVTAddXMLLine(xEntity);
		TVTAddXMLToken(xLine,textBuffer)
	;
	delete	textBuffer;
	return	result;
}

//	In the package "xmlParser" there is no function by means of which it
//	is possible to add a subree to XML-node. Therefore it was necessary
//	to write this module. This function isn't universal. Only the subtree
//	with a level 2 must be added. It is necessary to write universal
//	recursive procedure.

bool	InsertEntity(
		XMLNode&	rootNode,
		XMLNode&	subNode)
{
	bool	result	= false
	;
	LPCTSTR	subNodeName	= subNode.getName();
	if (!subNodeName
	)	return	result
	;
	if (lstrcmp(subNodeName,"Entity")
	)	return	result
	;
	XMLNode
	entityNode	= rootNode.AddChild(stringDup(subNodeName),0);
	
	int
	numberOfLines	= subNode.nChildNode();
	
	for (int i = 0; i < numberOfLines; i++)
	{
		XMLNode
		lineNode	= subNode.getChildNode(i);
		LPCTSTR	lineName	= lineNode.getName();
		XMLNode
		addedLineNode	= entityNode.AddChild(stringDup((char*)lineName),0);
	//	Add attributes
		int	nAttributes	= lineNode.nAttribute();
		for (int iAttr = 0; iAttr < nAttributes; iAttr++)
		{
			XMLAttribute
			attribute	= lineNode.getAttribute(iAttr);
			addedLineNode.AddAttribute(
				stringDup(attribute.lpszName,0),
					stringDup(attribute.lpszValue,0))
			;
		}
	//	Add tokens
		int	numberOfTokens	= lineNode.nChildNode();
		for (int j = 0; j < numberOfTokens; j++)
		{
			XMLNode
			tokenNode	= lineNode.getChildNode(j);
			LPCTSTR	tokenName	= tokenNode.getName();
			XMLNode
			addedTokenNode	= addedLineNode.AddChild(stringDup((char*)tokenName),0);
		//	Add attributes
			int	nTokenAttributes	= tokenNode.nAttribute();
			for (int iTokenAttr = 0; iTokenAttr < nTokenAttributes; iTokenAttr++)
			{
				XMLAttribute
				tokenAttribute	= tokenNode.getAttribute(iTokenAttr);
				addedTokenNode.AddAttribute(
					stringDup(tokenAttribute.lpszName,0),
						stringDup(tokenAttribute.lpszValue,0))
				;
			}
		}
	}
	result	= true;
	return	result;
}

std::string	TransformXML2PlainText(
		std::string&	string)
{
	std::string	result	= "";
	XMLResults	xResults;
	XMLNode	sink	= XMLNode::parseString((char*)string.c_str(),"EasyTest",&xResults);	
//////////////////////////////////////////////////////////
//	The next line should be removed later. Function should
//	receive the pointer to "Entity" directly.
//////////////////////////////////////////////////////////
	XMLNode	xSinkAction	= sink.getChildNode("Action");
//	Get access to child 'Entity'
	XMLNode
	xSinkEntity	= xSinkAction.getChildNode("Entity");
//	Get number of lines
	int	linesNumber	= xSinkEntity.nChildNode();
	for (int i = 0; i < linesNumber; i++)
	{
	//	Get node of the line
		XMLNode	xSinkLine	= xSinkEntity.getChildNode(i);
		int	tokensNumber	= xSinkLine.nChildNode();
		for (int j = 0; j < tokensNumber; j++)
		{
			XMLNode	xSinkToken	= xSinkLine.getChildNode(j);
			result += xSinkToken.getAttribute("text");
		}
		result += "\r\n";
	}
	return	result;
}
