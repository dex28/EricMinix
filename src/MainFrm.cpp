// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "EricMinix.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "EricMinixView.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

bool ReportAutoFlush = false;

void ReportFlush( void )
{
    CEricMinixView* view = GetMainView ();
    if ( ! view )
        return;
    
    view->ReportUpdateWindow ();
    }

void Report( const char* format, ... )
{
    CEricMinixView* view = GetMainView ();
    if ( ! view )
        return;

    if ( format == NULL ) // clear report
    {
        view->Report( NULL );
        return;
        }

    va_list argptr;
    va_start( argptr, format );

    char buffer[ 1024 ];
    vsprintf( buffer, format, argptr );

    // Convert 'LF' to 'CRLF' (only if 'LF' is not preceeded by 'CR' )
    char buffer2[ 1024 ];
    char* bufp = buffer2;
    for ( unsigned i = 0; i < strlen( buffer ); i++ )
    {
        if ( buffer[ i ] == '\n' && ( i == 0 || buffer[ i - 1 ] != '\r' ) )
        {
            *bufp++ = '\r';
            *bufp++ = '\n';
            }
        else
        {
            *bufp++ = buffer[ i ];
            }
        }
    *bufp = 0;

    view->Report( buffer2 );
    if ( ReportAutoFlush )
        view->ReportUpdateWindow( true /* show last line */ );
    }

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZING()
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_BUSY, OnUpdateStatusBar)
	ON_COMMAND(ID_VIEW_SMALLICON, OnViewSmallIcon)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SMALLICON, OnUpdateViewSmallIcon)
	ON_COMMAND(ID_VIEW_DETAILS, OnViewDetails)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DETAILS, OnUpdateViewDetails)
	ON_COMMAND(ID_VIEW_LARGEICON, OnViewLargeIcon)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LARGEICON, OnUpdateViewLargeIcon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here

}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;
	
	if ( ! m_wndToolBar.CreateEx(this) ||
		 ! m_wndToolBar.LoadToolBar(IDR_MAINFRAME)
         )
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	    }

	if ( ! m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		 CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR)
        )
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	    }

	if ( ! m_wndReBar.Create(this) ||
		 ! m_wndReBar.AddBar(&m_wndToolBar) ||
		 ! m_wndReBar.AddBar(&m_wndDlgBar)
        )
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	    }

    static UINT indicators[] =
    {
	    ID_SEPARATOR,           // status line indicator
	    ID_INDICATOR_CAPS,
	    ID_INDICATOR_NUM,
	    ID_INDICATOR_SCRL,
        ID_INDICATOR_BUSY,
        };

	if ( ! m_wndStatusBar.Create(this) ||
		 ! m_wndStatusBar.SetIndicators(indicators,
            sizeof(indicators)/sizeof(UINT))
        )
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	    }

	// Remove this if you don't want tool tips
    //
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY );

    m_wndStatusBar.SetPaneInfo( 0, ID_SEPARATOR, SBPS_STRETCH, 0 );

	CSplashWnd::ShowSplashScreen( this ); 

	return 0;
    }

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
    //
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(230, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CEricMinixView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

    // Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	CFrameWnd::OnTimer(nIDEvent);
    }

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CFrameWnd::OnSizing(fwSide, pRect);
    }

void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    if ( point.x == -1 && point.y == -1 )
    {
        // keyboard invocation
        GetCursorPos( &point );
        }

    CRect rect;

    GetLeftView()->GetWindowRect( &rect );
    if ( rect.PtInRect( point ) )
        GetLeftView()->ContextMenu( point );

    GetMainView()->GetWindowRect( &rect );
    if ( rect.PtInRect( point ) )
        GetMainView()->ContextMenu( point );
    }

void CMainFrame::OnUpdateStatusBar(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable ();
    }

void CMainFrame::OnViewSmallIcon() 
{
    CEricMinixView* view = GetMainView ();
    if ( ! view )
        return;

    view->SetViewStyle( LVS_SMALLICON );
    }

void CMainFrame::OnUpdateViewSmallIcon(CCmdUI* pCmdUI) 
{
    CEricMinixView* view = GetMainView ();
    if ( ! view )
        return;

    if ( ! IsWindow( view->m_FileList ) )
        return;

    if ( view->GetViewStyle () == LVS_SMALLICON )
        pCmdUI->SetCheck( 1 );
    else
        pCmdUI->SetCheck( 0 );
    }

void CMainFrame::OnViewDetails() 
{
    CEricMinixView* view = GetMainView ();
    if ( ! view )
        return;

    view->SetViewStyle( LVS_REPORT );
    }

void CMainFrame::OnUpdateViewDetails(CCmdUI* pCmdUI) 
{
    CEricMinixView* view = GetMainView ();
    if ( ! view )
        return;

    if ( ! IsWindow( view->m_FileList ) )
        return;

    if ( view->GetViewStyle () == LVS_REPORT )
        pCmdUI->SetCheck( 1 );
    else
        pCmdUI->SetCheck( 0 );
    }

void CMainFrame::OnViewLargeIcon() 
{
    CEricMinixView* view = GetMainView ();
    if ( ! view )
        return;

    view->SetViewStyle( LVS_ICON );
    }

void CMainFrame::OnUpdateViewLargeIcon(CCmdUI* pCmdUI) 
{
    CEricMinixView* view = GetMainView ();
    if ( ! view )
        return;

    if ( ! IsWindow( view->m_FileList ) )
        return;

    DWORD rc = view->GetViewStyle ();
    if ( rc == LVS_ICON )
        pCmdUI->SetCheck( 1 );
    else
        pCmdUI->SetCheck( 0 );
    }
