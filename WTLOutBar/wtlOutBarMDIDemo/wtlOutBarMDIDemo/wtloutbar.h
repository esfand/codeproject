/** \mainpage
 *	\subpage wtloutbar See wtloutbar for infos and the license of wtloutbar.h\n
 *	\subpage wtlgdiex See wtlgdiex for infos and the license of wtlgdiex.h\n
 *	\subpage stringcex See stringcex for infos and the license of stringcex.h\n
 */

/** \page wtloutbar wtloutbar.h
 *
 *  \section License License
 *  Copyright (c) 2005, 2006 Massimiliano Alberti <xanatos@geocities.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject
 *  to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  \section VersionHistory Version History
 *  \li wtloutbar.h   1.0   May 2005  Initial Release
 *  \li wtloutbar.h   1.1   May 2006  Small improvements, added comments
 *  \li wtloutbar.h   1.11  May 2006  Bugfix in the WM_DESTROY/WM_NCDESTROY handler. Put CMemoryStripDC in another header
 *  \li wtloutbar.h   1.12  Jun 2006  Corrected small memory leak (the HRGN wasn't deallocated). Changed the way the memory is freed during destruction.
 *
 *	\section Remember Remember!!!
 *  \li Add delayimp.lib to Linker->Input->Additional Dependencies
 *	\li Add uxtheme.dll to Linker->Input->Delay Loaded DLLs
 *
 *  \section TODO TODO
 *  \li Small icons
 *  \li Stateless version
 *  \li Tooltips
 *
 */

/** \file
 *  This header contains an implementation of a stateful Outlook bar.
 */

#pragma once

#ifndef __WTLOUTBAR_H__
#define __WTLOUTBAR_H__

#ifndef __STRINGCEX_H__
#include "stringcex.h"
#endif

#ifndef __WTLGDIEX_H__
#include "wtlgdiex.h"
#endif

#ifndef __ATLWIN_H__
#error wtloutbar.h requires atlwin.h to be included first
#endif

#ifndef __ATLTHEME_H__
#error wtloutbar.h requires atltheme.h to be included first
#endif

#ifndef TMSCHEMA_H
#error wtloutbar.h requires tmschema.h to be included first
#endif

#ifndef __ATLCOLL_H__
#error wtloutbar.h requires atlcoll.h to be included first
#endif

//

#pragma warning(push)
#pragma warning(disable : 4996) // Yes, I know ::strcpy/::strlen etc. are insecure, but I know what I'm doing (or so I hope)

//

namespace WTL
{

//

/// \name Window constants (Control Numbers, First Message, First and Last Notification)
/*@{*/
#define OB_FIRSTBTN				1000				///< The ID of the first Folder (Button)
#define OB_LISTVIEW				(OB_FIRSTBTN - 1)	///< The ID of the ListView

#define OBM_FIRST				(WM_USER + 1000)	///< First message used to control the OutBar
#define OBM_LAST				(OBM_FIRST + 100)	///< Last messages used to control the OutBar
#define OBN_FIRST				(0U-2000U)			///< Notifications used to control the OutBar
#define OBN_LAST				(0U-2100U)			///< Notifications used to control the OutBar
/*@}*/

/// \name Window Styles
/*@{*/
#define OBS_SHAREIMAGELISTS		0x0001				///< Style: the ImageList is shared with other "entities". It won't be destroyed when the OutBar is destroyed
#define OBS_FORCESELECT			0x0002				///< Style: there will always be a selected Item. If the program delete an Item the OutBar will select another Item
/*@}*/

/// \name Messages
/*@{*/
/// Inserts a Folder (or appends it at the end if wPram == OBM_GETFOLDERCOUNT)
/** \param wParam = 0. Not used
 *  \param lParam = (const OBFOLDER*)pFolder. An OBFOLDER struct describing the Folder
 *  \return = (int). The number of the Folder inserted, -1 if OBM_INSERTFOLDER fails
 */
#define OBM_INSERTFOLDER		(OBM_FIRST + 1)

/// Copy Folder data to an OBFOLDER struct
/** \param wParam = 0. Not used
 *  \param lParam = (OBFOLDER*)pFolder. An OBFOLDER struct that will receive the Folder
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_GETFOLDER			(OBM_FIRST + 2)

/// Sets Folder data (or creates a new Folder if OBFOLDER::iFolder == OBM_GETFOLDERCOUNT)
/** \param wParam = 0. Not used
 *  \param lParam = (const OBFOLDER*)pFolder. An OBFOLDER struct containing the new values to be used
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_SETFOLDER			(OBM_FIRST + 3)

/// Deletes a Folder
/** \param wParam = (int)iFolder. Folder number
 *  \param lParam = 0. Not used
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_DELETEFOLDER		(OBM_FIRST + 4)

/// Gets the number of Folders present in the OutBar
/** \param wParam = 0. Not used
 *  \param lParam = 0. Not used
 *  \return = (int). The number of Folders present in the OutBar
 */
#define OBM_GETFOLDERCOUNT		(OBM_FIRST + 5)

/// Allocates memory for a certain number of Folders (the Folders are NOT created)
/** \param wParam = (int)iFolderCount. Number of Folders to allocate memory for
 *  \param lParam = 0. Not used
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_SETFOLDERCOUNT		(OBM_FIRST + 6)

/// Deletes all the Folders (and their Items)
/** \param wParam = 0. Not used
 *  \param lParam = 0. Not used
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_DELETEALLFOLDERS	(OBM_FIRST + 7)

/// Inserts an Item in an existing Folder (or appends it at the end of the Folder if OBITEM::iItem == OBM_GETITEMCOUNT(iFolder))
/** \param wParam = 0. Not used
 *  \param lParam = (const OBITEM*)pItem. An OBITEM struct describing the Item
 *  \return = (int). The number of the Item inserted, -1 if OBM_INSERTITEM fails
 */
#define OBM_INSERTITEM			(OBM_FIRST + 10)

/// Copy Item data to an OBITEM struct
/** \param wParam = 0. Not used
 *  \param lParam = (OBITEM*)pItem. An OBItem struct that will receive the Item.
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_GETITEM				(OBM_FIRST + 11)

/// Sets Item data (or creates a new Item if OBITEM::iItem == OBM_GETITEMCOUNT(OBITEM::iFolder))
/** \param wParam = 0. Not used
 *  \param lParam = (const OBITEM*). pItem An OBITEM struct containing the new values to be used
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_SETITEM				(OBM_FIRST + 12)

/// Deletes an Item
/** \param wParam = (int)iFolder. Folder number
 *  \param lParam = (int)iItem. Item number
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_DELETEITEM			(OBM_FIRST + 13)

/// Gets the number of Items present in a Folder
/** \param wParam = (int)iFolder. Folder number
 *  \param lParam = 0. Not used
 *  \return = (int). The number of Items present in a Folder
 */
#define OBM_GETITEMCOUNT		(OBM_FIRST + 14)

/// Allocates memory for a certain number of Items in a Folder (the Items are NOT created)
/** \param wParam = (int)iFolder. Folder number
 *  \param lParam = (int)iItemCount. Number of Items to allocate memory for
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_SETITEMCOUNT		(OBM_FIRST + 15)

/// Deletes all the Items of a Folder
/** \param wParam = (int)iFolder. Folder number
 *  \param lParam = 0. Not used
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_DELETEALLITEMS		(OBM_FIRST + 16)

/// Gets the number of the Folder that is open (there is always an open Folder unless there are no Folders, then it returns -1)
/** \param wParam = 0. Not used
 *  \param lParam = 0. Not used
 *  \return = (int). The number of the Folder that is open
 */
#define OBM_GETOPENFOLDER		(OBM_FIRST + 20)

/// Opens a Folder
/** \param wParam = (int)iFolder. Folder number
 *  \param lParam = 0. Not used
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_SETOPENFOLDER		(OBM_FIRST + 21)

/// Gets the number of the Folder containing the selected Item
/** \param wParam = 0. Not used
 *  \param lParam = 0. Not used
 *  \return = (int). The number of the Folder that is selected
 */
#define OBM_GETSELECTEDFOLDER	(OBM_FIRST + 25)

/// Gets the number of the selected Item
/** \param wParam = 0. Not used
 *  \param lParam = 0. Not used
 *  \return = (int). The number of the Item that is selected
 */
#define OBM_GETSELECTEDITEM		(OBM_FIRST + 26)

/// Selects an Item
/** \param wParam = (int)iFolder. Folder number
 *  \param lParam = (int)iItem. Item number
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_SETSELECTEDITEM		(OBM_FIRST + 27)

/// Ensures that an Item is visible (opens it's Folder if necessary)
/** \param wParam = (int)iFolder. Folder number
 *  \param lParam = (int)iItem. Item number
 *  \return = (BOOL). If the method succeeds, the return value is nonzero
 */
#define OBM_ENSUREVISIBLE		(OBM_FIRST + 28)

/// Finds a Folder using the data given in lParam
/** To use this method you have to put in pFolder->iFolder the number of the Folder
 *  to start from, set the pFolder->mask using the OBFFF_* flags and fill some of the other
 *  fields with the data to look for. If a Folder is found pFolder->iFolder will contain its
 *  number and the return value will be nonzero. If no Folder is found but there isn't any
 *  error then pFolder->iFolder is -1 and the return value is 0. If there is an error the return
 *  value is 0 and the pFolder->iFolder value is the same it was at the moment of invocation.
 *  Note that cchTextMax is ignored.
 *  \param lParam = (OBFOLDER*)pFolder. An OBFOLDER struct that contains the data to seach, the Folder number to start from and that receives the number of the Folder searched for (or -1 if no Folder is found)
 *  \return = (BOOL). If the method succeeds and a Folder is found, the return value is nonzero.
 */
#define OBM_FINDFOLDER			(OBM_FIRST + 30)

/// Finds an Item using the data given in lParam
/** To use this method you have to put in pItem->iFolder the number of the Folder and in
 *  pItem->iItem the number of the Item to to start from, set the pItem->mask using the
 *  OBFIF_* flags and fill some of the other fields with the data to look for. If an Item
 *  is found pItem->iFolder and pItem->iItem will contain its number and the return value
 *  will be nonzero. If no Item is found but there isn't any error then pItem->iFolder and
 *  pItem->iItem are -1 and the return value is 0. If there is an error the return
 *  value is 0 and the pFolder->iFolder and pFolder iItem values are the same they were at the
 *  moment of invocation.
 *  Note that cchTextMax is ignored.
 *  \param lParam = (OBITEM*)pItem. An OBITEM struct that contains the data to seach, the Folder and Item number to start from and that receives the number of the Folder and Item searched for (or -1 if no Item is found)
 *  \return = (BOOL). If the method succeeds and an Item is found, the return value is nonzero.
 */
#define OBM_FINDITEM			(OBM_FIRST + 31)

/// Gets the HWND of a Folder (passing "-1" as the parameter will return the HWND of the ListView)
/** \param wParam = (int)iFolder. Folder number
 *  \param lParam = 0. Not used
 *  \return = (HWND). Handle to the control. NULL if there is an error
 */
#define OBM_GETHWNDHANDLE		(OBM_FIRST + 35)

/// Gets the color of the background of the ListView
/** \param wParam = 0. Not used
 *  \param lParam = 0. Not used
 *  \return = (COLORREF). Color of the background. CLR_DEFAULT for default color, CLR_NONE for transparent
 */
#define OBM_GETBKCOLOR			(OBM_FIRST + 40)

/// Sets the color of the background of the ListView
/** \param wParam = 0. Not used
 *  \param lParam = (COLORREF)clrBk. New background color. CLR_DEFAULT for default color, CLR_NONE for transparent
 *  \return = (COLORREF). Old color. CLR_INVALID if there is an error
 */
#define OBM_SETBKCOLOR			(OBM_FIRST + 41)

/// Gets the ImageList
/** \param wParam = (int)iImageListType. Type of ImageList. Legal values are OBSIL_NORMAL and OBSIL_SMALL (OBSIL_SMALL is NOT IMPLEMENTED)
 *  \param lParam = 0. Not used
 *  \return = (HIMAGELIST). Handle to the ImageList (in the form of a CImageList)
 */
#define OBM_GETIMAGELIST		(OBM_FIRST + 42)

/// Sets the ImageList
/** \param wParam = (int)iImageListType. Type of ImageList. Legal values are OBSIL_NORMAL and OBSIL_SMALL (OBSIL_SMALL is NOT IMPLEMENTED)
 *  \param lParam = (HIMAGELIST)hImageList. Handle to the new ImageList
 *  \return = (HIMAGELIST). Handle to the old ImageList (in the form of a CImageList)
 */
#define OBM_SETIMAGELIST		(OBM_FIRST + 43)
/*@}*/

/// \name Notifications
/*@{*/
/// A Folder has been opened. OBM_SETOPENFOLDER and OBM_ENSUREVISIBLE WILL NOT trigger this notification
#define OBN_FOLDEROPEN			(OBN_FIRST - 1)
/// The user has selected an Item. Reselecting the same Item WILL NOT trigger this notification
#define OBN_ITEMSELECT			(OBN_FIRST - 2)
/// The OBS_FORCESELECT is active and the OutBar had to automatically select an Item
#define OBN_ITEMSELECTAUTO		(OBN_FIRST - 3)
/*@}*/

/// \name Icon styles
/*@{*/
#define OBSIL_NORMAL			0					///< Standard Icons (32x32)
#define OBSIL_SMALL				1					///< NOT IMPLEMENTED
/*@}*/

/// \name Internal constants
/*@{*/
#define OB_CLASSNAME				_T("WtlOutBar")	///< Class name of the OutBar
#define OB_CLASSNAMEICONS			_T("WtlOutBarIcons")
													///< Class name of the ListView

#define OB_VERTICALICONSPACE		66				///< The "height" of a "row" of Icons
#define OB_ICONSIZE					32				///< The size of the Icons
#define OB_FRAMEOFFSET				 2				///< Thickness of the frame
#define OB_SPACEBEFOREFRAME			 1				///< Space before the Frame (empty space)
#define OB_SPACEAFTERFRAME			 1				///< Space between the frame of the Icon and the caption
#define OB_SPACEAFTERTEXT			 2				///< Space after text before the next "row" of Icons
#define OB_TEXTHEIGHT				(OB_VERTICALICONSPACE - OB_SPACEBEFOREFRAME - OB_FRAMEOFFSET - OB_ICONSIZE - OB_FRAMEOFFSET - OB_SPACEAFTERFRAME - OB_SPACEAFTERTEXT)
													///< Height of the caption of the Icons.
#define OB_TEXTWIDTH				(OB_ICONSIZE * 2 + OB_FRAMEOFFSET * 4)
													///< Maximum width of the caption of the Icons: double the width of an icon

#define OB_LISTVIEWPERCENT			25				///< % area of the OutBar occupied by the ListView
#define OB_SCROLLMSEC				100				///< Milliseconds between scrolls when pushing the ScrollBar buttons

#define OB_UPSCROLLHOT			((size_t)-2)		///< Up arrow of the ScrollBar is "hot"
#define OB_DOWNSCROLLHOT		((size_t)-3)		///< Down arrow of the ScrollBar is "hot"

/*@}*/

/// \name Flags used by OBFOLDER
/*@{*/
#define OBFF_TEXT			0x0001					///< The OBFOLDER::pszText is valid
#define OBFF_PARAM			0x0002					///< The OBFOLDER::lParam is valid
/*@}*/

/// \name Flags used by OBM_FINDFOLDER
/*@{*/
#define OBFFF_TEXT			0x0001					///< The OBFOLDER::pszText is valid
#define OBFFF_PARAM			0x0002					///< The OBFOLDER::lParam is valid
#define OBFFF_PARTIAL		0x0010					///< The OBFOLDER::pszText is valid. Look for partial matching of OBFOLDER::pszText. Supercedes OBFFF_TEXT. Can be combined with OBFFF_CASEINSENS.
#define OBFFF_CASEINSENS	0x0020					///< The OBFOLDER::pszText is valid. Look for case insensitive matching of OBFOLDER::pszText. Supercedes OBFFF_TEXT. Can be combined with OBFFF_PARTIAL.
#define OBFFF_WRAP			0x0200					///< If at the end of the Folders wrap around to the beginning
/*@}*/

/// A struct describing a Folder. Used by OBM_INSERTFOLDER, OBM_GETFOLDER, OBM_SETFOLDER
struct OBFOLDER
{
	UINT mask;				///< Set of flags that specifies which members of this structure contain data to be set or which members are being requested. Valid flags are: OBFF_TEXT and OBFF_PARAM
	int iFolder;			///< Folder number being requested/set
	LPTSTR pszText;			///< Caption of the Folder (valid only if the mask contains OBFF_TEXT)
	int cchTextMax;			///< Length of the buffer pointed by pszText (used only by OBM_GETFOLDER)
	LPARAM lParam;			///< Used defined parameter (valid only if the mask contains OBFF_PARAM)
};
typedef OBFOLDER* LPOBFOLDER;	///< An alias for a pointer to OBFOLDER

/// \name Flags used by OBITEM
/*@{*/
#define OBIF_TEXT			0x0001					///< The OBITEM::pszText is valid
#define OBIF_PARAM			0x0002					///< The OBITEM::lParam is valid
#define OBIF_IMAGE			0x0003					///< The OBITEM::iImage is valid
/*@}*/

/// \name Flags used by OBM_FINDITEM
/*@{*/
#define OBFIF_TEXT			0x0001					///< The OBITEM::pszText is valid
#define OBFIF_PARAM			0x0002					///< The OBITEM::lParam is valid
#define OBFIF_IMAGE			0x0003					///< The OBITEM::iImage is valid
#define OBFIF_PARTIAL		0x0010					///< The OBITEM::pszText is valid. Look for partial matching of OBITEM::pszText. Supercedes OBFIF_TEXT. Can be combined with OBFIF_CASEINSENS.
#define OBFIF_CASEINSENS	0x0020					///< The OBITEM::pszText is valid. Look for case insensitive matching of OBITEM::pszText. Supercedes OBFIF_TEXT. Can be combined with OBFIF_PARTIAL.
#define OBFIF_FOLDERONLY	0x0100					///< Look only in the OBITEM::iFolder and don't look in other Folders
#define OBFIF_WRAP			0x0200					///< If at the end of the Folders wrap around to the beginning (if combined with OBFIF_FOLDERONLY it will wrap around the OBITEM::iFolder)
/*@}*/

/// A struct describing an Item. Used by OBM_INSERTITEM, OBM_GETITEM, OBM_SETITEM
struct OBITEM
{
	UINT mask;				///< Set of flags that specifies which members of this structure contain data to be set or which members are being requested. Valid flags are: OBFI_TEXT, OBFI_IMAGE and OBFI_PARAM
	int iFolder;			///< Folder number being requested/set
	int iItem;				///< Item number being requested/set
	LPTSTR pszText;			///< Caption of the Item (valid only if the mask contains OBIF_TEXT)
	int cchTextMax;			///< Length of the buffer pointed by pszText (used only by OBM_GETITEM)
	int iImage;				///< Icon associated to the Item (image number of the ImageList)
	LPARAM lParam;			///< Used defined parameter (valid only if the mask contains OBIF_PARAM)
};
typedef OBITEM* LPOBITEM;		///< An alias for a pointer to OBITEM

//

/// A class usable to manipulate a CWtlOutBarImplT.
/** The implementation is in another class (CWtlOutBarImpl). Note that CWtlOutBarImpl derives
 *  from this class, so this class is only useful if you "discover" the HWND of the OutBar
 *  by yourself (for example with ::FindWindow) and want to use the helper functions.
 *  \param TBase The name of the base class (normally ATL::CWindow)
 */
template <class TBase>
class CWtlOutBarT : public TBase
{
public:
	/// Appends a Folder
	/** \param uMask Set of flags that specifies which parameters are valid. Accepts combinations of OBFF_TEXT and OBFF_PARAM
	 *  \param szCaption Caption of the Folder
	 *  \param lParam User defined parameter of the Folder
	 *  \return The number of the Folder added, -1 if AddFolder fails
	 */
	int AddFolder(UINT uMask, LPCTSTR szCaption, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int iFolderCount = GetFolderCount();
		return InsertFolder(iFolderCount, uMask, szCaption, lParam);
	}

