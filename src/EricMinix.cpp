// EricMinix.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EricMinix.h"

#include "MainFrm.h"
#include "EricMinixDoc.h"
#include "EricMinixView.h"
#include "LeftView.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEricMinixApp

BEGIN_MESSAGE_MAP(CEricMinixApp, CWinApp)
	//{{AFX_MSG_MAP(CEricMinixApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEricMinixApp construction

CEricMinixApp::CEricMinixApp()
{
	// Construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEricMinixApp object

CEricMinixApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEricMinixApp initialization

BOOL CEricMinixApp::InitInstance()
{
	// Initialize OLE libraries
    //
	if ( ! AfxOleInit() )
	{
		AfxMessageBox( "OLE initialization failed.\r\n"
            "Make sure that the OLE libraries are the correct version.", MB_ICONSTOP | MB_OK );
		return FALSE;
	    }

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
    //
	SetRegistryKey(_T("IPTC AB"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
    //
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CEricMinixDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLeftView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

    hExclusionMap = CreateFileMapping
        ( 
        INVALID_HANDLE_VALUE, // (HANDLE) 0xffffffff,
        NULL, PAGE_READONLY,
        0, 32, 
        "EricMINIX_ExclusionMap"
        );

    if( ! hExclusionMap || hExclusionMap == INVALID_HANDLE_VALUE )
        return FALSE;
    else if( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        //AfxMessageBox( "Application is already running.", MB_ICONSTOP | MB_OK );
        //return FALSE;
        }

	// Parse command line for standard shell commands, DDE, file open
    //
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

    if ( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen
        && ! cmdInfo.m_strFileName.IsEmpty () 
        )
    {
        cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
        }
    else if ( cmdInfo.m_nShellCommand != CCommandLineInfo::FileNew )
    {
        return FALSE;
        }
    else
    {
	    CSplashWnd::EnableSplashScreen( cmdInfo.m_bShowSplash );
        }

	// Dispatch commands specified on the command line
    //
	if ( ! ProcessShellCommand( cmdInfo ) )
		return FALSE;

	// The one and only window has been initialized, so show and update it.
    //
    //m_pMainWnd->SetWindowPos( NULL, 100, 100, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW );
	m_pMainWnd->ShowWindow( SW_SHOW );
	m_pMainWnd->UpdateWindow ();

	// Enable drag/drop open
    //
	m_pMainWnd->DragAcceptFiles ();

	return TRUE;
    }

int CEricMinixApp::ExitInstance() 
{
    if ( hExclusionMap && hExclusionMap != INVALID_HANDLE_VALUE )
        ::CloseHandle( hExclusionMap );
	
	return CWinApp::ExitInstance();
    }

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CEricMinixApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////

CEricMinixView* GetMainView( void )
{
    if ( ! theApp.GetMainWnd() )
        return NULL;

    CDocument* doc = ((CMainFrame*)theApp.GetMainWnd())->GetActiveDocument();
    if ( ! doc )
        return NULL;

    POSITION pos = doc->GetFirstViewPosition ();

    for ( ;; )
    {
        CView* v = doc->GetNextView( pos );
        if ( v->IsKindOf( RUNTIME_CLASS( CEricMinixView ) ) )
            return (CEricMinixView*)v;
        if ( ! v ) break;
        }

    return NULL;
    }

CLeftView* GetLeftView( void )
{
    if ( ! theApp.GetMainWnd() )
        return NULL;

    CDocument* doc = ((CMainFrame*)theApp.GetMainWnd())->GetActiveDocument();
    if ( ! doc )
        return NULL;

    POSITION pos = doc->GetFirstViewPosition ();

    for ( ;; )
    {
        CView* v = doc->GetNextView( pos );
        if ( v->IsKindOf( RUNTIME_CLASS( CLeftView ) ) )
            return (CLeftView*)v;
        if ( ! v ) break;
        }

    return NULL;
    }

/////////////////////////////////////////////////////////////////////////////
// CEricMinixApp message handlers

BOOL CEricMinixApp::PreTranslateMessage(MSG* pMsg)
{
	if ( CSplashWnd::PreTranslateAppMessage( pMsg ) )
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
    }
