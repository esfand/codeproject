// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <wininet.h>
#include "resource.h"
#include <Wincrypt.h>

#include "MainDlg.h"

CMainDlg::CMainDlg() : m_HttpsFlag(true),
m_bExitRequestLoop(FALSE),
m_ActionSelected(0),
m_Timeout(-1),
m_CertFlag(false),
m_bIgnoreBadCertificates(true)
{
};

CMainDlg::~CMainDlg()
{
	CloseHandle(hConnectedEvent);
	CloseHandle(hRequestOpenedEvent);
	CloseHandle(hRequestCompleteEvent);
	::InternetSetStatusCallback(hOpen, NULL);
	InternetCloseHandle(hOpen);
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	//change the cursor to indicate something is happening
	SetCursor(LoadCursor(NULL,IDC_WAIT));

	// Handling 401 is required if there is a password.
	// The 401 boolean is a per session variable.  The
	// server will not ask until a new session is created.
	m_sHostName.LoadString(IDS_HOST);
	m_sUserName = _T("");
	m_sPassword = _T("");

	DoDataExchange(false);

	CString	sTemp;

	sTemp.LoadString(IDS_WININETTEST);
	m_ComboAction.AddString((LPCTSTR) sTemp);
	sTemp.Empty();

	sTemp.LoadString(IDS_THEME);
	m_ComboAction.AddString((LPCTSTR) sTemp);
	sTemp.Empty();

	sTemp.LoadString(IDS_CPPBUTTON);
	m_ComboAction.AddString((LPCTSTR) sTemp);
	sTemp.Empty();

	m_ComboAction.SetCurSel(0);

	// Clear the other text boxes
	CListBox * cLB = (CListBox *) &GetDlgItem(IDC_CALLBACKLIST);
	cLB ->ResetContent();
	SetDlgItemText(IDC_HEADER, _T(""));
	SetDlgItemText(IDC_RESOURCE, _T(""));


	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	m_sAgent.LoadString(IDS_AGENTSTRING);
	m_bExitRequestLoop = FALSE;

	hConnectedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hRequestOpenedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hRequestCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//create the root HINTERNET handle using the systems default
	//settings.
	hOpen = ::InternetOpen((LPCTSTR) m_sAgent, INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, INTERNET_FLAG_ASYNC);

	//check if the root HINTERNET handle has been created
	if (hOpen == NULL)
	{
		return FALSE;
	}
	else
	{
		//sets the callback function
		iscCallback = ::InternetSetStatusCallback(hOpen, (INTERNET_STATUS_CALLBACK)WinInetCallback);
		if ( INTERNET_INVALID_STATUS_CALLBACK == iscCallback)
		{
			CString	tMsg;
			MessageBox((LPCTSTR) tMsg.LoadString(IDS_INVALIDCALLBACK));
			return FALSE;
		}
	}

	return TRUE;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl , BOOL& /*bHandled*/)
{
	// Clear our reporting controls
	
	// Obtain a pointer to the callback list box
	CListBox * cLB = (CListBox *) &GetDlgItem(IDC_CALLBACKLIST);
	// Clear the contents
	cLB->ResetContent();
	
	// Clear text from the header and resource text boxes
	SetDlgItemText(IDC_HEADER, _T(""));
	SetDlgItemText(IDC_RESOURCE, _T(""));
	
	// Assign the agent string
	m_sAgent.LoadString(IDS_AGENTSTRING);
		
	// Make sure all events are not signaled.
	ResetEvent(hConnectedEvent);
	ResetEvent(hRequestOpenedEvent);
	ResetEvent(hRequestCompleteEvent);

	// Get data from user input
	DoDataExchange(TRUE);

	// Disable the OK button until all operations are complete
	::EnableWindow( hWndCtl, 0 );

	//reset the edit boxes
	SetDlgItemText(IDC_RESOURCE,_T(""));
	SetDlgItemText(IDC_HEADER,_T(""));

	// If the user checked the personal certificate checkbox,
	// we need to open the store and let them select their
	// certificate.
	if (m_CertFlag)
	{
		// Open the 'MY' certificate store to be presented to the user
		if(!(m_hMyStore = CertOpenStore(
			CERT_STORE_PROV_SYSTEM,
			0,
			NULL,
			CERT_SYSTEM_STORE_CURRENT_USER,
			L"MY")))
		{
			// Let the user know we could not open the store.
			CString tMsg, tTitle;
			GetErrorMessage(GetLastError(), &tMsg);
			MessageBox((LPCTSTR) tMsg, (LPCTSTR) tTitle.LoadString(IDS_TITLEFAILED), MB_ICONEXCLAMATION);
		}
		else
		{
			// If we get to this stage, the certificate store
			// was opened successfully.
			// Display a selection of certificates to choose from.
			m_pHSCertContext = CryptUIDlgSelectCertificateFromStore(
				m_hMyStore,
				NULL,
				NULL,
				NULL,
				CRYPTUI_SELECT_EXPIRATION_COLUMN | 
				CRYPTUI_SELECT_LOCATION_COLUMN |
				CRYPTUI_SELECT_FRIENDLYNAME_COLUMN |
				CRYPTUI_SELECT_INTENDEDUSE_COLUMN ,
				0,
				NULL);
		}

		if (!m_pHSCertContext)
		{
			CString tMsg, tTitle;
			MessageBox((LPCTSTR) tMsg.LoadString(IDS_ERR1), (LPCTSTR) tTitle.LoadString(IDS_ERRT1), MB_ICONEXCLAMATION);
		}
	}
	//start the downloads
	SendRequest(hOpen);

	::EnableWindow( hWndCtl, 1 );

	ATLTRACE2("Closing handle hConnect\n");
	return InternetCloseHandle(hConnect);
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

WinInetCallback is the callback function that is registered using 
InternetSetStatusCallback.

WinInetCallback displays the current callback in a list box with the ID,
IDC_CallbackList.  The information displayed uses m_szMemo (which 
contains the last function that was called), the 
dwStatusInformationLength (to monitor the size of the information
being returned),

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void __stdcall CMainDlg::WinInetCallback	(HINTERNET hInternet,
									 DWORD_PTR dwContext,
									 DWORD dwInternetStatus,
									 LPVOID lpvStatusInformation,
									 DWORD dwStatusInformationLength)
{
	CString sMsg;
	CMainDlg * cpMainDlg = (CMainDlg *) dwContext;

	INTERNET_ASYNC_RESULT *pRes = (INTERNET_ASYNC_RESULT *)lpvStatusInformation;

	switch (dwInternetStatus)
	{
	case INTERNET_STATUS_COOKIE_RECEIVED:
		sMsg.Format("%s: COOKIE_RECEIVED (%d)", cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_DETECTING_PROXY:
		sMsg.Format("%s: DETECTING_PROXY (%d)", cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_CLOSING_CONNECTION:
		sMsg.Format("%s: CLOSING_CONNECTION (%d)", cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		sMsg.Format("%s: CONNECTED_TO_SERVER (%s)", cpMainDlg->m_szMemo, (LPSTR) lpvStatusInformation);
		break;
	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		sMsg.Format("%s: CONNECTING_TO_SERVER (%s)", cpMainDlg->m_szMemo, (LPSTR) lpvStatusInformation);
		break;
	case INTERNET_STATUS_CONNECTION_CLOSED:
		sMsg.Format("%s: CONNECTION_CLOSED (%d)", cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_HANDLE_CLOSING:
		{
			sMsg.Format("%s: HANDLE_CLOSING (%d)", cpMainDlg->m_szMemo, dwStatusInformationLength);
		}
		break;
	case INTERNET_STATUS_HANDLE_CREATED:
		{
			if (CREATEHCONNECT == cpMainDlg->m_nCallContext)
			{
				cpMainDlg->hConnect = (HINTERNET)pRes->dwResult;
				sMsg.Format("%s: CONNECT HANDLE_CREATED (%d)", cpMainDlg->m_szMemo, dwStatusInformationLength);
				SetEvent(cpMainDlg->hConnectedEvent);
				ATLTRACE2("Setting Event hConnectedEvent\n");
			}
			else if (CREATEHREQUEST == cpMainDlg->m_nCallContext)
			{
				cpMainDlg->hRequest = (HINTERNET)pRes->dwResult;
				sMsg.Format("%s: REQUEST HANDLE_CREATED (%d)", cpMainDlg->m_szMemo, dwStatusInformationLength);
				SetEvent(cpMainDlg->hRequestOpenedEvent);
				ATLTRACE2("Setting Event hRequestOpenedEvent\n");
			}
		}
		break;
	case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
		sMsg.Format("%s: INTERMEDIATE_RESPONSE (%d)", cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_USER_INPUT_REQUIRED:
		sMsg.Format("%s: USER_INPUT_REQUIRED (%d)", 
			cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_NAME_RESOLVED:
		sMsg.Format("%s: NAME_RESOLVED (%s)", 
			cpMainDlg->m_szMemo, (LPCSTR) lpvStatusInformation);
		break;
	case INTERNET_STATUS_RECEIVING_RESPONSE:
		sMsg.Format("%s: RECEIVING_RESPONSE (%d)", 
			cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED:
		sMsg.Format("%s: RESPONSE_RECEIVED (%d)", 
			cpMainDlg->m_szMemo, *(LPDWORD) lpvStatusInformation);
		break;
	case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
		sMsg.Format("%s: CTL_RESPONSE_RECEIVED (%d)", 
			cpMainDlg->m_szMemo, *(LPDWORD) lpvStatusInformation);
		break;
	case INTERNET_STATUS_PREFETCH:
		sMsg.Format("%s: PREFETCH (%d)", 
			cpMainDlg->m_szMemo, *(LPDWORD) lpvStatusInformation);
		break;
	case INTERNET_STATUS_REDIRECT:
		// When this status is received, lpvStatusInformation points to an encoded URL.
		// We need to decode this string to prevent ATLTRACE2 from falling over, thinking
		// the % symbols are formatting information.
		{
			// Make the buffer sufficient to avoid looping below.  (Perhaps)
			LPTSTR szBuffer = new TCHAR[dwStatusInformationLength + 64];
			DWORD BufferSize = dwStatusInformationLength + 64;
			// Be sure to specifiy the following two flags together, or else your encode AND decode
			// your string.  Possible side effects can occur when this happens.  Trust me on this one.
			DWORD dwTempError = 0;
			while (!::InternetCanonicalizeUrl((LPCTSTR) lpvStatusInformation, szBuffer, &BufferSize, ICU_DECODE | ICU_NO_ENCODE))
			{
				dwTempError = ::GetLastError();
				// The following switch doesn't do much.  It simply documents the common errors
				// returned by a failed call to InternetCanonicalizeUrl. With the exception of
				// ERROR_INSUFFICIENT_BUFFER, I am not sure what sort of processing we can perform
				// at this point in the code.
				switch (dwTempError)
				{
				case ERROR_BAD_PATHNAME:
					break;
				case ERROR_INSUFFICIENT_BUFFER:
					{
						delete [] szBuffer;
						BufferSize += 64;
						szBuffer = new TCHAR[BufferSize];
					}
					break;
				case ERROR_INTERNET_INVALID_URL:
					break;
				case ERROR_INVALID_PARAMETER:
					break;
				default:
					break;
				};
			};
			
			if (!dwTempError)
				sMsg.Format("%s: REDIRECT (%s)", cpMainDlg->m_szMemo, szBuffer);
			else
				sMsg.Format("%s: REDIRECT with error (%d)", cpMainDlg->m_szMemo, dwTempError);
			delete [] szBuffer;
		}
		break;
	case INTERNET_STATUS_REQUEST_COMPLETE:
		{
			//check for errors
			if (FALSE == pRes->dwError)
			{
				//check if the completed request is from SendRequest
				if (cpMainDlg->m_szMemo.Find(_T("SendRequest")) != -1)
				{
					sMsg.Format("%s: REQUEST_COMPLETE (%d) - Main Thread", cpMainDlg->m_szMemo, pRes->dwResult);
				}
				else
				{
					sMsg.Format("%s: REQUEST_COMPLETE (%d) - Seconday Thread", cpMainDlg->m_szMemo, pRes->dwResult);
				}
			}
			else if (ERROR_INTERNET_INVALID_CA == pRes->dwError)
			{
				if (cpMainDlg->m_bIgnoreBadCertificates)
				{
					DWORD dwFlags = 0;
					const DWORD dwBuffLen = sizeof(dwFlags);
					::InternetQueryOption( cpMainDlg->hRequest, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID) &dwFlags, (LPDWORD) &dwBuffLen);
					dwFlags |= SECURITY_SET_MASK;
					::InternetSetOption( cpMainDlg->hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof( dwFlags ) );
				}
				// Save dwError so we can act on it.
				cpMainDlg->m_dwError = pRes->dwError;
				sMsg.Format("%s: INVALID_CA (%d)", cpMainDlg->m_szMemo, pRes->dwError);
			} // end if (ERROR_INTERNET_INVALID_CA == pRes->dwError)
			else if(ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED == pRes->dwError)
			{
				// The server requires a certificate
				// Save dwError so we can act on it.
				cpMainDlg->m_dwError = pRes->dwError;
				sMsg.Format("%s: CLIENT_AUTH_CERT_NEEDED (%d)", cpMainDlg->m_szMemo, pRes->dwError);

				// Check that we have a certificate pointer,
				// otherwise, it's time to choke.
				if(cpMainDlg->m_pHSCertContext == NULL)
				{
					// Save dwError so we can act on it.
					cpMainDlg->m_dwError = pRes->dwError;
					sMsg += _T(" No cert provided by user");
				}
				else
				{
					// We have a pointer (there's no guarantee it's valid)
					// We have to be careful and check all returns for errors
					// as the pointer could point to la-la land if someone has
					// closed the certificate store.

					// Attempt to set the option
					if (!::InternetSetOption( cpMainDlg->hRequest, 
						INTERNET_OPTION_CLIENT_CERT_CONTEXT ,
						(LPVOID) cpMainDlg->m_pHSCertContext, 
						sizeof(CERT_CONTEXT) ))
					{
						// Display the error.
						CString sErrMsg;
						cpMainDlg->GetErrorMessage(GetLastError(), &sErrMsg); 
						sMsg += _T(" ");
						sMsg += sErrMsg;
					}
				}
			} // end if (ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED == dwError)
			else if(ERROR_INTERNET_EXTENDED_ERROR == pRes->dwError)
			{
				DWORD dwBufSize = 0;
				DWORD dwError = 0;
				LPTSTR lpDisplayBuf = NULL;
retryResponseInfo:
					if (!::InternetGetLastResponseInfo(&dwError, lpDisplayBuf, &dwBufSize))
					{
						if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
						{
							delete [] lpDisplayBuf;
							dwBufSize += 64;
							lpDisplayBuf = new TCHAR[dwBufSize];
							if (lpDisplayBuf)
								goto retryResponseInfo;
							else
								dwError = pRes->dwError;
						}
					}
					sMsg.Format(
					"%s: EXTENDED_ERROR (%lu)", 
					cpMainDlg->m_szMemo, dwError);
					delete [] lpDisplayBuf;
					// Save dwError so we can act on it.
					cpMainDlg->m_dwError = dwError;

			}
			else // end if(ERROR_INTERNET_EXTENDED_ERROR == pRes->dwError)
			{
				CString sErrMsg;
				cpMainDlg->GetErrorMessage(pRes->dwError, &sErrMsg); 
				sMsg.Format(
					"%s: REQUEST_COMPLETE (%d) - Unhandled Error: ", 
					cpMainDlg->m_szMemo, pRes->dwError);
				sMsg += sErrMsg;
				// Save dwError so we can act on it.
				cpMainDlg->m_dwError = pRes->dwError;
			}
			SetEvent(cpMainDlg->hRequestCompleteEvent);
			ATLTRACE2("Setting Event hRequestCompleteEvent\n");
		}
		break;
	case INTERNET_STATUS_REQUEST_SENT:
			sMsg.Format("%s: REQUEST_SENT (%d)", cpMainDlg->m_szMemo, *(LPDWORD) lpvStatusInformation);
		break;
	case INTERNET_STATUS_RESOLVING_NAME:
		sMsg.Format("%s: RESOLVING_NAME (%s)", 
			cpMainDlg->m_szMemo, (LPCSTR) lpvStatusInformation);
		break;
	case INTERNET_STATUS_SENDING_REQUEST:
		sMsg.Format("%s: SENDING_REQUEST (%d)", 
			cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_STATE_CHANGE:
		sMsg.Format("%s: STATE_CHANGE (%d)", 
			cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_COOKIE_SENT:
		sMsg.Format("%s: COOKIE_SENT (%d)", 
			cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_PRIVACY_IMPACTED:
		sMsg.Format("%s: PRIVACY_IMPACTED (%d)", 
			cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_P3P_HEADER:
		sMsg.Format("%s: P3P_HEADER (%d)", 
			cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_P3P_POLICYREF:
		sMsg.Format("%s: P3P_POLICYREF (%d)", 
			cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_COOKIE_HISTORY:
		sMsg.Format("%s: COOKIE_HISTORY (%d)", 
			cpMainDlg->m_szMemo, dwStatusInformationLength);
		break;
	default:
		sMsg.Format("%s: Unknown: Status %d Given", cpMainDlg->m_szMemo, dwInternetStatus);
		break;
	}

	// Add the callback information to the callback list box
	LRESULT Err = cpMainDlg->SendDlgItemMessage(IDC_CALLBACKLIST, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)sMsg);
	if (Err == LB_ERR || Err == LB_ERRSPACE)
	{
		sMsg += _T("\n***\nUnable to post to list box\n****");
	}

	// Add a linefeed for debug output.
	// Otherwise, everything is strung together.
	sMsg += _T("\n");
	ATLTRACE2((LPCTSTR)sMsg);
	// I set this last, otherwise trace messages show up in odd places, since
	// WinInetCallback is executing in a different thread than SendRequest.
	cpMainDlg->m_dwInternetStatus = dwInternetStatus;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SendRequest handles the initial download request using 
InternetOpenUrl.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void CMainDlg::SendRequest (HINTERNET hOpen)
{
	ULONG	count = 0;
	CString sTraceBuffer;
	DWORD	dwLastError = 0;
	int		nDeniedCount = 0;

	// Initialize the action string
	CString	sAction = m_ActionSelected == 0? _T("POST") : _T("GET");

	// Initialize the post setting
	INTERNET_PORT m_Port = m_HttpsFlag ? INTERNET_DEFAULT_HTTPS_PORT: INTERNET_DEFAULT_HTTP_PORT;
	

	//update the memo in the REQUEST_CONTEXT structure
	m_szMemo.Format(_T("SendRequest (%s):"), (LPCTSTR) m_sHostName);

	// First call that will actually complete asynchronously even
	// though there is no network traffic

	m_nCallContext = CREATEHCONNECT;

	hConnect = InternetConnect(hOpen, 
		(LPCTSTR) m_sHostName, 
		m_Port,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		(DWORD_PTR)this); // Connection handle's Context

	//check the HINTERNET handle for errors
	if (hConnect == NULL)
	{
		if ((dwLastError = GetLastError()) != ERROR_IO_PENDING)
		{
			// Lookup the message for the unhandled error
			CString sMsgBuffer;
			GetErrorMessage(dwLastError, &sMsgBuffer);
			// Format the message for viewing
			sTraceBuffer.Format(_T("%s: (%d) (%x)"), (LPCTSTR) sMsgBuffer, dwLastError, hRequest);
			// Print the message to the resource window
			SetDlgItemText(IDC_RESOURCE, sTraceBuffer);
			// Print a trace message
			sTraceBuffer += _T("\n");
			ATLTRACE2((LPCTSTR) sTraceBuffer);
			return;
		}
		WaitForSingleObject(hConnectedEvent, INFINITE);
	}

	sTraceBuffer.Format(_T("Connection opened for %s, port %d, handle %x\n"), (LPCTSTR) m_sHostName, m_Port, hConnect);
	ATLTRACE2((LPCTSTR) sTraceBuffer);

	m_nCallContext = CREATEHREQUEST;

	DWORD dwFlags = 0;

	if( m_HttpsFlag )
	{
		dwFlags =	INTERNET_FLAG_NO_CACHE_WRITE |
					INTERNET_FLAG_KEEP_CONNECTION |
					INTERNET_FLAG_PRAGMA_NOCACHE |
					INTERNET_FLAG_SECURE |
					INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
					INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
	}
	else
	{
		//for HTTP
		dwFlags =	INTERNET_FLAG_NO_CACHE_WRITE |
					INTERNET_FLAG_KEEP_CONNECTION |
					INTERNET_FLAG_PRAGMA_NOCACHE ;
	}

	LPTSTR AcceptTypes[2] = {TEXT("*/*"), NULL};

	hRequest = HttpOpenRequest(hConnect, 
		(LPCTSTR) sAction, 
		(LPCTSTR) m_sRequest,
		NULL,
		NULL,
		(LPCTSTR*)AcceptTypes,
		dwFlags,
		(DWORD_PTR)this);  // Request handle's context 

	if (hRequest == NULL)
	{
		if ((dwLastError = GetLastError()) != ERROR_IO_PENDING)
		{
			// Lookup the message for the unhandled error
			CString sMsgBuffer;
			GetErrorMessage(dwLastError, &sMsgBuffer);
			// Format the message for viewing
			sTraceBuffer.Format(_T("%s: (%d) (%x)"), (LPCTSTR) sMsgBuffer, dwLastError, hRequest);
			// Print the message to the resource window
			SetDlgItemText(IDC_RESOURCE, sTraceBuffer);
			// Print a trace message
			sTraceBuffer += _T("\n");
			ATLTRACE2((LPCTSTR) sTraceBuffer);
			return;
		}
		// Wait until we get the request handle
		WaitForSingleObject(hRequestOpenedEvent, INFINITE);
	}

	// Update the trace output	
	ATLTRACE2(_T("Request opened for:\n %s\n, action %s, handle %x\n"), (LPCTSTR) m_sRequest, (LPCTSTR) sAction, hRequest);

	// If the user needs to increase the timeout value.
	// All values are in milliseconds.
	if (-1 != m_Timeout)
		::InternetSetOption(hRequest,INTERNET_OPTION_RECEIVE_TIMEOUT,(void *)&m_Timeout,sizeof(DWORD));

	// This loop handles sending the request to the server
	// There are several intermediate responses that may occur:
	// 1. The server certificate (if SSL is chosen) may have expired, etc.
	do
	{
		m_dwError = 0;
		m_dwInternetStatus = 0;
		m_bExitRequestLoop = TRUE;
		ResetEvent(hRequestCompleteEvent);

		
		sTraceBuffer.Format(_T("Sending request %x\n"), hRequest);
		ATLTRACE2((LPCTSTR) sTraceBuffer);

		if (!HttpSendRequest(hRequest, 
			NULL, 
			0, 
			NULL,
			0))
		{
			if ( ERROR_IO_PENDING != (dwLastError = GetLastError()) )
			{
				// Lookup the message for the unhandled error
				CString sMsgBuffer;
				GetErrorMessage(dwLastError, &sMsgBuffer);
				// Format the message for viewing
				sTraceBuffer.Format(_T("%s: (%d) (%x)"), (LPCTSTR) sMsgBuffer, dwLastError, hRequest);
				// Print the message to the resource window
				SetDlgItemText(IDC_RESOURCE, sTraceBuffer);
				// Print a trace message
				sTraceBuffer += _T("\n");
				ATLTRACE2((LPCTSTR) sTraceBuffer);
				return;
			}
		}

		ATLTRACE2(_T("Waiting for INTERNET_STATUS_REQUEST_COMPLETE (%x)\n"), hRequest);

		WaitForSingleObject(hRequestCompleteEvent, INFINITE);

		ATLTRACE2(_T("Received INTERNET_STATUS_REQUEST_COMPLETE (%x)\n"), hRequest);

		switch (m_dwError)
		{
		case ERROR_INTERNET_INVALID_CA:
			{
				if (m_bIgnoreBadCertificates)
				{
					ATLTRACE2(_T("Resubmitting for INVALID CA (%x)\n"), hRequest);
					m_bExitRequestLoop = FALSE;
					continue;
				}
				else
				{
					ATLTRACE2(_T("INVALID CA request received (%x)\n"), hRequest);
					m_bExitRequestLoop = FALSE;
					continue;
				}
			}
		case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
			{
				if (!m_pHSCertContext)
				{
					ATLTRACE2(_T("Server requested personal certificate, none provided\n"), hRequest);
					continue;
				}
				else
				{
					ATLTRACE2(_T("Resubmitting for CLIENT_AUTH_CERT_NEEDED (%x)\n"), hRequest);
					m_bExitRequestLoop = FALSE;
					continue;
				}
			}
		case ERROR_INTERNET_EXTENDED_ERROR:
			{
				LPTSTR szErrorBuffer = NULL;
				DWORD dwBufferLength = 0;
				DWORD dwTempError = 0;
retryResponseInfo:
				if (!InternetGetLastResponseInfo( &dwTempError, szErrorBuffer, &dwBufferLength))
				{
					if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
					{
						szErrorBuffer = new TCHAR[++dwBufferLength];
						goto retryResponseInfo;
					}
				}
				szErrorBuffer[dwBufferLength - 1] = '\0';
				SetDlgItemText(IDC_RESOURCE, szErrorBuffer);
				delete [] szErrorBuffer;
				ATLTRACE2(_T("An extended error had occured. (%x)\n"), hRequest);
				continue;
			}
		default:
			{
				if (m_dwError > 0)
				{
					// Lookup the message for the unhandled error
					CString sMsgBuffer;
					GetErrorMessage(m_dwError, &sMsgBuffer);
					// Format the message for viewing
					sTraceBuffer.Format(_T("%s: (%d) (%x)"), (LPCTSTR) sMsgBuffer, m_dwError, hRequest);
					// Print the message to the resource window
					SetDlgItemText(IDC_RESOURCE, sTraceBuffer);
					// Print a trace message
					sTraceBuffer += _T("\n");
					ATLTRACE2((LPCTSTR) sTraceBuffer);
					continue;
				}
			}
		}
		
		DWORD dwStatus = 0;
		DWORD dwStatusSize = sizeof(dwStatus);

		// Check status
		ATLTRACE2(_T("HTTPQueryInfo submitted (%x)\n"), hRequest);
		if (::HttpQueryInfo(hRequest, HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_STATUS_CODE, &dwStatus, &dwStatusSize, NULL))
		{
			switch (dwStatus)
			{
			case HTTP_STATUS_DENIED:
				{
					// Increment the denied count
					if ((nDeniedCount++ > 2))
					{
						//write error to resource edit box
						CString sErrMsg(_T("::HttpQueryInfo returned HTTP_STATUS_DENIED - Log in has failed."));
						SetDlgItemText(IDC_RESOURCE, (LPCTSTR) sErrMsg);
						sErrMsg += _T("\n");
						ATLTRACE2((LPCTSTR) sErrMsg);
						m_szMemo = _T("WriteHeader");
						WriteHeader(IDC_HEADER, hRequest);
						continue;
					}

					//write error to resource edit box
					SetDlgItemText(IDC_RESOURCE, "::HttpQueryInfo returned HTTP_STATUS_DENIED");
					ATLTRACE2("::HttpQueryInfo returned HTTP_STATUS_DENIED\n");

					if ((m_sUserName.GetLength() > 0) && (m_sPassword.GetLength() > 0))
					{
						::InternetSetOption(hRequest, INTERNET_OPTION_USERNAME, (void *) (LPCTSTR) m_sUserName, m_sUserName.GetLength());
						::InternetSetOption(hRequest, INTERNET_OPTION_PASSWORD, (void *) (LPCTSTR) m_sPassword, m_sPassword.GetLength());
						m_bExitRequestLoop = FALSE;
						ATLTRACE2(_T("Resubmitting for HTTP_STATUS_DENIED (%x)\n"), hRequest);
						continue;
					}
					else
					{
						CString sErrMsg(_T("::HttpQueryInfo returned HTTP_STATUS_DENIED, no log on info supplied."));
						SetDlgItemText(IDC_RESOURCE, (LPCTSTR) sErrMsg);
						sErrMsg += _T("\n");
						ATLTRACE2((LPCTSTR) sErrMsg);
						m_bExitRequestLoop = TRUE;
						continue;
					}
					continue;
				}
			case HTTP_STATUS_OK:
				{
					// At this point we have a web page.
					// Take steps to dump the page.
					// Reset the denied count
					nDeniedCount = 0;

					ATLTRACE2( _T("Received HTTP_STATUS_OK (%x)\n"), hRequest );
					SetDlgItemText(IDC_RESOURCE, "::HttpQueryInfo returned HTTP_STATUS_OK");

					//set m_szMemo to the name of the function to be called
					m_szMemo = _T("WriteHeader");
					//call the header function
					WriteHeader(IDC_HEADER, hRequest);

					//set m_szMemo to the name of the function to be called
					m_szMemo = _T("WriteResponse");
					//call the WriteResponse function
					WriteResponse(IDC_RESOURCE, hRequest);
					continue;
				}
			case HTTP_STATUS_SERVER_ERROR:
				{
					ATLTRACE2( _T("Received HTTP_STATUS_SERVER_ERROR (%x)\n"), hRequest );
					SetDlgItemText(IDC_RESOURCE, "::HttpQueryInfo returned HTTP_STATUS_SERVER_ERROR");

					m_szMemo = _T("WriteHeader");
					//call the header function
					WriteHeader(IDC_HEADER, hRequest);

					//set m_szMemo to the name of the function to be called
					m_szMemo = _T("WriteResponse");
					//call the WriteResponse function
					WriteResponse(IDC_RESOURCE, hRequest);

				}
			default:
				{
					//write error to resource edit box
					sTraceBuffer.Format( _T("::HttpQueryInfo returned unhandled status (%d)"), dwStatus );
					SetDlgItemText(IDC_RESOURCE, (LPCTSTR) sTraceBuffer);
					sTraceBuffer += _T("\n");
					ATLTRACE2((LPCTSTR) sTraceBuffer);
				}
			}
		}
		else
		{
				CString sMsgBuffer;
				dwLastError = GetLastError();
				GetErrorMessage(dwLastError, &sMsgBuffer);
				// Format the message for viewing
				sTraceBuffer.Format(_T("HttpQueryInfo failed - %s: (%d) (%x)"), (LPCTSTR) sMsgBuffer, dwLastError, hRequest);
				// Print the message to the resource window
				SetDlgItemText(IDC_RESOURCE, sTraceBuffer);
				// Print a trace message
				sTraceBuffer += _T("\n");
				ATLTRACE2((LPCTSTR) sTraceBuffer);
		}

	} while (!m_bExitRequestLoop);

	// Close the HINTERNET handle
	ATLTRACE2("Closing handle hRequest\n");
	InternetCloseHandle(hRequest);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

WriteHeader handles the retrieval of the HTTP header information.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int WINAPI CMainDlg::WriteHeader(int intCtrlID, HINTERNET hHttp)
{
	DWORD dwHeaderType=-1;
	DWORD dwSize=0;
	LPVOID lpOutBuffer = NULL;

	char szError[80];

	ATLTRACE2(_T("Entering WriteHeader\n"));

	//change the cursor
	SetCursor(LoadCursor(NULL,IDC_WAIT));


	//set the header type to be requested, which is all headers in
	//this case
	dwHeaderType = HTTP_QUERY_RAW_HEADERS_CRLF;	

retry:

	//call HttpQueryInfo.
	//first time with a zero buffer size to get the size of the buffer
	//needed and to check if the header exists
	if(!HttpQueryInfo(hHttp,dwHeaderType,(LPVOID)lpOutBuffer,&dwSize,NULL))
	{
		//check if the header was not found
		if (GetLastError()==ERROR_HTTP_HEADER_NOT_FOUND)
		{
			sprintf(szError,"Error %d encountered", GetLastError());
			SetDlgItemText(intCtrlID, szError);
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			return TRUE;
		}
		else
		{
			//check if the buffer was insufficient
			if (GetLastError()==ERROR_INSUFFICIENT_BUFFER)
			{
				//allocate the buffer
				lpOutBuffer = new char[dwSize+1];
				goto retry;				
			}
			else
			{
				//display other errors in header edit box
				sprintf(szError,"Error %d encountered", GetLastError());
				SetDlgItemText(intCtrlID, szError);
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				return FALSE;
			}
		}
	}

	SetDlgItemText(intCtrlID,(LPSTR)lpOutBuffer);
	delete[]lpOutBuffer;
	SetCursor(LoadCursor(NULL,IDC_ARROW));
	ATLTRACE2(_T("Exiting WriteHeader\n"));
	return 1;
}	


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int WINAPI CMainDlg::WriteResponse(int intCtrlID, HINTERNET hRequest)
{

	LPSTR	lpszData;		// buffer for the data
	DWORD	dwSize;			// size of the data available
	DWORD	dwDownloaded;	// size of the downloaded data
	DWORD	dwSizeSum=0;	// size of the data in the textbox
	LPSTR	lpszHolding;	// buffer to merge the textbox data and buffer
	ULONG	count = 0;
	char	szError[80];	// buffer for error information
    INTERNET_BUFFERS InetBuff;

	ATLTRACE2(_T("Entering WriteResponse\n"));

	// Set the cursor to an hourglass.
	SetCursor(LoadCursor(NULL,IDC_WAIT));

	// I/O is pending.
	// This loop handles reading the data.  
	do
	{
		FillMemory(&InetBuff, sizeof(InetBuff), 0);

		// The call to InternetQueryDataAvailable determines the amount of 
		// data available to download.
		if (!::InternetQueryDataAvailable( hRequest, &dwSize, 0, 0))
		{
			if (ERROR_IO_PENDING != ::GetLastError())
			{
				sprintf(szError, _T("Error %d encountered by InternetQueryDataAvailable"), GetLastError());
				SetDlgItemText(intCtrlID, szError);
				SetCursor(LoadCursor(NULL,IDC_ARROW));
			}
			break;
		}
		else
		{
			ATLTRACE2(_T("InternetQueryDataAvailable returned %d bytes\n"), dwSize);

			if (dwSize == 0)
				break;
			// Allocates a buffer of the size returned by InternetQueryDataAvailable
			lpszData = new char[dwSize+1];

			InetBuff.dwStructSize = sizeof(InetBuff);
			InetBuff.lpvBuffer = lpszData;
			InetBuff.dwBufferLength = dwSize + 1;

			// Reads the data from the HINTERNET handle.
			if(!::InternetReadFileEx(hRequest, &InetBuff, 0, (DWORD_PTR)this))
			{
				if (ERROR_IO_PENDING != ::GetLastError())
				{
					sprintf(szError,"Error %d encountered by InternetReadFile",	GetLastError());
					SetDlgItemText(intCtrlID, szError);
					SetCursor(LoadCursor(NULL,IDC_ARROW));
					delete[] lpszData;
					break;
				}

				WaitForSingleObject(hRequestCompleteEvent, INFINITE);
				ATLTRACE2(_T("Waiting for INTERNET_STATUS_REQUEST_COMPLETE (%x)\n"), hRequest);
			}

			ATLTRACE2(_T("Received INTERNET_STATUS_REQUEST_COMPLETE (%x)\n"), hRequest);

			// Save the number of downloaded bytes.
			dwDownloaded = InetBuff.dwBufferLength;

			ATLTRACE2(_T("InternetReadFileEx returned %d bytes\n"), dwDownloaded);

			if (dwDownloaded == 0)
				break;

			// Adds a null terminator to the end of the data buffer
			// InternetReadFileEx does not return a null terminated string
			// We have to subtract one from the download count so that
			// we do not overwrited the end of the buffer.
			lpszData[dwDownloaded - 1]='\0';

			// Allocates the holding buffer
			lpszHolding = new char[dwSizeSum + dwDownloaded + 1];

			// Checks if there has been any data written to the textbox
			if (dwSizeSum != 0)
			{
				// Retrieves the data stored in the textbox if any
				GetDlgItemText(intCtrlID,(LPSTR)lpszHolding, dwSizeSum);

				// Adds a null terminator at the end of the textbox data
				lpszHolding[dwSizeSum]='\0';
			}
			else
			{
				// Make the holding buffer an empty string. 
				lpszHolding[0]='\0';
			}

			// Adds the new data to the holding buffer
			strcat(lpszHolding,lpszData);

			// Writes the holding buffer to the textbox
			SetDlgItemText(intCtrlID,(LPSTR)lpszHolding);

			// Delete the two buffers
			delete[] lpszHolding;
			delete[] lpszData;

			// Add the size of the downloaded data to the textbox data size
			dwSizeSum = dwSizeSum + dwDownloaded + 1;

			// Check the size of the remaining data.  If it is zero, break.
			if (dwDownloaded == 0)
				break;
		}
	}
	while(TRUE);

	// Set the cursor back to an arrow
	SetCursor(LoadCursor(NULL,IDC_ARROW));

	ATLTRACE2(_T("Exiting WriteResponse\n"));
	// Return
	return TRUE;
}

void CMainDlg::GetErrorMessage( /* in */ DWORD dwErrIn, /* out */ CString * sMessage) 
{ 
	LPTSTR lpMsgBuf;

	if (dwErrIn >= 12000 && dwErrIn <= 12174)
	{
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_HMODULE,
			GetModuleHandle( _T("wininet.dll") ),
			dwErrIn,                          
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,               
			0,
			NULL );       
	}
	else    
	{
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwErrIn,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0,
			NULL );
	}

	*sMessage = lpMsgBuf;
	LocalFree(lpMsgBuf);
}