	/// Inserts a Folder (or appends it at the end if iFolder == GetFolderCount())
	/** \param iFolder Folder number
	 *  \param uMask Set of flags that specifies which parameters are valid. Accepts combinations of OBFF_TEXT and OBFF_PARAM
	 *  \param szCaption Caption of the Folder
	 *  \param lParam User defined parameter of the Folder
	 *  \return The number of the Folder inserted, -1 if InsertFolder fails
	 */
	int InsertFolder(int iFolder, UINT uMask, LPCTSTR szCaption, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		OBFOLDER obFolder = { 0 };
		obFolder.mask = uMask;
		obFolder.iFolder = iFolder;
		obFolder.pszText = (LPTSTR)szCaption;
		obFolder.lParam = lParam;
		return InsertFolder(&obFolder);
	}

	/// Inserts a Folder (or appends it at the end if iFolder == GetFolderCount())
	/** \param pFolder An OBFOLDER struct describing the Folder
	 *  \return The number of the Folder inserted, -1 if InsertFolder fails
	 */
	int InsertFolder(const OBFOLDER* pFolder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, OBM_INSERTFOLDER, 0, (LPARAM)pFolder);
	}

	/// Copy Folder data to an OBFOLDER struct
	/** You must set the pFolder->iFolder and pFolder->mask before calling this method
	 *  \param pFolder An OBFOLDER struct that will receive the Folder
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL GetFolder(OBFOLDER* pFolder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_GETFOLDER, 0, (LPARAM)pFolder);
	}

	/// Sets Folder data (or creates a new Folder if pFolder->iFolder == GetFolderCount())
	/** You must set the pFolder->iFolder and pFolder->mask before calling this method
	 *  \param pFolder An OBFOLDER struct containing the new values to be used
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL SetFolder(const OBFOLDER* pFolder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_SETFOLDER, 0, (LPARAM)pFolder);
	}

	/// Deletes a Folder
	/** \param iFolder Folder number
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL DeleteFolder(int iFolder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_DELETEFOLDER, iFolder, 0);
	}

	/// Gets the number of Folders present in the OutBar
	/** \return The number of Folders present in the OutBar
	 */
	int GetFolderCount()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, OBM_GETFOLDERCOUNT, 0, 0);
	}

	/// Allocates memory for a certain number of Folders (the Folders are NOT created)
	/** \param iFolderCount Number of Folders to allocate memory for
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL SetFolderCount(int iFolderCount)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_SETFOLDERCOUNT, iFolderCount, 0);
	}

	/// Deletes all the Folders (and their Items)
	/** \return If the method succeeds, the return value is nonzero
	 */
	BOOL DeleteAllFolders()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_DELETEALLFOLDERS, 0, 0);
	}

	/// Appends an Item to a Folder
	/** \param uMask Set of flags that specifies which parameters are valid. Accepts combinations of OBIF_TEXT, OBIF_IMAGE and OBIF_PARAM
	 *  \param iFolder Folder number
	 *  \param szCaption Caption of the Item
	 *  \param iImage Icon number of the Item
	 *  \param lParam User defined parameter of the Folder
	 *  \return The number of the Item added, -1 if AddItem fails
	 */
	int AddItem(int iFolder, UINT uMask, LPCTSTR szCaption, int iImage, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int iItemCount = GetItemCount(iFolder);
		return (iItemCount != -1 ? InsertItem(iFolder, iItemCount, uMask, szCaption, iImage, lParam) : -1);
	}

	/// Inserts an Item in an existing Folder (or appends it at the end of the Folder if iItem == GetItemCount(iFolder))
	/** \param iFolder Folder number
	 *  \param iItem Item number
	 *  \param uMask Set of flags that specifies which parameters are valid. Accepts combinations of OBIF_TEXT, OBIF_IMAGE and OBIF_PARAM
	 *  \param szCaption Caption of the Item
	 *  \param iImage Icon number of the Item
	 *  \param lParam User defined parameter of the Folder
	 *  \return The number of the Item inserted, -1 if InsertItem fails
	 */
	int InsertItem(int iFolder, int iItem, UINT uMask, LPCTSTR szCaption, int iImage, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		OBITEM obItem = { 0 };
		obItem.mask = uMask;
		obItem.iFolder = iFolder;
		obItem.iItem = iItem;
		obItem.pszText = (LPTSTR)szCaption;
		obItem.iImage = iImage;
		obItem.lParam = lParam;
		return InsertItem(&obItem);
	}

	/// Inserts an Item in an existing Folder (or appends it at the end of the Folder if iItem == GetItemCount(iFolder))
	/** \param pItem An OBITEM struct describing the Item
	 *  \return The number of the Item inserted, -1 if InsertItem fails
	 */
	int InsertItem(const OBITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, OBM_INSERTITEM, 0, (LPARAM)pItem);
	}

	/// Copy Item data to an OBITEM struct
	/** You must set the pItem->iFolder, pItem->iItem and pItem->mask before calling this method
	 *  \param pItem An OBItem struct that will receive the Item
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL GetItem(OBITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_GETITEM, 0, (LPARAM)pItem);
	}

	/// Sets Item data (or creates a new Item if pItem->iItem == GetItemCount(pItem->iFolder))
	/** You must set the pItem->iFolder, pItem->iItem and pItem->mask before calling this method
	 *  \param pItem An OBITEM struct containing the new values to be used
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL SetItem(const OBITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_SETITEM, 0, (LPARAM)pItem);
	}

	/// Deletes an Item
	/** \param iFolder Folder number
	/** \param iItem Item number
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL DeleteItem(int iFolder, int iItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_DELETEITEM, iFolder, iItem);
	}

	/// Gets the number of Items present in a Folder
	/** \param iFolder Folder number
	 *  \return The number of Items present in a Folder
	 */
	int GetItemCount(int iFolder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, OBM_GETITEMCOUNT, iFolder, 0);
	}

	/// Allocates memory for a certain number of Items in a Folder (the Items are NOT created)
	/** \param iFolder Folder number
	 *  \param iItemCount Number of Items to allocate memory for
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL SetItemCount(int iFolder, int iItemCount)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_SETITEMCOUNT, iFolder, iItemCount);
	}

	/// Deletes all the Items of a Folder
	/** \param iFolder Folder number
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL DeleteAllItems(int iFolder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_DELETEALLITEMS, iFolder, 0);
	}

	/// Gets the number of the Folder that is open (there is always an open Folder unless there are no Folders, then it returns -1)
	/** \return The number of the Folder that is open
	 */
	int GetOpenFolder()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, OBM_GETOPENFOLDER, 0, 0);
	}

	/// Opens a Folder
	/** \param iFolder Folder number
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL SetOpenFolder(int iFolder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_SETOPENFOLDER, iFolder, 0);
	}

	/// Gets the number of the Folder containing the selected Item
	/** \return The number of the Folder that is selected
	 */
	int GetSelectedFolder()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, OBM_GETSELECTEDFOLDER, 0, 0);
	}

	/// Gets the number of the selected Item
	/** \return The number of the Item that is selected
	 */
	int GetSelectedItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, OBM_GETSELECTEDITEM, 0, 0);
	}

	/// Selects an Item
	/** \param iFolder Folder number
	 *  \param iItem Item number
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL SetSelectedItem(int iFolder, int iItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_SETSELECTEDITEM, iFolder, iItem);
	}

	/// Ensures that an Item is visible (opens it's Folder if necessary)
	/** \param iFolder Folder number
	 *  \param iItem Item number
	 *  \return If the method succeeds, the return value is nonzero
	 */
	BOOL EnsureVisible(int iFolder, int iItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_ENSUREVISIBLE, iFolder, iItem);
	}

	/// Finds a Folder using the data given in pFolder
	/** To use this method you have to put in pFolder->iFolder the number of the Folder
	 *  to start from, set the pFolder->mask using the OBFFF_* flags and fill some of the other
	 *  fields with the data to look for. If a Folder is found pFolder->iFolder will contain its
	 *  number and the return value will be nonzero. If no Folder is found but there isn't any
	 *  error then pFolder->iFolder is -1 and the return value is 0. If there is an error the return
	 *  value is 0 and the pFolder->iFolder value is the same it was at the moment of invocation.
	 *  Note that cchTextMax is ignored.
	 *  \param pFolder An OBFOLDER struct that contains the data to seach, the Folder number to start from and that receives the number of the Folder searched for (or -1 if no Folder is found)
	 *  \return If the method succeeds and a Folder is found, the return value is nonzero.
	 */
	BOOL FindFolder(OBFOLDER* pFolder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_FINDFOLDER, 0, (LPARAM)pFolder);
	}

	/// Finds an Item using the data given in pItem
	/** To use this method you have to put in pItem->iFolder the number of the Folder and in
	 *  pItem->iItem the number of the Item to to start from, set the pItem->mask using the
	 *  OBFIF_* flags and fill some of the other fields with the data to look for. If an Item
	 *  is found pItem->iFolder and pItem->iItem will contain its number and the return value
	 *  will be nonzero. If no Item is found but there isn't any error then pItem->iFolder and
	 *  pItem->iItem are -1 and the return value is 0. If there is an error the return
	 *  value is 0 and the pFolder->iFolder and pFolder iItem values are the same they were at the
	 *  moment of invocation.
	 *  Note that cchTextMax is ignored.
	 *  \param pItem An OBITEM struct that contains the data to seach, the Folder and Item number to start from and that receives the number of the Folder and Item searched for (or -1 if no Item is found)
	 *  \return If the method succeeds and an Item is found, the return value is nonzero.
	 */
	BOOL FindItem(OBITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, OBM_FINDITEM, 0, (LPARAM)pItem);
	}

	/// Gets the HWND of a Folder (passing "-1" as the parameter will return the HWND of the ListView)
	/** iFolder Folder number (-1 for the ListView)
	 *  \return Handle to the control (in the form of a CWindow). CWindow(NULL) if there is an error
	 */
	CWindow GetHWNDHandle(int iFolder)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CWindow((HWND)::SendMessage(m_hWnd, OBM_GETHWNDHANDLE, iFolder, 0));
	}

	/// Gets the color of the background of the ListView
	/** \return Color of the background. CLR_DEFAULT for default color, CLR_NONE for transparent
	 */
	COLORREF GetBkColor()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, OBM_GETBKCOLOR, 0, 0);
	}

	/// Sets the color of the background of the ListView
	/** \param clrNewBk New background color. CLR_DEFAULT for default color, CLR_NONE for transparent
	 *  \return Old color. CLR_INVALID if there is an error
	 */
	COLORREF SetBkColor(COLORREF clrNewBk)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, OBM_GETBKCOLOR, 0, (LRESULT)clrNewBk);
	}

	/// Gets the ImageList
	/** \param iImageListType Type of ImageList. Legal values are OBSIL_NORMAL and OBSIL_SMALL (OBSIL_SMALL is NOT IMPLEMENTED)
	 *  \return Handle to the ImageList (in the form of a CImageList)
	 */
	CImageList GetImageList(int iImageListType) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CImageList((HIMAGELIST)::SendMessage(m_hWnd, OBM_GETIMAGELIST, iImageListType, 0L));
	}

	/// Sets the ImageList
	/** \param hImageList Handle to the new ImageList
	 *  \param iImageListType Type of ImageList. Legal values are OBSIL_NORMAL and OBSIL_SMALL (OBSIL_SMALL is NOT IMPLEMENTED)
	 *  \return Handle to the old ImageList (in the form of a CImageList)
	 */
	CImageList SetImageList(HIMAGELIST hImageList, int iImageListType)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return CImageList((HIMAGELIST)::SendMessage(m_hWnd, OBM_SETIMAGELIST, iImageListType, (LPARAM)hImageList));
	}
};

