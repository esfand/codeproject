#include "StdAfx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

LRESULT CMainDlg::OnBnClickedActivate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
	TTActivate( IsDlgButtonChecked( wID ));
	return 0;
	}

LRESULT CMainDlg::OnEnChangeEditTt(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
	int size=SendDlgItemMessage( wID, WM_GETTEXTLENGTH,0,0 );
	TCHAR * txt= new TCHAR[++size];
	GetDlgItemText( wID, txt, size );
	TTSetTxt( m_hWnd , txt );
	delete txt;
	return 0;
	}

LRESULT CMainDlg::OnNMReleasedcaptureSizeTt(int idCtrl, LPNMHDR /*pNMHDR*/, BOOL& /*bHandled*/)
	{
	TTSize( 4 * SendDlgItemMessage(idCtrl,TBM_GETPOS,0,0));
	return 0;
	}
