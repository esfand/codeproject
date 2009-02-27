// CoolEdit.h - implementation of the CCoolEdit
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "CoolContextMenu.h"

class CCoolEdit : public CWindowImpl<CCoolEdit, CRichEditCtrl>,
                  public CRichEditCommands<CCoolEdit>,
                  public CCoolContextMenu<CCoolEdit>
{
	CMenu m_menuEdit;		// context menu
	HCURSOR m_hCursor;

public:
    DECLARE_WND_SUPERCLASS(NULL, CRichEditCtrl::GetWndClassName())

	BEGIN_MSG_MAP(CCoolEdit)
        MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
        MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenuPopup)
        CHAIN_MSG_MAP(CCoolContextMenu<CCoolEdit>)
        CHAIN_MSG_MAP_ALT(CRichEditCommands<CCoolEdit>, 1)
	END_MSG_MAP()

    LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (m_menuEdit.m_hMenu != NULL)
			return 0;

		// Cursor's horizontal and vertical position
		CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 

		// Convert client coordinates to screen coordinates 
		ClientToScreen(&pt);

		// Load the menu resource 
		if (!m_menuEdit.LoadMenu(IDR_CONTEXTMENU))
		{
			ATLTRACE(_T("Menu resource was not loaded successfully!\n"));
			return 0;
		}

		// TrackPopupMenu cannot display the menu bar so get 
		// a handle to the first shortcut menu. 
 		CMenuHandle menuPopup = m_menuEdit.GetSubMenu(0);
		if (menuPopup.m_hMenu == NULL)
		{
			ATLTRACE(_T("Submenu was not retrieved successfully!\n"));
			return 0;
		}

		// Display the shortcut menu
 		if (!menuPopup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON,
								pt.x, pt.y, m_hWnd))
		{
			ATLTRACE(_T("Context menu was not displayed successfully!\n"));
			return 0;
		}		

		// Destroy the menu and free any memory that the menu occupies
		menuPopup.DestroyMenu();
		m_menuEdit.DestroyMenu();
		m_menuEdit.m_hMenu = NULL;
		return 0;
	}

    LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		int nPos;
		UINT uId;

        // Call CCoolContextMenu's implementation
        CCoolContextMenu<CCoolEdit>::OnInitMenuPopup(uMsg, wParam, lParam, bHandled);

		// Set the cursor shape to an arrow
		m_hCursor = ::LoadCursor(NULL, IDC_ARROW);
		ATLASSERT(m_hCursor);
		::SetCursor(m_hCursor);

		CMenuHandle menuPopup = (HMENU)wParam;
		ATLASSERT(menuPopup.m_hMenu != NULL);

        for (nPos = 0; nPos < menuPopup.GetMenuItemCount(); nPos++) 
		{ 
			uId = GetMenuItemID(menuPopup, nPos); 

			switch (uId) 
			{ 
			case ID_EDIT_UNDO:
				EnableMenuItem(menuPopup, uId, CanUndo() ?
											MF_BYCOMMAND | MF_ENABLED : 
											MF_BYCOMMAND | MF_GRAYED);
				break;
			case ID_EDIT_SELECT_ALL:
				EnableMenuItem(menuPopup, uId, (GetWindowTextLength() > 0) ?
											MF_BYCOMMAND | MF_ENABLED : 
											MF_BYCOMMAND | MF_GRAYED);
				break;
			case ID_EDIT_CUT: 
			case ID_EDIT_COPY: 
			case ID_EDIT_CLEAR: 
				EnableMenuItem(menuPopup, uId, HasSelection() ?
											MF_BYCOMMAND | MF_ENABLED : 
											MF_BYCOMMAND | MF_GRAYED);
				break; 
			case ID_EDIT_PASTE: 
				EnableMenuItem(menuPopup, uId, IsClipboardFormatAvailable(CF_TEXT) ? 
											MF_BYCOMMAND | MF_ENABLED : 
											MF_BYCOMMAND | MF_GRAYED);
				break;
			default:
				break;
			}
		}

        return 0;
    }

    // Associates an image with a menu item
    void AssociateImage(CMenuItemInfo& mii, MenuItemData * pMI)
    {
        switch (mii.wID)
        {
        case ID_EDIT_UNDO:
            pMI->iImage = 17;
            break;
        case ID_EDIT_CUT:
            pMI->iImage = 3;
            break;
        case ID_EDIT_COPY:
            pMI->iImage = 4;
            break;
        case ID_EDIT_PASTE:
            pMI->iImage = 5;
            break;
        case ID_EDIT_CLEAR:
            pMI->iImage = 20;
            break;
		default:
            pMI->iImage = -1;
			break;
        }
    }
};