typedef CWtlOutBarT<ATL::CWindow>	CWtlOutBar; ///< Basic non-templated version of CWtlOutBarT

/// The implementation of the OutBar
/** This is the implementation of the OutBar. You can directly control it through messages (OBM_*) OR
 *  using the CWtlOutBar methods. Note that this is a template class, but you will normally
 *  make it derive from CWtlOutBar (that in turn derives from CWindow)
 *  \param TBase The name of the base class (normally CWtlOutBar)
 */
template <class TBase>
class CWtlOutBarImplT :
	public CWindowImpl<CWtlOutBarImplT<TBase>, TBase, CWinTraits<WS_CHILD|WS_VISIBLE, WS_EX_CONTROLPARENT> >,
	public CThemeImpl<CWtlOutBarImplT<TBase> >
{
protected:
	typedef CWtlOutBarImplT<TBase> thisClass;	///< A shortcut to the complete name of this class

protected:
	/// The internal representation of an Item
	struct CItem
	{
		CAutoVectorPtr<TCHAR> m_szCaption;		///< The caption of the Item. In the end it's a pointer to a TCHAR[]
		int m_iImage;							///< The icon index of the Item
		LPARAM m_lParam;						///< The lParam of the Item
	};

	/// The internal representation of a Folder
	class CFolder
	{
	public:
		CButton m_btn;							///< Handler of the Button of the Folder (the Caption of the Folder is saved by Windows)
		CAutoPtrArray<CItem> m_arpItems;		///< An array of CItems
		size_t m_uOffset;						///< Scroll offset of the Folder

		/// The destructor of CFolder destroy his Button
		~CFolder()
		{
			if (m_btn) {
				ATLVERIFY(m_btn.DestroyWindow());
			}
		}
	};

protected:
	CContainedWindow m_ListIcons;			///< The ListIconControl
	CImageList m_ImageList;					///< The ImageList for the Icons
	CFontHandle m_Font;						///< The Font that will be used to draw the OutBar
	BOOL m_bOwnedFont;						///< TRUE the Font must be deleted by the OutBar
	COLORREF m_clrBK;						///< The color of the background. CLR_DEFAULT for default color, CLR_NONE for transparent
	HTHEME m_hThemeScrollBar;				///< Handle to the secondary Theme used to paint the ScrollBars

	CAutoPtrArray<CFolder> m_arpFolders;	///< The Folders of the OutBar
	size_t m_uFolderOpen;					///< The number of the Folder that is open (-1 if no Folder is open). If there is at least a Folder, then m_uFolderOpen >= 0
	size_t m_uFolderSelect;					///< The number of the Folder that contains the selected Item
	size_t m_uItemSelect;					///< The number of the Item that is selected
	size_t m_uHotItem;						///< The number of the Item that is "hot". Could be OB_UPSCROLLHOT and OB_DOWNSCROLLHOT
	size_t m_uScrollTimes;					///< The number of times a scroll button is pressed in a row (-1 = no scroll button is pressed)

public:
	DECLARE_WND_CLASS_EX(OB_CLASSNAME, 0, -1);

	/// The constructor
	CWtlOutBarImplT() : m_ListIcons(_T("WtlOutBarIcons"), this, 1)
	{
		WNDCLASSEX wcEx = {0};
		wcEx.cbSize = sizeof(WNDCLASSEX);
		if (!::GetClassInfoEx(NULL, OB_CLASSNAMEICONS, &wcEx) && !::GetClassInfoEx(_AtlBaseModule.GetModuleInstance(), OB_CLASSNAMEICONS, &wcEx)) {
			HCURSOR hCur;
			ATLVERIFY((hCur = ::LoadCursor(NULL, IDC_ARROW)) != NULL);
			WNDCLASSEX wcEx = { sizeof(WNDCLASSEX), CS_HREDRAW|CS_VREDRAW, ::DefWindowProc, 0, 0, NULL, NULL, hCur, (HBRUSH)0, NULL, OB_CLASSNAMEICONS, NULL };
			ATLVERIFY(::RegisterClassEx(&wcEx));
		}
	}

	//

	/// The MessageMap
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNCDestroy)

		MESSAGE_HANDLER(WM_GETFONT, OnGetFont)
		MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
		MESSAGE_HANDLER(WM_THEMECHANGED, OnThemeChanged)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)

		if (m_ListIcons) {
			MESSAGE_HANDLER(WM_SIZE, OnSize)
			MESSAGE_HANDLER(WM_STYLECHANGED, OnStyleChanged)
			MESSAGE_HANDLER(WM_ENABLE, OnEnable)
		}

		//

		if (!m_ListIcons && uMsg >= OBM_FIRST && uMsg <= OBM_LAST) {
			return 0;
		}

		MESSAGE_HANDLER(OBM_INSERTFOLDER, OnInsertFolder)
		MESSAGE_HANDLER(OBM_GETFOLDER, OnGetFolder)
		MESSAGE_HANDLER(OBM_SETFOLDER, OnSetFolder)
		MESSAGE_HANDLER(OBM_DELETEFOLDER, OnDeleteFolder)
		MESSAGE_HANDLER(OBM_GETFOLDERCOUNT, OnGetFolderCount)
		MESSAGE_HANDLER(OBM_SETFOLDERCOUNT, OnSetFolderCount)
		MESSAGE_HANDLER(OBM_DELETEALLFOLDERS, OnDeleteAllFolders)

		MESSAGE_HANDLER(OBM_INSERTITEM, OnInsertItem)
		MESSAGE_HANDLER(OBM_GETITEM, OnGetItem)
		MESSAGE_HANDLER(OBM_SETITEM, OnSetItem)
		MESSAGE_HANDLER(OBM_DELETEITEM, OnDeleteItem)
		MESSAGE_HANDLER(OBM_GETITEMCOUNT, OnGetItemCount)
		MESSAGE_HANDLER(OBM_SETITEMCOUNT, OnSetItemCount)
		MESSAGE_HANDLER(OBM_DELETEALLITEMS, OnDeleteAllItems)

		MESSAGE_HANDLER(OBM_GETOPENFOLDER, OnGetOpenFolder)
		MESSAGE_HANDLER(OBM_SETOPENFOLDER, OnSetOpenFolder)

		MESSAGE_HANDLER(OBM_GETSELECTEDFOLDER, OnGetSelectedFolder)
		MESSAGE_HANDLER(OBM_GETSELECTEDITEM, OnGetSelectedItem)
		MESSAGE_HANDLER(OBM_SETSELECTEDITEM, OnSetSelectedItem)

		MESSAGE_HANDLER(OBM_ENSUREVISIBLE, OnEnsureVisible)

		MESSAGE_HANDLER(OBM_FINDFOLDER, OnFindFolder)
		MESSAGE_HANDLER(OBM_FINDITEM, OnFindItem)

		MESSAGE_HANDLER(OBM_GETHWNDHANDLE, OnGetHWNDHandle)

		MESSAGE_HANDLER(OBM_GETBKCOLOR, OnGetBkColor)
		MESSAGE_HANDLER(OBM_SETBKCOLOR, OnSetBkColor)

		MESSAGE_HANDLER(OBM_GETIMAGELIST, OnGetImageList)
		MESSAGE_HANDLER(OBM_SETIMAGELIST, OnSetImageList)

		CHAIN_MSG_MAP(CThemeImpl<thisClass>)

		if (m_arpFolders.GetCount()) {
			COMMAND_RANGE_HANDLER(OB_FIRSTBTN, (OB_FIRSTBTN + m_arpFolders.GetCount() - 1), OnFolderCommand)
		}

	/// The ListIcons MessageMap
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnListIconPaint)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnListIconMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnListIconMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnListIconButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnListIconButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnListIconButtonUp)
		MESSAGE_HANDLER(WM_TIMER, OnListIconTimer)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnListIconCaptureChanged)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnListIconMouseWheel)
		MESSAGE_HANDLER(WM_SETFOCUS, OnListIconSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnListIconKillFocus)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnListIconGetDlgCode)
		MESSAGE_HANDLER(WM_KEYDOWN, OnListIconKeyDown)
	END_MSG_MAP()

protected:
	/// \name System Messages
	/*@{*/
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		_Initialize((const CREATESTRUCT*)lParam);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_uHotItem != (size_t)-1) {
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_CANCEL|TME_LEAVE, m_ListIcons, HOVER_DEFAULT };
			ATLVERIFY(_TrackMouseEvent(&tme));
		}

		if (m_uScrollTimes != (size_t)-1) {
			m_uScrollTimes = (size_t)-1;

			if (m_uHotItem == OB_UPSCROLLHOT || m_uHotItem == OB_DOWNSCROLLHOT) {
				::KillTimer(m_ListIcons.m_hWnd, 0);
			}

			::ReleaseCapture();
		}

		m_ListIcons.DestroyWindow();
		_DeleteAllFolders();

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNCDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_ImageList && !(GetStyle() & OBS_SHAREIMAGELISTS)) {
			ATLVERIFY(m_ImageList.Destroy());
		}

		_CloseThemeData2();

		if (m_bOwnedFont) {
			m_Font.DeleteObject();
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnGetFont(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return (LRESULT)m_Font.m_hFont;
	}

	LRESULT OnSetFont(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HFONT hFontOld = m_Font;

		if (wParam) {
			m_Font = (HFONT)wParam;
			m_bOwnedFont = FALSE;
		} else {
			_SetDefaultFont();
		}

		if (hFontOld != m_Font.m_hFont) {
			for (size_t i = 0; i < m_arpFolders.GetCount(); i++) {
				m_arpFolders[i]->m_btn.SetFont(m_Font, FALSE);
			}

			if (lParam) {
				_ArrangeFolders(TRUE);
			}
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		_ArrangeFolders(TRUE);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnStyleChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == GWL_EXSTYLE) {
			STYLESTRUCT *pss = (STYLESTRUCT*)lParam;

			if (!(pss->styleOld & OBS_FORCESELECT) && pss->styleNew & OBS_FORCESELECT) {
				if (m_uFolderSelect == (size_t)-1) {
					for (size_t i = 0; i < m_arpFolders.GetCount(); i++) {
						if (m_arpFolders[i]->m_arpItems.GetCount()) {
							_SetSelectedItem(i, 0);
						}
					}
				}
			}
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnThemeChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		_CloseThemeData2();

		if (m_lpstrThemeClassList) {
			_OpenThemeData2();
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEnable(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		for (size_t i = 0; i < m_arpFolders.GetCount(); i++) {
			m_arpFolders[i]->m_btn.EnableWindow((BOOL)wParam);
		}
		m_ListIcons.EnableWindow((BOOL)wParam);

		if (m_uFolderSelect != (size_t)-1 && m_uFolderSelect == m_uFolderOpen) {
			_RedrawItem((int)m_uItemSelect);
		}

		_RedrawScrolls(TRUE, TRUE);

		bHandled = FALSE;
		return 0;
	}
	/*@}*/

	/// \name Buttons and ListView Notifications
	/*@{*/
	LRESULT OnFolderCommand(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		wID -= OB_FIRSTBTN;

		if (wID == m_uFolderOpen) {
			if (m_arpFolders.GetCount() == 1) {
				return 0;
			}

			m_uFolderOpen = m_arpFolders.GetCount() - 1;
		} else {
			m_uFolderOpen = wID;
		}

		_ArrangeFolders(TRUE);

		NMHDR nmhdr = { m_hWnd, GetDlgCtrlID(), OBN_FOLDEROPEN };
		::SendMessage(GetParent(), WM_NOTIFY, nmhdr.idFrom, (LPARAM)&nmhdr);

		return 0;
	}

	LRESULT OnListIconPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_ListIcons);

		RECT rect;
		m_ListIcons.GetClientRect(&rect);

		int iXCaption = (rect.right - OB_TEXTWIDTH) / 2;
		if (iXCaption < 0) {
			iXCaption = 0;
		}

		CRgn hRgn;
		ATLVERIFY((hRgn.CreateRectRgn(0,0,0,0)) != 0);
		::GetRandomRgn(dc, hRgn, SYSRGN);

		if ((::GetVersion() & 0x80000000) == 0) { // Windows NT/2000/XP
			POINT pt = { 0, 0 };
			ATLVERIFY(m_ListIcons.ScreenToClient(&pt));
			ATLVERIFY(hRgn.OffsetRgn(pt.x, pt.y) != ERROR);
		}

		COLORREF clrBK = (m_clrBK != CLR_DEFAULT ? m_clrBK : ::GetSysColor(COLOR_BTNFACE));
		CBrush hBrushBK = (m_clrBK != CLR_NONE ? ::CreateSolidBrush(clrBK) : NULL);

		if (m_uFolderOpen != (size_t)-1) { // ScrollBars
			BOOL bUpScrollActive = (m_arpFolders[m_uFolderOpen]->m_uOffset != 0);
			BOOL bDownScrollActive = (m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE > m_arpFolders[m_uFolderOpen]->m_uOffset + (size_t)rect.bottom);

			if (bUpScrollActive || bDownScrollActive) {
				RECT rectScrollUp, rectScrollDown;
				_CalcScrollRect(&rectScrollUp, &rectScrollDown, &rect, iXCaption + OB_TEXTWIDTH);

				BOOL bIsDisabled = GetStyle() & WS_DISABLED;

				CMemoryStripDC memDC(dc, rectScrollUp);

				if (hRgn.RectInRegion(&rectScrollUp)) {
					if (m_hThemeScrollBar) {
						int iStatus;
						if (bIsDisabled || !bUpScrollActive) {
							iStatus = ABS_UPDISABLED;
						} else if (m_uHotItem == OB_UPSCROLLHOT && m_uScrollTimes == (size_t)-1) {
							iStatus = ABS_UPHOT;
						} else if (m_uHotItem == OB_UPSCROLLHOT && m_uScrollTimes != (size_t)-1) {
							iStatus = ABS_UPPRESSED;
						} else {
							iStatus = ABS_UPNORMAL;
						}

						if (::IsThemeBackgroundPartiallyTransparent(m_hThemeScrollBar, SBP_ARROWBTN, iStatus)) {
							if (clrBK != CLR_NONE) {
								ATLVERIFY(memDC.FillRect(&rectScrollUp, hBrushBK));
							}
						}
						ATLVERIFY(SUCCEEDED(::DrawThemeBackground(m_hThemeScrollBar, memDC, SBP_ARROWBTN, iStatus, &rectScrollUp, NULL)));
					} else {
						UINT uiStatus;
						if (bIsDisabled || !bUpScrollActive) {
							uiStatus = DFCS_INACTIVE;
						} else if (m_uHotItem == OB_UPSCROLLHOT && m_uScrollTimes != (size_t)-1) {
							uiStatus = DFCS_PUSHED;
						} else {
							uiStatus = 0;
						}
						memDC.DrawFrameControl(&rectScrollUp, DFC_SCROLL, DFCS_SCROLLUP|uiStatus);
					}

					memDC.Draw();

					dc.ExcludeClipRect(&rectScrollUp);
				}

				memDC.Move(rectScrollDown.left, rectScrollDown.top);

				if (hRgn.RectInRegion(&rectScrollDown)) {
					if (m_hThemeScrollBar) {
						int iStatus;
						if (bIsDisabled || !bDownScrollActive) {
							iStatus = ABS_DOWNDISABLED;
						} else if (m_uHotItem == OB_DOWNSCROLLHOT && m_uScrollTimes == (size_t)-1) {
							iStatus = ABS_DOWNHOT;
						} else if (m_uHotItem == OB_DOWNSCROLLHOT && m_uScrollTimes != (size_t)-1) {
							iStatus = ABS_DOWNPRESSED;
						} else {
							iStatus = ABS_DOWNNORMAL;
						}

						if (::IsThemeBackgroundPartiallyTransparent(m_hThemeScrollBar, SBP_ARROWBTN, iStatus)) {
							if (clrBK != CLR_NONE) {
								ATLVERIFY(memDC.FillRect(&rectScrollDown, hBrushBK));
							}
						}
						ATLVERIFY(SUCCEEDED(::DrawThemeBackground(m_hThemeScrollBar, memDC, SBP_ARROWBTN, iStatus, &rectScrollDown, NULL)));
					} else {
						UINT uiStatus;
						if (bIsDisabled || !bDownScrollActive) {
							uiStatus = DFCS_INACTIVE;
						} else if (m_uHotItem == OB_DOWNSCROLLHOT && m_uScrollTimes!= (size_t)-1) {
							uiStatus = DFCS_PUSHED;
						} else {
							uiStatus = 0;
						}
						memDC.DrawFrameControl(&rectScrollDown, DFC_SCROLL, DFCS_SCROLLDOWN|uiStatus);
					}

					memDC.Draw();

					dc.ExcludeClipRect(&rectScrollDown);
				}

				RECT rect;
				int iRgn = dc.GetClipBox(&rect); // There is a little bug in ExcludeClipRect that always returns COMPLEXREGION
				if (iRgn == NULLREGION) {
					return 0;
				}
			}
		}

		if (clrBK != CLR_NONE) {
			// SX Band
			if (dc.m_ps.rcPaint.left < iXCaption) {
				RECT rectSXBand = { 0, 0, iXCaption, dc.m_ps.rcPaint.bottom };
				ATLVERIFY(dc.FillRect(&rectSXBand, hBrushBK));
			}

			// DX Band
			if (dc.m_ps.rcPaint.right > iXCaption + OB_TEXTWIDTH) {
				RECT rectDXBand = { iXCaption + OB_TEXTWIDTH, 0, dc.m_ps.rcPaint.right, dc.m_ps.rcPaint.bottom };
				ATLVERIFY(dc.FillRect(&rectDXBand, hBrushBK));
			}
		}

		// Icons
		if (dc.m_ps.rcPaint.left < iXCaption + OB_TEXTWIDTH && dc.m_ps.rcPaint.right > iXCaption) {
			if (m_uFolderOpen != (size_t)-1) {
				size_t uStart = (dc.m_ps.rcPaint.top + m_arpFolders[m_uFolderOpen]->m_uOffset) / OB_VERTICALICONSPACE;

				rect.left = iXCaption;
				rect.top = (int)(uStart * OB_VERTICALICONSPACE) - (int)m_arpFolders[m_uFolderOpen]->m_uOffset;
				rect.right = iXCaption + OB_TEXTWIDTH;
				rect.bottom = rect.top + OB_VERTICALICONSPACE;

				if (uStart < m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount()) {
					size_t uEnd = (dc.m_ps.rcPaint.bottom + m_arpFolders[m_uFolderOpen]->m_uOffset) / OB_VERTICALICONSPACE;

					if (uEnd >= m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount()) {
						uEnd = m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() - 1;
					}

					CMemoryStripDC memDC(dc, rect);

					HFONT hOldFont;
					ATLVERIFY((hOldFont = memDC.SelectFont(m_Font)) != NULL);
					ATLVERIFY(memDC.SetBkMode(TRANSPARENT));
					ATLVERIFY(memDC.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT)) != CLR_INVALID);

					BOOL bHasFocus = (::GetFocus() == m_ListIcons);

					for (size_t i = uStart; i <= uEnd; i++) {
						if (hRgn.RectInRegion(&rect)) {
							if (clrBK != CLR_NONE) {
								ATLVERIFY(memDC.FillRect(&rect, hBrushBK));
							}

							_DrawItem(memDC.m_hDC, &rect, i, bHasFocus);

							memDC.Draw();
						}

						rect.top += OB_VERTICALICONSPACE;
						rect.bottom += OB_VERTICALICONSPACE;
						memDC.Move(rect.left, rect.top);
					}

					ATLVERIFY(memDC.SelectFont(hOldFont));
				}
			}

			{ // Bottom Band
				if (rect.top <= dc.m_ps.rcPaint.bottom) {
					rect.bottom = dc.m_ps.rcPaint.bottom;

					if (clrBK != CLR_NONE) {
						ATLVERIFY(dc.FillRect(&rect, hBrushBK));
					}
				}
			}
		}

		return 0;
	}

	LRESULT OnListIconMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		size_t uNewHot;

		if (!::GetFocus()) {
			uNewHot = (size_t)-1;
		} else {
			uNewHot = _GetItemHit(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}

		if (uNewHot != m_uHotItem) {
			size_t uOldHot = m_uHotItem;
			m_uHotItem = uNewHot;

			BOOL bRedrawScrollUp = FALSE, bRedrawScrollDown = FALSE;

			if (uOldHot == OB_UPSCROLLHOT) {
				bRedrawScrollUp = TRUE;
				if (m_uScrollTimes != (size_t)-1) {
					::KillTimer(m_ListIcons.m_hWnd, 0);
				}
			} else if (uOldHot == OB_DOWNSCROLLHOT) {
				bRedrawScrollDown = TRUE;
				if (m_uScrollTimes != (size_t)-1) {
					::KillTimer(m_ListIcons.m_hWnd, 0);
				}
			} else if (uOldHot != (size_t)-1 && !(m_uFolderOpen == m_uFolderSelect && uOldHot == m_uItemSelect)) {
				_RedrawItem(uOldHot);
			}

			if (m_uHotItem == OB_UPSCROLLHOT) {
				if (m_uScrollTimes != (size_t)-1) {
					m_uScrollTimes = 0;
					_ScrollUpBtn(); // Will do the ScrollUp redraw
				} else {
					bRedrawScrollUp = TRUE;
				}
			} else if (m_uHotItem == OB_DOWNSCROLLHOT) {
				if (m_uScrollTimes != (size_t)-1) {
					m_uScrollTimes = 0;
					_ScrollDownBtn(); // Will do the ScrollDown redraw
				} else {
					bRedrawScrollDown = TRUE;
				}
			} else if (m_uHotItem != (size_t)-1 && !(m_uFolderOpen == m_uFolderSelect && m_uHotItem == m_uItemSelect)) {
				_RedrawItem(m_uHotItem);
			}

			if (bRedrawScrollUp || bRedrawScrollDown) {
				_RedrawScrolls(bRedrawScrollUp, bRedrawScrollDown);
			}

			if (uOldHot == (size_t)-1 && m_uHotItem != (size_t)-1) {
				TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_ListIcons, HOVER_DEFAULT };
				ATLVERIFY(_TrackMouseEvent(&tme));
			} else if (uOldHot != (size_t)-1 && m_uHotItem == (size_t)-1) {
				TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_CANCEL|TME_LEAVE, m_ListIcons, HOVER_DEFAULT };
				ATLVERIFY(_TrackMouseEvent(&tme));
			}
		}

		return 0;
	}

	LRESULT OnListIconMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		size_t uOldHot = m_uHotItem;
		m_uHotItem = (size_t)-1;

		if (uOldHot == OB_UPSCROLLHOT) {
			if (m_uScrollTimes != (size_t)-1) {
				::KillTimer(m_ListIcons.m_hWnd, 0);
			}
			_RedrawScrolls(TRUE, FALSE);
		} else if (uOldHot == OB_DOWNSCROLLHOT) {
			if (m_uScrollTimes != (size_t)-1) {
				::KillTimer(m_ListIcons.m_hWnd, 0);
			}
			_RedrawScrolls(FALSE, TRUE);
		} else if (uOldHot != (size_t)-1) {
			_RedrawItem(uOldHot);
		}

		return 0;
	}

	LRESULT OnListIconButtonDown(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int iX = GET_X_LPARAM(lParam);
		int iY = GET_Y_LPARAM(lParam);

		size_t uItem = _GetItemHit(iX, iY);

		if (uItem == (size_t)-1) {
			if (::GetFocus() != m_ListIcons.m_hWnd) {
				m_ListIcons.SetFocus();
			}
		} else if (uItem == OB_UPSCROLLHOT) {
			if (uMsg == WM_LBUTTONDOWN) {
				if (m_uScrollTimes == (size_t)-1) {
					_ScrollUpBtn(TRUE);
				}
			}
		} else if (uItem == OB_DOWNSCROLLHOT) {
			if (uMsg == WM_LBUTTONDOWN) {
				if (m_uScrollTimes == (size_t)-1) {
					_ScrollDownBtn(TRUE);
				}
			}
		} else {
			BOOL bHasFocus = ::GetFocus() == m_ListIcons.m_hWnd;

			if (m_uFolderOpen == m_uFolderSelect && m_uItemSelect == uItem) {
				if (!bHasFocus) {
					m_ListIcons.SetFocus();
				}

				return 0;
			}

			if (m_uFolderOpen == m_uFolderSelect) {
				size_t uOldItemSelect = m_uItemSelect;
				m_uItemSelect = uItem;

				_RedrawItem(uOldItemSelect);
			} else {
				m_uFolderSelect = m_uFolderOpen;
				m_uItemSelect = uItem;
			}

			if (!bHasFocus) {
				m_ListIcons.SetFocus();
			} else {
				_RedrawItem(m_uItemSelect);
			}

			_NotifyItemSelect(TRUE);
		}

		return 0;
	}

	LRESULT OnListIconButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_uScrollTimes != (size_t)-1) {
			m_uScrollTimes = (size_t)-1;

			::KillTimer(m_ListIcons.m_hWnd, 0);

			if (m_uHotItem == OB_UPSCROLLHOT) {
				_RedrawScrolls(TRUE, FALSE);
			} else if (m_uHotItem == OB_DOWNSCROLLHOT) {
				_RedrawScrolls(FALSE, TRUE);
			}

			::ReleaseCapture();
		}

		return 0;
	}

	LRESULT OnListIconTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_uHotItem == OB_UPSCROLLHOT) {
			_ScrollUpBtn();
		} else if (m_uHotItem == OB_DOWNSCROLLHOT) {
			_ScrollDownBtn();
		} else {
			ATLASSERT(FALSE);
		}

		return 0;
	}

	LRESULT OnListIconCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_uScrollTimes != (size_t)-1) {
			if (m_uHotItem == OB_UPSCROLLHOT || m_uHotItem == OB_DOWNSCROLLHOT) {
				::KillTimer(m_ListIcons.m_hWnd, 0);

				m_uScrollTimes = (size_t)-1;

				if (m_uHotItem == OB_UPSCROLLHOT) {
					_RedrawScrolls(TRUE, FALSE);
				} else if (m_uHotItem == OB_DOWNSCROLLHOT) {
					_RedrawScrolls(FALSE, TRUE);
				}
			}
		}

		return 0;
	}

	LRESULT OnListIconMouseWheel(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		int iDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		if (iDelta < 0) {
			_ScrollDown(OB_VERTICALICONSPACE * (size_t)(-iDelta) / 120);
		} else {
			_ScrollUp(OB_VERTICALICONSPACE * (size_t)(iDelta) / 120);
		}

		return 0;
	}

	LRESULT OnListIconSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_uFolderOpen != (size_t)-1 && m_uFolderOpen == m_uFolderSelect) {
			_RedrawItem(m_uItemSelect);
		}

		return 0;
	}

	LRESULT OnListIconKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_uFolderOpen != (size_t)-1 && m_uFolderOpen == m_uFolderSelect) {
			_RedrawItem(m_uItemSelect);
		}

		return 0;
	}

	LRESULT OnListIconGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return DLGC_WANTARROWS;
	}

	LRESULT OnListIconKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_uFolderOpen != (size_t)-1 && m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount()) {
			if (wParam == VK_UP) {
				if (m_uFolderOpen != m_uFolderSelect) {
					_SetSelectedItem(m_uFolderOpen, 0, TRUE);
					_EnsureVisible(m_uFolderOpen, m_uItemSelect);
				} else if (m_uItemSelect) {
					_SetSelectedItem(m_uFolderOpen, m_uItemSelect - 1, TRUE);
					_EnsureVisible(m_uFolderOpen, m_uItemSelect);
				}
			} else if (wParam == VK_DOWN) {
				if (m_uFolderOpen != m_uFolderSelect) {
					_SetSelectedItem(m_uFolderOpen, 0, TRUE);
					_EnsureVisible(m_uFolderOpen, m_uItemSelect);
				} else if (m_uItemSelect + 1 < m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount()) {
					_SetSelectedItem(m_uFolderOpen, m_uItemSelect + 1, TRUE);
					_EnsureVisible(m_uFolderOpen, m_uItemSelect);
				}
			} else if (wParam == VK_HOME) {
				if (::GetKeyState(VK_CONTROL) < 0) {
					_SetSelectedItem(m_uFolderOpen, 0, TRUE);
					_EnsureVisible(m_uFolderOpen, m_uItemSelect);
				}
			} else if (wParam == VK_END) {
				if (::GetKeyState(VK_CONTROL) < 0) {
					_SetSelectedItem(m_uFolderOpen, m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() - 1, TRUE);
					_EnsureVisible(m_uFolderOpen, m_uItemSelect);
				}
			} else if (wParam == VK_PRIOR) {
				if (!(::GetKeyState(VK_CONTROL) < 0)) {
					if (m_uFolderOpen != m_uFolderSelect) {
						_SetSelectedItem(m_uFolderOpen, 0, TRUE);
						_EnsureVisible(m_uFolderOpen, m_uItemSelect);
					} else if (m_uItemSelect) {
						RECT rect;
						m_ListIcons.GetClientRect(&rect);

						size_t uDeltaSelect = rect.bottom / OB_VERTICALICONSPACE;
						size_t uNewSelect;
						if (uDeltaSelect < m_uItemSelect) {
							uNewSelect = m_uItemSelect - uDeltaSelect;
						} else {
							uNewSelect = 0;
						}

						_SetSelectedItem(m_uFolderOpen, uNewSelect, TRUE);
						_EnsureVisible(m_uFolderOpen, m_uItemSelect);
					}
				}
			} else if (wParam == VK_NEXT) {
				if (!(::GetKeyState(VK_CONTROL) < 0)) {
					if (m_uFolderOpen != m_uFolderSelect) {
						_SetSelectedItem(m_uFolderOpen, m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() - 1, TRUE);
						_EnsureVisible(m_uFolderOpen, m_uItemSelect);
					} else if (m_uItemSelect != m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() - 1) {
						RECT rect;
						m_ListIcons.GetClientRect(&rect);

						size_t uDeltaSelect = rect.bottom / OB_VERTICALICONSPACE;
						size_t uNewSelect = uNewSelect = m_uItemSelect + uDeltaSelect;

						if (uNewSelect >= m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount()) {
							uNewSelect = m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() - 1;
						}

						_SetSelectedItem(m_uFolderOpen, uNewSelect, TRUE);
						_EnsureVisible(m_uFolderOpen, m_uItemSelect);
					}
				}
			}
		}

		return 0;
	}
	/*@}*/

	//

	/// \name CWtlOutBarImplT Messages
	/*@{*/
	LRESULT OnInsertFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		const OBFOLDER *pOBFolder = (const OBFOLDER*)lParam;

		return (LRESULT)_InsertFolder(pOBFolder);
	}

	LRESULT OnGetFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		OBFOLDER *pOBFolder = (OBFOLDER*)lParam;

		return _GetFolder(pOBFolder);
	}

	LRESULT OnSetFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		const OBFOLDER *pOBFolder = (const OBFOLDER*)lParam;

		return _SetFolder(pOBFolder);
	}

	LRESULT OnDeleteFolder(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// int iFolder = (int)wParam;

		return _DeleteFolder((size_t)wParam);
	}

	LRESULT OnGetFolderCount(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return (LRESULT)_GetFolderCount();
	}

	LRESULT OnSetFolderCount(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// int iFolderCount = (int)wParam;

		return _SetFolderCount((size_t)wParam);
	}

	LRESULT OnDeleteAllFolders(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return _DeleteAllFolders();
	}

	//

	LRESULT OnInsertItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		const OBITEM *pOBItem = (const OBITEM*)lParam;

		return (LRESULT)_InsertItem(pOBItem);
	}

	LRESULT OnGetItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		OBITEM *pOBItem = (OBITEM*)lParam;

		return _GetItem(pOBItem);
	}

	LRESULT OnSetItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		const OBITEM *pOBItem = (const OBITEM*)lParam;

		return _SetItem(pOBItem);
	}

	LRESULT OnDeleteItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// int iFolder = (int)wParam;
		// int iItem = (int)lParam;

		return _DeleteItem((size_t)wParam, (size_t)lParam);
	}

	LRESULT OnGetItemCount(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// int iFolder = (int)wParam;

		return (LRESULT)_GetItemCount((size_t)wParam);
	}

	LRESULT OnSetItemCount(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// int iFolder = (int)wParam;
		// int iFolderCount = (int)lParam;

		return _SetItemCount((size_t)wParam, (size_t)lParam);
	}

	LRESULT OnDeleteAllItems(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// int iFolder = (int)wParam;

		return _DeleteAllItems((size_t)wParam);
	}

	//

	LRESULT OnGetOpenFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return (LRESULT)_GetOpenFolder();
	}

	LRESULT OnSetOpenFolder(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// int iFolder = (int)wParam;

		return _SetOpenFolder((size_t)wParam);
	}

	//

	LRESULT OnGetSelectedFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return (LRESULT)_GetSelectedFolder();
	}

	LRESULT OnGetSelectedItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return (LRESULT)_GetSelectedItem();
	}

	LRESULT OnSetSelectedItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// int iFolder = (int)wParam;
		// int iItem = (int)lParam;

		return _SetSelectedItem((size_t)wParam, (size_t)lParam);
	}

	//

	LRESULT OnEnsureVisible(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// int iFolder = (int)wParam;
		// int iItem = (int)lParam;

		return _EnsureVisible((size_t)wParam, (size_t)lParam);
	}

	LRESULT OnFindFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		OBFOLDER *pOBFolder = (OBFOLDER*)lParam;

		return _FindFolder(pOBFolder);
	}

	LRESULT OnFindItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		OBITEM *pOBItem = (OBITEM*)lParam;

		return _FindItem(pOBItem);
	}

	LRESULT OnGetHWNDHandle(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// int iFolder = (int)wParam;

		return (LRESULT)(HWND)_GetHWNDHandle((size_t)wParam);
	}

	LRESULT OnGetBkColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return (LRESULT)m_clrBK;
	}

	LRESULT OnSetBkColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		COLORREF clrNewBk = (COLORREF)lParam;

		if (clrNewBk == m_clrBK) {
			return (LRESULT)clrNewBk;
		}

		if (clrNewBk != CLR_NONE && clrNewBk != CLR_DEFAULT && LOBYTE(clrNewBk >> 24)) {
			return (LRESULT)CLR_INVALID;
		}

		COLORREF clrOldBk = m_clrBK;
		m_clrBK = clrNewBk;

		_RedrawAll();

		return (LRESULT)clrOldBk;
	}

	LRESULT OnGetImageList(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		int iType = (int)wParam;

		return (LRESULT)(HIMAGELIST)_GetImageList(iType);
	}

	LRESULT OnSetImageList(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int iType = (int)wParam;
		HIMAGELIST hImageList = (HIMAGELIST)lParam;

		return (LRESULT)(HIMAGELIST)_SetImageList(hImageList, iType);
	}
	/*@}*/

protected:
	/// \name CWtlOutBarImplT Implementation
	/*@{*/
	void _Initialize(const CREATESTRUCT * /*pcs*/)
	{
		m_uFolderOpen = (size_t)-1;
		m_uFolderSelect = (size_t)-1;
		m_uItemSelect = (size_t)-1;
		m_uHotItem = (size_t)-1;
		m_uScrollTimes = (size_t)-1;

		m_clrBK = CLR_DEFAULT;
		m_hThemeScrollBar = NULL;

		_SetDefaultFont();

		RECT rect;
		GetClientRect(&rect);
		m_ListIcons.Create(m_hWnd, rect, _T(""), WS_CHILDWINDOW|WS_TABSTOP|WS_VISIBLE, 0, OB_FIRSTBTN - 1);

		if (m_nIsThemingSupported) {
			m_nIsThemingSupported = 0;

			if (::IsAppThemed()) {
				HMODULE hComCtrl32 = ::GetModuleHandle(_T("comctl32.dll")); // The Common Controls DLL is surely already loaded (we just created a ListView)
				if (hComCtrl32) {
					DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)::GetProcAddress(hComCtrl32, "DllGetVersion");
					if (pDllGetVersion) {
						DLLVERSIONINFO dvi = { 0 };
						dvi.cbSize = sizeof(DLLVERSIONINFO);

						if (SUCCEEDED(pDllGetVersion(&dvi)) && dvi.dwMajorVersion >= 6) {
							m_nIsThemingSupported = 1;
							ATLVERIFY(SetThemeClassList(L"BUTTON"));

							_OpenThemeData2();
						}
					}
				}
			}
		}
	}

	size_t _InsertFolder(const OBFOLDER *pOBFolder)
	{
		if (!pOBFolder) {
			return (size_t)-1;
		}

		if (pOBFolder->mask & ~(OBFF_TEXT|OBIF_PARAM)) {
			return (size_t)-1;
		}

		return _InsertFolder((size_t)pOBFolder->iFolder, pOBFolder->mask & OBFF_TEXT ? pOBFolder->pszText : _T(""), pOBFolder->mask & OBFF_PARAM ? pOBFolder->lParam : 0);
	}

	size_t _InsertFolder(size_t uFolder, LPCTSTR szCaption, LPARAM lParam)
	{
		if (uFolder < 0) {
			return ((size_t)-1);
		}

		if (uFolder > m_arpFolders.GetCount()) {
			uFolder = m_arpFolders.GetCount();
		}

		CAutoPtr<CFolder> pFolder;
		pFolder.Attach(new CFolder);
		ATLVERIFY(pFolder->m_btn.Create(m_hWnd, NULL, szCaption, WS_CHILDWINDOW|WS_TABSTOP|(GetStyle() & WS_DISABLED), WS_EX_NOPARENTNOTIFY, OB_FIRSTBTN + (UINT)uFolder));
		pFolder->m_btn.SetWindowLongPtr(GWLP_USERDATA, lParam);
		pFolder->m_btn.SetFont(m_Font, FALSE);
		pFolder->m_uOffset = 0;

		m_arpFolders.InsertAt(uFolder, pFolder);

		if (uFolder <= m_uFolderOpen || m_uFolderOpen == (size_t)-1) { // This covers even the case m_uFolderOpen == -1
			m_uFolderOpen++;
		}

		if (uFolder <= m_uFolderSelect && m_uFolderSelect != (size_t)-1) {
			m_uFolderSelect++;
		}

		for (size_t i = uFolder + 1; i < m_arpFolders.GetCount(); i++) {
			m_arpFolders[i]->m_btn.SetWindowLongPtr(GWL_ID, OB_FIRSTBTN + i);
		}

		_ArrangeFolders();

		return uFolder;
	}

	BOOL _GetFolder(OBFOLDER *pOBFolder) const
	{
		if (!pOBFolder) {
			return FALSE;
		}

		if (pOBFolder->iFolder < 0 || pOBFolder->iFolder >= (int)m_arpFolders.GetCount()) {
			return FALSE;
		}

		if (pOBFolder->mask & ~(OBFF_TEXT|OBIF_PARAM)) {
			return FALSE;
		}

		if (pOBFolder->mask & OBFF_TEXT && pOBFolder->pszText && !pOBFolder->cchTextMax) {
			return FALSE;
		}

		if (pOBFolder->mask & OBFF_TEXT) {
			if (pOBFolder->pszText) {
				m_arpFolders[(size_t)pOBFolder->iFolder]->m_btn.GetWindowText(pOBFolder->pszText, pOBFolder->cchTextMax);
			} else {
				pOBFolder->cchTextMax = m_arpFolders[(size_t)pOBFolder->iFolder]->m_btn.GetWindowTextLength() + 1;
			}
		}

		if (pOBFolder->mask & OBFF_PARAM) {
			pOBFolder->lParam = m_arpFolders[(size_t)pOBFolder->iFolder]->m_btn.GetWindowLongPtr(GWLP_USERDATA);
		}

		return TRUE;
	}

	BOOL _SetFolder(const OBFOLDER *pOBFolder)
	{
		if (!pOBFolder) {
			return FALSE;
		}

		if (pOBFolder->iFolder == (int)m_arpFolders.GetCount()) {
			return (_InsertFolder((size_t)pOBFolder->iFolder, pOBFolder->mask & OBFF_TEXT ? pOBFolder->pszText : _T(""), pOBFolder->mask & OBFF_PARAM ? pOBFolder->lParam : 0) != (size_t)-1 ? TRUE : FALSE);
		}

		if (pOBFolder->iFolder < 0 || pOBFolder->iFolder > (int)m_arpFolders.GetCount()) {
			return FALSE;
		}

		if (pOBFolder->mask & ~(OBFF_TEXT|OBIF_PARAM)) {
			return FALSE;
		}

		if (pOBFolder->mask & OBFF_TEXT && !pOBFolder->pszText) {
			return FALSE;
		}

		if (pOBFolder->mask & OBFF_TEXT) {
			ATLVERIFY(m_arpFolders[(size_t)pOBFolder->iFolder]->m_btn.SetWindowText(pOBFolder->pszText));
		}

		if (pOBFolder->mask & OBFF_PARAM) {
			m_arpFolders[(size_t)pOBFolder->iFolder]->m_btn.SetWindowLongPtr(GWLP_USERDATA, pOBFolder->lParam);
		}

		return TRUE;
	}

	BOOL _DeleteFolder(size_t uFolder)
	{
		if (uFolder < 0 || uFolder >= m_arpFolders.GetCount()) {
			return FALSE;
		}

		m_arpFolders.RemoveAt(uFolder);

		BOOL bResynchLV = (uFolder == m_uFolderOpen ? TRUE : FALSE);
		BOOL bNotify = FALSE;

		if (uFolder < m_uFolderOpen) { // If there is a Folder, then it's impossible m_uFolderOpen == -1
			m_uFolderOpen--;
		} else if (m_uFolderOpen == m_arpFolders.GetCount()) { // This happens when uFolder == m_uFolderOpen == (m_arpFolders.GetCount() - 1)
			m_uFolderOpen--;
		}

		if (uFolder == m_uFolderSelect) {
			if (GetStyle() & OBS_FORCESELECT) {
				size_t i;

				for (i = uFolder; i < m_arpFolders.GetCount(); i++) {
					if (m_arpFolders[i]->m_arpItems.GetCount()) {
						m_uFolderSelect = (size_t)i;
						m_uItemSelect = 0;
						break;
					}
				}

				if (i == m_arpFolders.GetCount()) {
					i = uFolder;

					while (TRUE) {
						if (!i) {
							m_uFolderSelect = (size_t)-1;
							m_uItemSelect = (size_t)-1;
							break;
						}

						i--;

						if (m_arpFolders[i]->m_arpItems.GetCount()) {
							m_uFolderSelect = i;
							m_uItemSelect = 0;
							break;
						}
					}
				}
			} else {
				m_uFolderSelect = (size_t)-1;
				m_uItemSelect = (size_t)-1;
			}

			bNotify = TRUE;
		} else if (uFolder < m_uFolderSelect && m_uFolderSelect != (size_t)-1) {
			m_uFolderSelect--;
		}

		for (size_t i = uFolder; i < m_arpFolders.GetCount(); i++) {
			m_arpFolders[i]->m_btn.SetWindowLongPtr(GWL_ID, OB_FIRSTBTN + i);
		}

		_ArrangeFolders(bResynchLV);

		if (bNotify) {
			if (m_uFolderOpen == m_uFolderSelect) {
				_RedrawItem(m_uItemSelect);
			}

			_NotifyItemSelect(FALSE);
		}

		return TRUE;
	}

	size_t _GetFolderCount() const
	{
		return m_arpFolders.GetCount();
	}

	BOOL _SetFolderCount(size_t uNewSize)
	{
		if (uNewSize < m_arpFolders.GetCount()) {
			return FALSE;
		} else if (uNewSize == m_arpFolders.GetCount()) {
			return TRUE;
		} else {
			return (BOOL)m_arpFolders.SetCount(m_arpFolders.GetCount(), (int)(uNewSize - m_arpFolders.GetCount()));
		}
	}

	BOOL _DeleteAllFolders()
	{
		if (m_uFolderOpen != (size_t)-1) {
			m_arpFolders.RemoveAll();
			m_uFolderOpen = (size_t)-1;
			m_uFolderSelect = (size_t)-1;
			m_uItemSelect = (size_t)-1;

			if (m_ListIcons) {
				_ArrangeFolders(TRUE);
				_NotifyItemSelect(FALSE);
			}
		}

		return TRUE;
	}

	size_t _InsertItem(const OBITEM *pOBItem)
	{
		if (!pOBItem) {
			return (size_t)-1;
		}

		if (pOBItem->mask & ~(OBIF_TEXT|OBIF_PARAM|OBIF_IMAGE)) {
			return (size_t)-1;
		}

		return _InsertItem((size_t)pOBItem->iFolder, (size_t)pOBItem->iItem, pOBItem->mask & OBIF_TEXT ? pOBItem->pszText : _T(""), pOBItem->mask & OBIF_IMAGE ? pOBItem->iImage : 0, pOBItem->mask & OBIF_PARAM ? pOBItem->lParam : 0);
	}

	size_t _InsertItem(size_t uFolder, size_t uItem, LPCTSTR szCaption, int iImage, LPARAM lParam)
	{
		if (uFolder < 0 || uFolder >= m_arpFolders.GetCount()) {
			return ((size_t)-1);
		}

		if (uItem < 0) {
			return ((size_t)-1);
		}

		if (uItem > m_arpFolders[uFolder]->m_arpItems.GetCount()) {
			uItem = m_arpFolders[uFolder]->m_arpItems.GetCount();
		}

		CAutoPtr<CItem> pItem;
		pItem.Attach(new CItem);

		pItem->m_lParam = lParam;

		size_t uLen = ::_tcslen(szCaption) + 1;
		pItem->m_szCaption.Allocate(uLen);
		::memcpy(pItem->m_szCaption, szCaption, uLen * sizeof(TCHAR));

		pItem->m_iImage = iImage;

		m_arpFolders[uFolder]->m_arpItems.InsertAt(uItem, pItem);

		BOOL bNotify = FALSE;
		BOOL bRedrawAll = _RecalcScrolls(TRUE, uFolder, uItem);

		if (uFolder == m_uFolderOpen) {
			if (m_uItemSelect == (size_t)-1 && GetStyle() & OBS_FORCESELECT) {
				m_uFolderSelect = uFolder;
				m_uItemSelect = uItem;

				bNotify = TRUE;
			} else if (uItem <= m_uItemSelect && m_uItemSelect != (size_t)-1) {
				m_uItemSelect++;
			}

			if (bRedrawAll) {
				_RedrawItems((size_t)-1, (size_t)-1);
			} else {
				_RedrawItems(uItem, (size_t)-1);
			}
		} else if (m_uItemSelect == (size_t)-1 && GetStyle() & OBS_FORCESELECT) {
			m_uFolderSelect = uFolder;
			m_uItemSelect = uItem;

			bNotify = TRUE;
		}

		if (bNotify) {
			_NotifyItemSelect(FALSE);
		}

		return uItem;
	}

	BOOL _GetItem(OBITEM *pOBItem) const
	{
		if (!pOBItem) {
			return FALSE;
		}

		if (pOBItem->iFolder < 0 || pOBItem->iFolder >= (int)m_arpFolders.GetCount()) {
			return FALSE;
		}

		if (pOBItem->iItem < 0 || pOBItem->iItem >= (int)m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems.GetCount()) {
			return FALSE;
		}

		if (pOBItem->mask & ~(OBIF_TEXT|OBIF_PARAM|OBIF_IMAGE)) {
			return FALSE;
		}

		if (pOBItem->mask & OBIF_TEXT && pOBItem->pszText && !pOBItem->cchTextMax) {
			return FALSE;
		}

		if (pOBItem->mask & OBIF_TEXT) {
			if (pOBItem->pszText) {
				::_tcsncpy(pOBItem->pszText, m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems[pOBItem->iItem]->m_szCaption, pOBItem->cchTextMax);
				pOBItem->pszText[pOBItem->cchTextMax - 1] = 0;
			} else {
				pOBItem->cchTextMax = (int)::_tcslen(m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems[pOBItem->iItem]->m_szCaption) + 1;
			}
		}

		if (pOBItem->mask & OBIF_PARAM) {
			pOBItem->lParam = m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems[pOBItem->iItem]->m_lParam;
		}

		if (pOBItem->mask & OBIF_IMAGE) {
			pOBItem->iImage = m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems[pOBItem->iItem]->m_iImage;
		}

		return TRUE;
	}

	BOOL _SetItem(const OBITEM *pOBItem)
	{
		if (!pOBItem) {
			return FALSE;
		}

		if (pOBItem->iFolder < 0 || pOBItem->iFolder >= (int)m_arpFolders.GetCount()) {
			return FALSE;
		}

		if (pOBItem->iItem < 0 || pOBItem->iItem > (int)m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems.GetCount()) {
			return FALSE;
		}

		if (pOBItem->iFolder == (int)m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems.GetCount()) {
			return (_InsertItem((size_t)pOBItem->iFolder, (size_t)pOBItem->iItem, pOBItem->mask & OBIF_TEXT ? pOBItem->pszText : _T(""), pOBItem->mask & OBIF_IMAGE ? pOBItem->iImage : 0, pOBItem->mask & OBIF_PARAM ? pOBItem->lParam : 0) != (size_t)-1 ? TRUE : FALSE);
		}

		if (pOBItem->mask & ~(OBIF_TEXT|OBIF_PARAM|OBIF_IMAGE)) {
			return FALSE;
		}

		if (pOBItem->mask & OBIF_TEXT && !pOBItem->pszText) {
			return FALSE;
		}

		if (pOBItem->mask & OBIF_TEXT) {
			size_t uLen = ::_tcslen(pOBItem->pszText) + 1;
			m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems[(size_t)pOBItem->iItem]->m_szCaption.Free();
			m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems[(size_t)pOBItem->iItem]->m_szCaption.Allocate(uLen);
			::memcpy(m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems[(size_t)pOBItem->iItem]->m_szCaption, pOBItem->pszText, uLen * sizeof(TCHAR));
		}

		if (pOBItem->mask & OBIF_IMAGE) {
			m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems[(size_t)pOBItem->iItem]->m_iImage = pOBItem->iImage;
		}

		if (pOBItem->mask & OBIF_PARAM) {
			m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems[(size_t)pOBItem->iItem]->m_lParam = pOBItem->lParam;
		}

		if (m_uFolderOpen == (size_t)pOBItem->iFolder && pOBItem->mask & (OBIF_TEXT|OBIF_IMAGE|OBIF_PARAM)) {
			_RedrawItem((size_t)pOBItem->iItem);
		}

		return TRUE;
	}

	BOOL _DeleteItem(size_t uFolder, size_t uItem)
	{
		if (uFolder < 0 || uFolder >= m_arpFolders.GetCount()) {
			return FALSE;
		}

		if (uItem >= m_arpFolders[uFolder]->m_arpItems.GetCount() || uItem < 0) {
			return FALSE;
		}

		m_arpFolders[uFolder]->m_arpItems.RemoveAt(uItem);

		BOOL bNotify = FALSE;

		if (uFolder == m_uFolderSelect) {
			if (uItem == m_uItemSelect) {
				if (GetStyle() & OBS_FORCESELECT) {
					if (m_arpFolders[uFolder]->m_arpItems.GetCount()) {
						if (m_uItemSelect == m_arpFolders[uFolder]->m_arpItems.GetCount()) {
							m_uItemSelect--;
						}
					} else {
						size_t i;

						for (i = uFolder + 1; i < m_arpFolders.GetCount(); i++) {
							if (m_arpFolders[i]->m_arpItems.GetCount()) {
								m_uFolderSelect = i;
								m_uItemSelect = 0;
								break;
							}
						}

						if (i == m_arpFolders.GetCount()) {
							i = uFolder;

							while (TRUE) {
								if (!i) {
									m_uFolderSelect = (size_t)-1;
									m_uItemSelect = (size_t)-1;
									break;
								}

								i--;

								if (m_arpFolders[i]->m_arpItems.GetCount()) {
									m_uFolderSelect = i;
									m_uItemSelect = 0;
									break;
								}
							}
						}
					}
				} else {
					m_uFolderSelect = (size_t)-1;
					m_uItemSelect = (size_t)-1;
				}

				bNotify = TRUE;
			} else if (uItem < m_uItemSelect) {
				m_uItemSelect--;
			}
		}

		BOOL bRedrawAll = _RecalcScrolls(FALSE, uFolder, uItem);

		if (uFolder == m_uFolderOpen) {
			if (bRedrawAll) {
				_RedrawItems((size_t)-1, (size_t)-1);
			} else {
				_RedrawItems(uItem, (size_t)-1);
			}
		}

		if (bNotify) {
			if (m_uFolderOpen == m_uFolderSelect && !(uFolder == m_uFolderOpen && uItem <= m_uItemSelect)) {
				_RedrawItem(m_uItemSelect);
			}

			_NotifyItemSelect(FALSE);
		}

		return TRUE;
	}

	size_t _GetItemCount(size_t uFolder) const
	{
		if (uFolder >= m_arpFolders.GetCount()) {
			return (size_t)-1;
		}

		return m_arpFolders[uFolder]->m_arpItems.GetCount();
	}

	BOOL _SetItemCount(size_t uFolder, size_t uNewSize)
	{
		if (uFolder < 0 || uFolder >= m_arpFolders.GetCount()) {
			return FALSE;
		} else if (uNewSize < m_arpFolders[uFolder]->m_arpItems.GetCount()) {
			return FALSE;
		} else if (uNewSize == m_arpFolders[uFolder]->m_arpItems.GetCount()) {
			return TRUE;
		} else {
			return (BOOL)m_arpFolders[uFolder]->m_arpItems.SetCount(m_arpFolders[uFolder]->m_arpItems.GetCount(), (int)(uNewSize - m_arpFolders[uFolder]->m_arpItems.GetCount()));
		}
	}

	BOOL _DeleteAllItems(size_t uFolder)
	{
		if (uFolder < 0 || uFolder >= m_arpFolders.GetCount()) {
			return FALSE;
		}

		m_arpFolders[uFolder]->m_arpItems.RemoveAll();

		BOOL bNotify = FALSE;

		if (uFolder == m_uFolderSelect) {
			if (GetStyle() & OBS_FORCESELECT) {
				size_t i;

				for (i = uFolder + 1; i < m_arpFolders.GetCount(); i++) {
					if (m_arpFolders[i]->m_arpItems.GetCount()) {
						m_uFolderSelect = i;
						m_uItemSelect = 0;
						break;
					}
				}

				if (i == m_arpFolders.GetCount()) {
					i = uFolder;

					while (TRUE) {
						if (!i) {
							m_uFolderSelect = (size_t)-1;
							m_uItemSelect = (size_t)-1;
							break;
						}

						i--;

						if (m_arpFolders[i]->m_arpItems.GetCount()) {
							m_uFolderSelect = i;
							m_uItemSelect = 0;
							break;
						}
					}
				}
			} else {
				m_uFolderSelect = (size_t)-1;
				m_uItemSelect = (size_t)-1;
			}

			bNotify = TRUE;
		}

		if (uFolder == m_uFolderOpen) {
			_RedrawAll();
		}

		if (bNotify) {
			if (m_uFolderOpen == m_uFolderSelect && uFolder != m_uFolderOpen) {
				_RedrawItem(m_uItemSelect);
			}

			_NotifyItemSelect(FALSE);
		}

		return TRUE;
	}

	size_t _GetOpenFolder() const
	{
		return m_uFolderOpen;
	}

	BOOL _SetOpenFolder(size_t uFolder)
	{
		if (uFolder < 0 || uFolder >= m_arpFolders.GetCount()) {
			return FALSE;
		}

		if (uFolder == m_uFolderOpen) {
			return TRUE;
		}

		m_uFolderOpen = uFolder;
		_ArrangeFolders(TRUE);

		return TRUE;
	}

	size_t _GetSelectedFolder() const
	{
		return m_uFolderSelect;
	}

	size_t _GetSelectedItem() const
	{
		return m_uItemSelect;
	}

	BOOL _SetSelectedItem(size_t uFolder, size_t uItem, BOOL bUser = FALSE)
	{
		if (uFolder == (size_t)-1 && uItem == (size_t)-1 && m_uFolderSelect != (size_t)-1 && m_uItemSelect != (size_t)-1 && GetStyle() & OBS_FORCESELECT) {
			return FALSE;
		}

		if (uFolder == (size_t)-1 && uItem != (size_t)-1) {
			return FALSE;
		}

		if (uFolder != (size_t)-1 && uItem == (size_t)-1) {
			return FALSE;
		}

		if (uFolder != (size_t)-1 && uFolder >= m_arpFolders.GetCount()) {
			return FALSE;
		}

		if (uFolder != (size_t)-1 && uFolder < 0) {
			return FALSE;
		}

		if (uItem != (size_t)-1 && uItem >= m_arpFolders[uFolder]->m_arpItems.GetCount()) {
			return FALSE;
		}

		if (uItem != (size_t)-1 && uItem < 0) {
			return FALSE;
		}

		if (uFolder != m_uFolderSelect || uItem != m_uItemSelect) {
			if (m_uFolderSelect == m_uFolderOpen) {
				size_t uOldItemSelect = m_uItemSelect;
				m_uItemSelect = (size_t)-1;

				_RedrawItem(uOldItemSelect);
			}

			m_uFolderSelect = uFolder;
			m_uItemSelect = uItem;

			if (uFolder == m_uFolderOpen) {
				_RedrawItem(m_uItemSelect);
			}

			_NotifyItemSelect(bUser);
		}

		return TRUE;
	}

	BOOL _EnsureVisible(size_t uFolder, size_t uItem)
	{
		if (uFolder == (size_t)-1 && uItem == (size_t)-1) {
			if (m_uFolderSelect == (size_t)-1) {
				return FALSE;
			}

			uFolder = m_uFolderSelect;
			uItem = m_uItemSelect;
		} else if (uFolder != (size_t)-1 && uFolder < m_arpFolders.GetCount() && uItem != (size_t)-1 && uItem < m_arpFolders[uFolder]->m_arpItems.GetCount()) {
			// Nothing
		} else {
			return FALSE;
		}

		RECT rect;
		m_ListIcons.GetClientRect(&rect);

		size_t uFirstItem = (m_arpFolders[uFolder]->m_uOffset + OB_VERTICALICONSPACE - 1) / OB_VERTICALICONSPACE;
		size_t uLastItem = (m_arpFolders[uFolder]->m_uOffset + rect.bottom) / OB_VERTICALICONSPACE;
		if (uLastItem) {
			uLastItem--;
		}

		if (uFolder == m_uFolderOpen && uItem >= uFirstItem && uItem <= uLastItem) {
			return TRUE;
		}

		if (uItem < uFirstItem || uItem > uLastItem) {
			if (!uItem) {
				m_arpFolders[uFolder]->m_uOffset = 0;
			} else {
				m_arpFolders[uFolder]->m_uOffset = (uItem - 1) * OB_VERTICALICONSPACE;

				if (m_arpFolders[uFolder]->m_arpItems.GetCount() * OB_VERTICALICONSPACE < m_arpFolders[uFolder]->m_uOffset + (size_t)rect.bottom) {
					m_arpFolders[uFolder]->m_uOffset = m_arpFolders[uFolder]->m_arpItems.GetCount() * OB_VERTICALICONSPACE - (size_t)rect.bottom;
				}
			}
		}

		if (m_uFolderOpen != uFolder) {
			m_uFolderOpen = uFolder;
			_ArrangeFolders(TRUE);
		} else {
			_ResynchListView();
		}

		return TRUE;
	}

	BOOL _FindFolder(OBFOLDER *pOBFolder) const
	{
		if (!pOBFolder) {
			return FALSE;
		}

		if (!pOBFolder->mask) {
			return FALSE;
		}

		if (pOBFolder->mask & ~(OBFFF_TEXT|OBFFF_PARAM|OBFFF_PARTIAL|OBFFF_CASEINSENS|OBFFF_WRAP)) {
			return FALSE;
		}

		if (pOBFolder->mask & (OBFFF_TEXT|OBFFF_PARTIAL|OBFFF_CASEINSENS) && !pOBFolder->pszText) {
			return FALSE;
		}

		for (size_t i = (size_t)pOBFolder->iFolder; i < m_arpFolders.GetCount(); i++) {
			if (_CompareFolders(i, pOBFolder)) {
				pOBFolder->iFolder = (int)i;
				return TRUE;
			}
		}

		if (pOBFolder->mask & OBFFF_WRAP && pOBFolder->iFolder != 0) {
			size_t uEnd = ((size_t)pOBFolder->iFolder < m_arpFolders.GetCount() ? (size_t)pOBFolder->iFolder : m_arpFolders.GetCount());
			for (size_t i = 0; i < uEnd; i++) {
				if (_CompareFolders(i, pOBFolder)) {
					pOBFolder->iFolder = (int)i;
					return TRUE;
				}
			}
		}

		pOBFolder->iFolder = -1;
		return FALSE;
	}

	BOOL _CompareFolders(size_t uFolder, const OBFOLDER *pOBFolder) const
	{
		if (pOBFolder->mask & OBFFF_PARAM) {
			if (pOBFolder->lParam != m_arpFolders[uFolder]->m_btn.GetWindowLongPtr(GWLP_USERDATA)) {
				return FALSE;
			}
		}

		if (pOBFolder->mask & (OBFFF_TEXT|OBFFF_PARTIAL|OBFFF_CASEINSENS)) {
			CAutoVectorPtr<TCHAR> szCaption;
			size_t uLen = (size_t)m_arpFolders[uFolder]->m_btn.GetWindowTextLength() + 1;
			szCaption.Allocate(uLen);
			m_arpFolders[uFolder]->m_btn.GetWindowText(szCaption.m_p, (int)uLen);

			if ((pOBFolder->mask & (OBFFF_PARTIAL|OBFFF_CASEINSENS)) == (OBFFF_PARTIAL|OBFFF_CASEINSENS)) {
				if (::_tcsicmpsub(pOBFolder->pszText, szCaption) > 0) {
					return FALSE;
				}
			} else if (pOBFolder->mask & OBFFF_PARTIAL) {
				if (::_tcscmpsub(pOBFolder->pszText, szCaption) > 0) {
					return FALSE;
				}
			} else if (pOBFolder->mask & OBFFF_CASEINSENS) {
				if (::_tcsicmp(pOBFolder->pszText, szCaption) != 0) {
					return FALSE;
				}
			} else {
				if (::_tcscmp(pOBFolder->pszText, szCaption) != 0) {
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	BOOL _FindItem(OBITEM *pOBItem) const
	{
		if (!pOBItem) {
			return FALSE;
		}

		if (!pOBItem->mask) {
			return FALSE;
		}

		if (pOBItem->mask & ~(OBFIF_TEXT|OBFIF_PARAM|OBFIF_IMAGE|OBFIF_PARTIAL|OBFIF_CASEINSENS|OBFIF_FOLDERONLY|OBFIF_WRAP)) {
			return FALSE;
		}

		if (pOBItem->mask & (OBFIF_TEXT|OBFIF_PARTIAL|OBFIF_CASEINSENS) && !pOBItem->pszText) {
			return FALSE;
		}

		if ((size_t)pOBItem->iFolder < m_arpFolders.GetCount()) {
			for (size_t j = pOBItem->iItem; j < m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems.GetCount(); j++) {
				if (_CompareItems((size_t)pOBItem->iFolder, j, pOBItem)) {
					pOBItem->iFolder = pOBItem->iFolder;
					pOBItem->iItem = (int)j;
					return TRUE;
				}
			}

			if ((pOBItem->mask & (OBFIF_FOLDERONLY|OBFIF_WRAP)) == (OBFIF_FOLDERONLY|OBFIF_WRAP) && pOBItem->iItem != 0) {
				size_t uEnd = ((size_t)pOBItem->iItem < m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems.GetCount() ? (size_t)pOBItem->iItem : m_arpFolders[(size_t)pOBItem->iFolder]->m_arpItems.GetCount());
				for (size_t j = 0; j < uEnd; j++) {
					if (_CompareItems((size_t)pOBItem->iFolder, j, pOBItem)) {
						pOBItem->iFolder = pOBItem->iFolder;
						pOBItem->iItem = (int)j;
						return TRUE;
					}
				}
			}
		}

		if (!(pOBItem->mask & OBFIF_FOLDERONLY)) {
			if (pOBItem->iFolder != -1) {
				for (size_t i = (size_t)pOBItem->iFolder + 1; i < m_arpFolders.GetCount(); i++) {
					for (size_t j = 0; j < m_arpFolders[i]->m_arpItems.GetCount(); j++) {
						if (_CompareItems(i, j, pOBItem)) {
							pOBItem->iFolder = (int)i;
							pOBItem->iItem = (int)j;
							return TRUE;
						}
					}
				}
			}

			if (pOBItem->mask & OBFFF_WRAP && pOBItem->iFolder != 0) {
				size_t uEnd = ((size_t)pOBItem->iFolder < m_arpFolders.GetCount() ? (size_t)pOBItem->iFolder : m_arpFolders.GetCount());
				for (size_t i = 0; i < uEnd; i++) {
					for (size_t j = 0; j < m_arpFolders[i]->m_arpItems.GetCount(); j++) {
						if (_CompareItems(i, j, pOBItem)) {
							pOBItem->iFolder = (int)i;
							pOBItem->iItem = (int)j;
							return TRUE;
						}
					}
				}
			}
		}

		pOBItem->iFolder = -1;
		pOBItem->iItem = -1;
		return FALSE;
	}

	BOOL _CompareItems(size_t uFolder, size_t uItem, const OBITEM *pOBItem) const
	{
		if (pOBItem->mask & OBFIF_PARAM) {
			if (pOBItem->lParam != m_arpFolders[uFolder]->m_arpItems[uItem]->m_lParam) {
				return FALSE;
			}
		}

		if (pOBItem->mask & OBFIF_IMAGE) {
			if (pOBItem->iImage != m_arpFolders[uFolder]->m_arpItems[uItem]->m_iImage) {
				return FALSE;
			}
		}

		if ((pOBItem->mask & (OBFIF_PARTIAL|OBFIF_CASEINSENS)) == (OBFIF_PARTIAL|OBFIF_CASEINSENS)) {
			if (::_tcsicmpsub(pOBItem->pszText, m_arpFolders[uFolder]->m_arpItems[uItem]->m_szCaption) > 0) {
				return FALSE;
			}
		} else if (pOBItem->mask & OBFIF_PARTIAL) {
			if (::_tcscmpsub(pOBItem->pszText, m_arpFolders[uFolder]->m_arpItems[uItem]->m_szCaption) > 0) {
				return FALSE;
			}
		} else if (pOBItem->mask & OBFIF_CASEINSENS) {
			if (::_tcsicmp(pOBItem->pszText, m_arpFolders[uFolder]->m_arpItems[uItem]->m_szCaption) != 0) {
				return FALSE;
			}
		} else if (pOBItem->mask & OBFIF_TEXT) {
			if (::_tcscmp(pOBItem->pszText, m_arpFolders[uFolder]->m_arpItems[uItem]->m_szCaption) != 0) {
				return FALSE;
			}
		}

		return TRUE;
	}

	HWND _GetHWNDHandle(size_t uFolder)
	{
		if (uFolder == (size_t)-1) {
			return m_ListIcons.m_hWnd;
		} else if (uFolder >= 0 && uFolder < m_arpFolders.GetCount()) {
			return m_arpFolders[uFolder]->m_btn.m_hWnd;
		} else {
			return 0;
		}
	}

	CImageList _GetImageList(int iImageListType) const
	{
		if (iImageListType != OBSIL_NORMAL) {
			return NULL;
		}

		return m_ImageList;
	}

	CImageList _SetImageList(HIMAGELIST hImageList, int iImageListType)
	{
		if (iImageListType != OBSIL_NORMAL) {
			return NULL;
		}

		HIMAGELIST hOldIL = m_ImageList.Detach();
		m_ImageList.Attach(hImageList);
		return hOldIL;
	}
	/*@}*/

protected:
	/// \name Internal methods used by CWtlOutBarImplT
	/*@{*/
	void _ResynchListView()
	{
		RECT rect;
		m_ListIcons.GetClientRect(&rect);

		if (m_uFolderOpen != (size_t)-1) {
			if (m_arpFolders[m_uFolderOpen]->m_uOffset && m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE < m_arpFolders[m_uFolderOpen]->m_uOffset + (size_t)rect.bottom) { // Overscroll
				if (m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE > (size_t)rect.bottom) {
					m_arpFolders[m_uFolderOpen]->m_uOffset = m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE - rect.bottom;
				} else {
					m_arpFolders[m_uFolderOpen]->m_uOffset = 0;
				}
			}
		}

		_RedrawAll();
	}

	void _ArrangeFolders(BOOL bResynchLV = FALSE)
	{
		RECT rect;
		GetClientRect(&rect);

		int iFontSize = _CalcFontSize();
		if (iFontSize * (int)m_arpFolders.GetCount() > rect.bottom * (100 - OB_LISTVIEWPERCENT) / 100) {
			iFontSize = rect.bottom * (100 - OB_LISTVIEWPERCENT) / (100 * (int)m_arpFolders.GetCount());
		}

		if (m_arpFolders.GetCount()) {
			HDWP hdwp;

			ATLVERIFY((hdwp = ::BeginDeferWindowPos((int)m_arpFolders.GetCount() + 1)) != 0);

			HWND hwndLast = HWND_TOP;
			int iY = 0;

			for (size_t i = 0; i < m_arpFolders.GetCount(); i++) {
				ATLVERIFY((hdwp = ::DeferWindowPos(hdwp, m_arpFolders[i]->m_btn, hwndLast, 0, iY, rect.right, iFontSize, SWP_SHOWWINDOW)) != 0);
				iY += iFontSize;
				hwndLast = m_arpFolders[i]->m_btn;

				if (i == m_uFolderOpen) {
					if (bResynchLV) {
						_ResynchListView();
					}

					int iListSize = rect.bottom - iFontSize * (int)m_arpFolders.GetCount();
					ATLVERIFY((hdwp = ::DeferWindowPos(hdwp, m_ListIcons, hwndLast, 0, iY, rect.right, iListSize, SWP_SHOWWINDOW)) != 0);

					iY += iListSize;
					hwndLast = m_ListIcons;
				}
			}

			ATLVERIFY(::EndDeferWindowPos(hdwp));
		} else {
			if (bResynchLV) {
				_ResynchListView();
			}

			ATLVERIFY(m_ListIcons.SetWindowPos(HWND_TOP, 0, 0, rect.right, rect.bottom, SWP_SHOWWINDOW));
		}
	}

	void _NotifyItemSelect(BOOL bUserSelection = TRUE)
	{
		NMHDR nmhdr = { m_hWnd, GetDlgCtrlID(), bUserSelection ? OBN_ITEMSELECT : OBN_ITEMSELECTAUTO };
		::SendMessage(GetParent(), WM_NOTIFY, nmhdr.idFrom, (LPARAM)&nmhdr);
	}

	int _CalcFontSize()
	{
		CClientDC cdc(m_hWnd);

		HFONT hFontOld;
		ATLVERIFY((hFontOld = cdc.SelectFont(m_Font)) != NULL);

		TEXTMETRIC tm;
		ATLVERIFY(cdc.GetTextMetrics(&tm));

		ATLVERIFY((cdc.SelectFont(hFontOld)) != NULL);

		return ::MulDiv(tm.tmHeight, 14, 8);
	}

	size_t _GetItemHit(int iX, int iY)
	{
		if (m_uFolderOpen == (size_t)-1) {
			return (size_t)-1;
		}

		RECT rect;
		m_ListIcons.GetClientRect(&rect);

		int iXCaption = (rect.right - OB_TEXTWIDTH) / 2;
		if (iXCaption < 0) {
			iXCaption = 0;
		}

		BOOL bUpScrollActive = (m_arpFolders[m_uFolderOpen]->m_uOffset != 0);
		BOOL bDownScrollActive = (m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE > m_arpFolders[m_uFolderOpen]->m_uOffset + (size_t)rect.bottom);

		if (bUpScrollActive || bDownScrollActive) {
			RECT rectScrollUp, rectScrollDown;
			_CalcScrollRect(&rectScrollUp, &rectScrollDown, &rect, iXCaption + OB_TEXTWIDTH);

			if (bUpScrollActive) {
				if (iX >= rectScrollUp.left && iX < rectScrollUp.right && iY >= rectScrollUp.top && iY < rectScrollUp.bottom) {
					return OB_UPSCROLLHOT;
				}
			}

			if (bDownScrollActive) {
				if (iX >= rectScrollDown.left && iX < rectScrollDown.right && iY >= rectScrollDown.top && iY < rectScrollDown.bottom) {
					return OB_DOWNSCROLLHOT;
				}
			}
		}

		if ((iX + 1) < iXCaption || iX >= iXCaption + OB_TEXTWIDTH + 3) {
			return (size_t)-1;
		} else {
			iY += (int)m_arpFolders[m_uFolderOpen]->m_uOffset;

			size_t uItem = (size_t)(iY / OB_VERTICALICONSPACE);

			if (uItem >= m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount()) {
				return (size_t)-1;
			} else {
				int iXIcon = iXCaption + (OB_TEXTWIDTH - OB_ICONSIZE - OB_FRAMEOFFSET * 2) / 2;
				int iYIcon = (int)(uItem * OB_VERTICALICONSPACE) + OB_SPACEBEFOREFRAME;
				int iYCaption = iYIcon + OB_ICONSIZE + OB_FRAMEOFFSET * 2 + OB_SPACEAFTERFRAME;

				if (iX >= iXIcon && iX < iXIcon + OB_ICONSIZE + OB_FRAMEOFFSET * 2 && iY >= iYIcon && iY <= iYCaption) { // We can cheat a little with iY <= iYCaption
					return uItem;
				} else {
					LPCTSTR szCaption;
					if (m_arpFolders[m_uFolderOpen]->m_arpItems[uItem]->m_szCaption) {
						szCaption = m_arpFolders[m_uFolderOpen]->m_arpItems[uItem]->m_szCaption;
					} else {
						szCaption = _T("");
					}

					RECT rectMeasure = { iXCaption, iYCaption, iXCaption + OB_TEXTWIDTH, iYCaption + OB_TEXTHEIGHT };

					CClientDC cdc(m_ListIcons);
					ATLVERIFY((cdc.DrawText(szCaption, -1, &rectMeasure, DT_CALCRECT|DT_CENTER|DT_EDITCONTROL|DT_EXTERNALLEADING|DT_WORDBREAK|DT_NOPREFIX|DT_TOP|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS)) != 0);
					rectMeasure.bottom += 2;
					if (rectMeasure.bottom > iYCaption + OB_TEXTHEIGHT) {
						rectMeasure.bottom = iYCaption + OB_TEXTHEIGHT;
					}

					int iShift = (iXCaption + OB_TEXTWIDTH - rectMeasure.right) / 2;
					rectMeasure.left += iShift - 1;
					rectMeasure.right += iShift + 3;

					if (iX >= rectMeasure.left && iX < rectMeasure.right && iY >= rectMeasure.top && iY < rectMeasure.bottom) {
						return uItem;
					}

					return (size_t)-1;
				}
			}
		}
	}

	void _CalcScrollRect(RECT *prectScrollUp, RECT *prectScrollDown, const RECT *prectClient, int iXEndCaption)
	{
		int iScrollWidth = ::GetSystemMetrics(SM_CXHSCROLL);
		int iScrollHeight = ::GetSystemMetrics(SM_CXVSCROLL);

		if (iXEndCaption + iScrollWidth + iScrollWidth / 2 <= prectClient->right) {
			prectScrollUp->right = prectClient->right - iScrollWidth / 2;
			prectScrollUp->left = prectScrollUp->right - iScrollWidth;
			prectScrollDown->right = prectScrollUp->right;
			prectScrollDown->left = prectScrollUp->left;
		} else if (iXEndCaption + iScrollWidth <= prectClient->right) {
			prectScrollUp->left = iXEndCaption;
			prectScrollUp->right = prectScrollUp->left + iScrollWidth;
			prectScrollDown->right = prectScrollUp->right;
			prectScrollDown->left = prectScrollUp->left;
		} else {
			prectScrollUp->right = prectClient->right;
			prectScrollUp->left = prectScrollUp->right - iScrollWidth;
			prectScrollDown->right = prectScrollUp->right;
			prectScrollDown->left = prectScrollUp->left;
		}

		if (3 * iScrollHeight / 2 * 2 <= prectClient->bottom) {
			prectScrollUp->top = iScrollHeight / 2;
			prectScrollUp->bottom = prectScrollUp->top + iScrollHeight;
			prectScrollDown->bottom = prectClient->bottom - prectScrollUp->top;
			prectScrollDown->top = prectScrollDown->bottom - iScrollHeight;
		} else if (iScrollHeight * 2 <= prectClient->bottom) {
			prectScrollUp->top = (prectClient->bottom - iScrollWidth * 2) / 2;
			prectScrollUp->bottom = prectScrollUp->top + iScrollHeight;
			prectScrollDown->top = prectScrollUp->bottom;
			prectScrollDown->bottom = prectScrollDown->top + iScrollHeight;
		} else {
			prectScrollUp->top = 0;
			prectScrollUp->bottom = prectClient->bottom / 2;
			prectScrollDown->top = prectScrollUp->bottom;
			prectScrollDown->bottom = prectClient->bottom;
		}
	}

	void _RedrawItem(size_t uEl)
	{
		_RedrawItems(uEl, uEl);
	}

	void _RedrawItems(size_t uStart, size_t uEnd)
	{
		if (m_uFolderOpen == (size_t)-1) {
			return;
		}

		RECT rect;
		m_ListIcons.GetClientRect(&rect);

		if (uStart != (size_t)-1) {
			if ((uStart * OB_VERTICALICONSPACE) > m_arpFolders[m_uFolderOpen]->m_uOffset) {
				rect.top = (int)(uStart * OB_VERTICALICONSPACE - m_arpFolders[m_uFolderOpen]->m_uOffset);
			}
		}

		if (uEnd != (size_t)-1) {
			int iBottom = (int)((uEnd + 1) * OB_VERTICALICONSPACE) - (int)m_arpFolders[m_uFolderOpen]->m_uOffset;
			if (iBottom < rect.bottom) {
				rect.bottom = iBottom;
			}
		}

		if (rect.top < rect.bottom) {
			int iXCaption = (rect.right - OB_TEXTWIDTH) / 2;
			if (iXCaption < 0) {
				iXCaption = 0;
			}

			rect.left = iXCaption;
			rect.right = rect.left + OB_TEXTWIDTH;
			ATLVERIFY(m_ListIcons.InvalidateRect(&rect, TRUE));
		}
	}

	void _RedrawScrolls(BOOL bUp, BOOL bDown)
	{
		if (!bUp && !bDown) {
			return;
		}

		RECT rect;
		m_ListIcons.GetClientRect(&rect);

		int iXCaption = (rect.right - OB_TEXTWIDTH) / 2;
		if (iXCaption < 0) {
			iXCaption = 0;
		}

		RECT rectScrollUp, rectScrollDown;
		_CalcScrollRect(&rectScrollUp, &rectScrollDown, &rect, iXCaption + OB_TEXTWIDTH);

		if (bUp) {
			ATLVERIFY(m_ListIcons.InvalidateRect(&rectScrollUp, TRUE));
		}

		if (bDown) {
			ATLVERIFY(m_ListIcons.InvalidateRect(&rectScrollDown, TRUE));
		}
	}

	BOOL _RecalcScrolls(BOOL bInsert, size_t uFolder, size_t uItem)
	{
		RECT rect;
		m_ListIcons.GetClientRect(&rect);

		size_t uOldItemCount = m_arpFolders[uFolder]->m_arpItems.GetCount();
		if (bInsert) {
			uOldItemCount--;
		} else {
			uOldItemCount++;
		}

		size_t uFirstItem = m_arpFolders[uFolder]->m_uOffset / OB_VERTICALICONSPACE;
		size_t uLastItem = (m_arpFolders[uFolder]->m_uOffset + (size_t)rect.bottom) / OB_VERTICALICONSPACE;

		BOOL bRedrawAll = FALSE;

		if (uItem < uFirstItem) {
			if (bInsert) {
				m_arpFolders[uFolder]->m_uOffset += OB_VERTICALICONSPACE;
			} else {
				m_arpFolders[uFolder]->m_uOffset -= OB_VERTICALICONSPACE;
			}
		} else if (uFolder == m_uFolderOpen && uItem > uLastItem) {
			// Do nothing
		} else if (uFolder == m_uFolderOpen) {
			if (!bInsert && m_arpFolders[uFolder]->m_uOffset && m_arpFolders[uFolder]->m_arpItems.GetCount() * OB_VERTICALICONSPACE < m_arpFolders[uFolder]->m_uOffset + (size_t)rect.bottom) { // Overscroll
				if (m_arpFolders[uFolder]->m_arpItems.GetCount() * OB_VERTICALICONSPACE > (size_t)rect.bottom) {
					m_arpFolders[uFolder]->m_uOffset = m_arpFolders[uFolder]->m_arpItems.GetCount() * OB_VERTICALICONSPACE - (size_t)rect.bottom;
				} else {
					m_arpFolders[uFolder]->m_uOffset = 0;
				}

				bRedrawAll = TRUE;

				POINT pt;
				ATLVERIFY(::GetCursorPos(&pt));
				ATLVERIFY(m_ListIcons.ScreenToClient(&pt));
				m_uHotItem = _GetItemHit(pt.x, pt.y);
			}
		}

		if (uFolder == m_uFolderOpen) {
			BOOL bUpScrollActiveOld = (m_arpFolders[uFolder]->m_uOffset != 0);
			BOOL bDownScrollActiveOld = (uOldItemCount * OB_VERTICALICONSPACE > m_arpFolders[uFolder]->m_uOffset + (size_t)rect.bottom);

			BOOL bUpScrollActive = (m_arpFolders[uFolder]->m_uOffset != 0);
			BOOL bDownScrollActive = (m_arpFolders[uFolder]->m_arpItems.GetCount() * OB_VERTICALICONSPACE > m_arpFolders[uFolder]->m_uOffset + (size_t)rect.bottom);

			if (!bUpScrollActiveOld && !bDownScrollActiveOld && (bUpScrollActive || bDownScrollActive)) {
				_RedrawScrolls(TRUE, TRUE);
			} else if ((bUpScrollActiveOld || bDownScrollActiveOld) && (!bUpScrollActive && !bDownScrollActive)) {
				_RedrawScrolls(TRUE, TRUE);
			} else {
				_RedrawScrolls(bUpScrollActiveOld != bUpScrollActive, bDownScrollActiveOld != bDownScrollActive);
			}

			return bRedrawAll;
		} else {
			return FALSE;
		}
	}

	void _RedrawAll()
	{
		ATLVERIFY(RedrawWindow(NULL, NULL, RDW_ERASE|RDW_INVALIDATE));
	}

	void _DrawItem(CDCHandle hDC, const RECT *pRect, size_t uEl, BOOL bHasFocus)
	{
		int iXIcon = pRect->left + (OB_TEXTWIDTH - OB_ICONSIZE - OB_FRAMEOFFSET * 2) / 2;
		int iYIcon = pRect->top + OB_SPACEBEFOREFRAME;

		int iStatus;
		if (m_uFolderOpen == m_uFolderSelect && uEl == m_uItemSelect) {
			if (GetStyle() & WS_DISABLED) {
				iStatus = PBS_DISABLED;
			} else {
				iStatus = PBS_PRESSED;
			}
		} else if (uEl == m_uHotItem) {
			iStatus = PBS_HOT;
		} else {
			iStatus = PBS_NORMAL;
		}

		UINT nIconStyle = (iStatus == PBS_PRESSED && bHasFocus ? ILD_SELECTED : ILD_NORMAL);

		if (iStatus != PBS_NORMAL) {
			RECT rect = { iXIcon, iYIcon, iXIcon + OB_ICONSIZE + OB_FRAMEOFFSET * 2, iYIcon + OB_ICONSIZE + OB_FRAMEOFFSET * 2 };
			if (m_hTheme) {
				ATLVERIFY(SUCCEEDED(DrawThemeBackground(hDC, BP_PUSHBUTTON, iStatus, &rect, NULL)));
			} else {
				if (iStatus == PBS_PRESSED || iStatus == PBS_DISABLED) {
					ATLVERIFY(::DrawEdge(hDC, &rect, EDGE_SUNKEN, BF_RECT));
				}
			}
		}

		if (m_ImageList) {
			m_ImageList.Draw(hDC, m_arpFolders[m_uFolderOpen]->m_arpItems[uEl]->m_iImage, iXIcon + OB_FRAMEOFFSET, iYIcon + OB_FRAMEOFFSET, nIconStyle);
		}

		int iYCaption = iYIcon + OB_FRAMEOFFSET * 2 + OB_ICONSIZE + OB_SPACEAFTERFRAME;

		LPCTSTR szCaption;
		if (m_arpFolders[m_uFolderOpen]->m_arpItems[uEl]->m_szCaption) {
			szCaption = m_arpFolders[m_uFolderOpen]->m_arpItems[uEl]->m_szCaption;
		} else {
			szCaption = _T("");
		}

		RECT rectCaption = { pRect->left, iYCaption, pRect->left + OB_TEXTWIDTH, iYCaption + OB_TEXTHEIGHT };

		if (nIconStyle == ILD_SELECTED) {
			RECT rectMeasure = rectCaption;
			ATLVERIFY((hDC.DrawText(szCaption, -1, &rectMeasure, DT_CALCRECT|DT_CENTER|DT_EDITCONTROL|DT_EXTERNALLEADING|DT_WORDBREAK|DT_NOPREFIX|DT_TOP|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS)) != 0);
			rectMeasure.bottom += 2;
			if (rectMeasure.bottom > rectCaption.bottom) {
				rectMeasure.bottom = rectCaption.bottom;
			}

			int iShift = (rectCaption.right - rectMeasure.right) / 2;
			rectMeasure.left += iShift - 1;
			rectMeasure.right += iShift + 3;

			if (rectMeasure.right > pRect->right) {
				rectMeasure.right = pRect->right;
			}

			HBRUSH hBrush;
			ATLVERIFY((hBrush = ::GetSysColorBrush(COLOR_HIGHLIGHT)) != NULL);
			ATLVERIFY(hDC.FillRect(&rectMeasure, hBrush));
			hDC.DrawFocusRect(&rectMeasure);

			ATLVERIFY(hDC.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT)) != CLR_INVALID);
		}

		if (iStatus == PBS_HOT && m_hTheme) {
#ifndef _UNICODE
			CT2W wszCaption(szCaption);
			hDC.DrawShadowText(wszCaption, -1, &rectCaption, DT_CENTER|DT_EDITCONTROL|DT_EXTERNALLEADING|DT_WORDBREAK|DT_NOPREFIX|DT_TOP|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS, ::GetSysColor(COLOR_HIGHLIGHTTEXT), RGB(0, 0, 0), 2, 2);
#else
			hDC.DrawShadowText(szCaption, -1, &rectCaption, DT_CENTER|DT_EDITCONTROL|DT_EXTERNALLEADING|DT_WORDBREAK|DT_NOPREFIX|DT_TOP|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS, ::GetSysColor(COLOR_HIGHLIGHTTEXT), RGB(0, 0, 0), 2, 2);
#endif
		} else {
			ATLVERIFY((hDC.DrawText(szCaption, -1, &rectCaption, DT_CENTER|DT_EDITCONTROL|DT_EXTERNALLEADING|DT_WORDBREAK|DT_NOPREFIX|DT_TOP|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS)) != 0);
		}

		if (nIconStyle == ILD_SELECTED) {
			ATLVERIFY(hDC.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT)) != CLR_INVALID);
		}
	}

	HTHEME _OpenThemeData2()
	{
		if (!m_lpstrThemeClassList) {
			return NULL;
		}

		_CloseThemeData2();

		if (!IsThemingSupported()) {
			return NULL;
		}

		ATLASSERT(!m_hThemeScrollBar);
		m_hThemeScrollBar = ::OpenThemeData(m_hWnd, L"SCROLLBAR");

		return m_hThemeScrollBar;
	}

	HRESULT _CloseThemeData2()
	{
		HRESULT hRet = S_FALSE;

		if (m_hThemeScrollBar != NULL) {
			hRet = ::CloseThemeData(m_hThemeScrollBar);
			if (SUCCEEDED(hRet)) {
				m_hThemeScrollBar = NULL;
			}
		}

		return hRet;
	}

	HFONT _SetDefaultFont()
	{
		HFONT hFontOld = m_Font;

		m_Font.Detach();
		CWindowDC hdc(m_hWnd);

		DWORD dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(GetVersion())));

		if (dwWindowsMajorVersion >= 5) { // Windows 2000/XP
			m_Font.CreatePointFont(80, _T("MS Shell Dlg 2"), hdc);
		} else {
			m_Font.CreatePointFont(80, _T("MS Shell Dlg"), hdc);
		}

		m_bOwnedFont = TRUE;

		ATLVERIFY(m_Font.m_hFont);

		return hFontOld;
	}

	void _ScrollUpBtn(BOOL bReset = FALSE)
	{
		if (bReset) {
			m_uScrollTimes = 0;
			m_ListIcons.SetCapture();
		}

		if (_ScrollUp(_GetScrollPixels(m_uScrollTimes))) {
			if (m_uScrollTimes == 0) {
				ATLVERIFY(::SetTimer(m_ListIcons.m_hWnd, 0, OB_SCROLLMSEC, NULL));
			}
			m_uScrollTimes++;
		} else {
			if (m_uScrollTimes != 0) {
				::KillTimer(m_ListIcons.m_hWnd, 0);
			}
		}
	}

	void _ScrollDownBtn(BOOL bReset = FALSE)
	{
		if (bReset) {
			m_uScrollTimes = 0;
			m_ListIcons.SetCapture();
		}

		if (_ScrollDown(_GetScrollPixels(m_uScrollTimes))) {
			if (m_uScrollTimes == 0) {
				ATLVERIFY(::SetTimer(m_ListIcons.m_hWnd, 0, OB_SCROLLMSEC, NULL));
			}
			m_uScrollTimes++;
		} else {
			if (m_uScrollTimes != 0) {
				::KillTimer(m_ListIcons.m_hWnd, 0);
			}
		}
	}

	size_t _GetScrollPixels(size_t uSec)
	{
		const size_t aruScrolls[] = {10, 10, 10, 10, 20, 20, 20, 20, 40, 40, 40, 40, 80};

		if (uSec >= sizeof(aruScrolls) / sizeof(aruScrolls[0])) {
			return aruScrolls[sizeof(aruScrolls) / sizeof(aruScrolls[0]) - 1];
		} else {
			return aruScrolls[uSec];
		}
	}

	size_t _ScrollUp(size_t uScrollPixels)
	{
		RECT rect;
		m_ListIcons.GetClientRect(&rect);

		BOOL bRedrawScrollUp, bRedrawScrollDown;
		size_t uOffsetOld = m_arpFolders[m_uFolderOpen]->m_uOffset;

		if (m_arpFolders[m_uFolderOpen]->m_uOffset > uScrollPixels) {
			m_arpFolders[m_uFolderOpen]->m_uOffset -= uScrollPixels;
			if (m_uScrollTimes == 0) {
				bRedrawScrollUp = TRUE;
			} else {
				bRedrawScrollUp = FALSE;
			}
		} else {
			m_arpFolders[m_uFolderOpen]->m_uOffset = 0;

			bRedrawScrollUp = uOffsetOld != m_arpFolders[m_uFolderOpen]->m_uOffset;
		}

		bRedrawScrollDown = (m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE <= uOffsetOld + (size_t)rect.bottom && m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE > m_arpFolders[m_uFolderOpen]->m_uOffset + (size_t)rect.bottom);

		POINT pt;
		ATLVERIFY(::GetCursorPos(&pt));
		ATLVERIFY(m_ListIcons.ScreenToClient(&pt));
		m_uHotItem = _GetItemHit(pt.x, pt.y);

		_RedrawItems((size_t)-1, (size_t)-1);
		_RedrawScrolls(bRedrawScrollUp, bRedrawScrollDown);

		return m_arpFolders[m_uFolderOpen]->m_uOffset;
	}

	size_t _ScrollDown(size_t uScrollPixels)
	{
		RECT rect;
		m_ListIcons.GetClientRect(&rect);

		BOOL bRedrawScrollUp, bRedrawScrollDown;
		size_t uOffsetOld = m_arpFolders[m_uFolderOpen]->m_uOffset;

		if (m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE > m_arpFolders[m_uFolderOpen]->m_uOffset + uScrollPixels + (size_t)rect.bottom) {
			m_arpFolders[m_uFolderOpen]->m_uOffset += uScrollPixels;
			if (m_uScrollTimes == 0) {
				bRedrawScrollDown = TRUE;
			} else {
				bRedrawScrollDown = FALSE;
			}
		} else {
			m_arpFolders[m_uFolderOpen]->m_uOffset = (size_t)((int)(m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE) - rect.bottom);

			bRedrawScrollDown = uOffsetOld != m_arpFolders[m_uFolderOpen]->m_uOffset;
		}

		bRedrawScrollUp = (uOffsetOld == 0 && m_arpFolders[m_uFolderOpen]->m_uOffset > 0);

		POINT pt;
		ATLVERIFY(::GetCursorPos(&pt));
		ATLVERIFY(m_ListIcons.ScreenToClient(&pt));
		m_uHotItem = _GetItemHit(pt.x, pt.y);

		_RedrawItems((size_t)-1, (size_t)-1);
		_RedrawScrolls(bRedrawScrollUp, bRedrawScrollDown);

		return m_arpFolders[m_uFolderOpen]->m_arpItems.GetCount() * OB_VERTICALICONSPACE - m_arpFolders[m_uFolderOpen]->m_uOffset - (size_t)rect.bottom;
	}
	/*@}*/
};

typedef CWtlOutBarImplT<CWtlOutBar>	CWtlOutBarImpl; ///< Basic non-templated version of CWtlOutBarImplT

//

} // namespace WTL

//

#pragma warning(pop)

#endif // __WTLOUTBAR_H__