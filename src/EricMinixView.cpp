// EricMinixView.cpp : implementation of the CEricMinixView class
//

#include "stdafx.h"
#include "EricMinix.h"

#include "EricMinixDoc.h"
#include "EricMinixView.h"
#include "LeftView.h"
#include "FilePropertiesDialog.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileListCtrl

CFileListCtrl::CFileListCtrl()
{
    view = NULL;
    }

CFileListCtrl::~CFileListCtrl()
{
    }

BEGIN_MESSAGE_MAP(CFileListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CFileListCtrl)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileListCtrl message handlers

void CFileListCtrl::OnDropFiles(HDROP hDropInfo) 
{
    view->ShowReport ();
    Report( NULL );

    int fileCount = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0 );

    for ( int i = 0; i < fileCount; i++ )
    {
        char szFileName[ MAX_PATH ];
        DragQueryFile(
            hDropInfo,           // handle to structure for dropped files
            i,                   // index of file to query
            szFileName,          // buffer for returned filename
            sizeof( szFileName ) // size of buffer for filename
            );

        Report( "Merging file: '%s'\n", szFileName );
        }

	CListCtrl::OnDropFiles(hDropInfo);
    }

/////////////////////////////////////////////////////////////////////////////
// CEricMinixView

IMPLEMENT_DYNCREATE(CEricMinixView, CFormView)

BEGIN_MESSAGE_MAP(CEricMinixView, CFormView)
	//{{AFX_MSG_MAP(CEricMinixView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_FILE_LIST, OnColumnclickFileList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, OnItemchangedFileList)
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_LIST, OnDblclk)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FILE_LIST, OnEndLabelEdit)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_FILE_LIST, OnBeginLabelEdit)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_FILE_LIST, OnBeginDrag)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_COPYTO, OnFileCopyTo)
	ON_COMMAND(ID_FILE_DELETE, OnFileDelete)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_COMMAND(ID_FILE_RENAME, OnFileRename)
	ON_COMMAND(ID_FILE_VIEW, OnFileView)
	ON_COMMAND(ID_EDIT_LABEL, OnEditLabel)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEricMinixView construction/destruction

CEricMinixView::CEricMinixView()
	: CFormView(CEricMinixView::IDD)
{
	//{{AFX_DATA_INIT(CEricMinixView)
	m_strReport = _T("");
	//}}AFX_DATA_INIT
	// TODO: add construction code here

    reportIsVisible = false;
    isInitialized = false;
    m_FileList.view = this;
    }

CEricMinixView::~CEricMinixView()
{
    }

void CEricMinixView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEricMinixView)
	DDX_Control(pDX, IDC_REPORT, m_Report);
	DDX_Control(pDX, IDC_FILE_LIST, m_FileList);
	DDX_Text(pDX, IDC_REPORT, m_strReport);
	//}}AFX_DATA_MAP
    }

BOOL CEricMinixView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
    }

void CEricMinixView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

    // Localized update

    ShowReport ();

    DeleteFileList ();

    if ( ! isInitialized )
    {
        isInitialized = true;

        m_Report.SetFont( &font );
        m_FileList.SetImageList( &m_FileListImageList, LVSIL_SMALL );
        m_FileList.SetImageList( &m_FileListImageListBig, LVSIL_NORMAL );

        m_FileList.InsertColumn( 0, "Name", LVCFMT_LEFT, 95 );
        m_FileList.InsertColumn( 1, "Usr", LVCFMT_LEFT, 35 );
        m_FileList.InsertColumn( 2, "Grp", LVCFMT_LEFT, 35 );
        m_FileList.InsertColumn( 3, "Oth", LVCFMT_LEFT, 35 );
        m_FileList.InsertColumn( 4, "INODE", LVCFMT_RIGHT, 50 );
        m_FileList.InsertColumn( 5, "UID", LVCFMT_RIGHT, 35 );
        m_FileList.InsertColumn( 6, "GID", LVCFMT_RIGHT, 35 );
        m_FileList.InsertColumn( 8, "Links", LVCFMT_RIGHT, 40 );
        m_FileList.InsertColumn( 8, "Size", LVCFMT_RIGHT, 65 );
        m_FileList.InsertColumn( 9, "Last Access", LVCFMT_LEFT, 115 );

        m_FileList.SetExtendedStyle( LVS_EX_FULLROWSELECT );

        if ( CSplashWnd::IsEnabledSplashScreen () )
        {
            CLeftView* view = GetLeftView ();
            if ( view )
                view->PostMessage( WM_COMMAND, ID_START_AUTOSCAN, 0 );
            }
        }
    }

/////////////////////////////////////////////////////////////////////////////
// CEricMinixView printing

BOOL CEricMinixView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
    }

void CEricMinixView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
    }

void CEricMinixView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
    }

void CEricMinixView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
    }

/////////////////////////////////////////////////////////////////////////////
// CEricMinixView diagnostics

#ifdef _DEBUG
void CEricMinixView::AssertValid() const
{
	CFormView::AssertValid();
    }

void CEricMinixView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
    }

CEricMinixDoc* CEricMinixView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEricMinixDoc)));
	return (CEricMinixDoc*)m_pDocument;
    }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEricMinixView message handlers

void CEricMinixView::OnSize(UINT nType, int cx, int cy) 
{
    CRect rect;
    GetClientRect( &rect );

    // Resize dialog, first
    SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER );

    if ( ::IsWindow( m_Report.m_hWnd ) )
        m_Report.SetWindowPos( NULL, rect.left, rect.top, rect.Width (), rect.Height (), SWP_NOZORDER );

    if ( ::IsWindow( m_FileList.m_hWnd ) )
        m_FileList.SetWindowPos( NULL, rect.left, rect.top, rect.Width (), rect.Height (), SWP_NOZORDER );
    }

HBRUSH CEricMinixView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
    static CBrush black( RGB( 0, 0, 128 ) );
    static CBrush white( RGB( 255, 255, 255 ) );

    if ( reportIsVisible )
    {
        // Set color to green on black and return the background brush.
        pDC->SetTextColor( RGB( 255, 255, 255 ) );
        pDC->SetBkColor( RGB( 0, 0, 128 ) );
        return (HBRUSH)black;
        }
    else
    {
        // Set color to black on white and return the background brush.
        pDC->SetTextColor( RGB( 0, 0, 0 ) );
        pDC->SetBkColor( RGB( 255, 255, 255 ) );
        return (HBRUSH)white;
        }

	return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
}

/////////////////////////////////////////////////////////////////////////////

struct LVITEM_DATA_INODE
{
    MINIX_FS* fs;
    uint32 inode_index;
    char name[ 32 ];
    char visible_name[ 32 ];
    uint32 size;
    int uid;
    int gid;
    int nlinks;
    uint32 time;
    uint16 mode;
    };

static bool atoz[ 10 ] = { false };

static int CALLBACK 
MyCompareProc( LPARAM lParam1, LPARAM lParam2, LPARAM lParam )
{
    LVITEM_DATA_INODE* lvi1 = (LVITEM_DATA_INODE*)lParam1;
    LVITEM_DATA_INODE* lvi2 = (LVITEM_DATA_INODE*)lParam2;

    int rc = strcmp( lvi1->visible_name, lvi2->visible_name );

    switch( lParam )
    {
        case 4: // inode
            rc = lvi1->inode_index - lvi2->inode_index;
            break;
        case 5: // uid
            rc = lvi1->uid - lvi2->uid;
            break;
        case 6: // gid
            rc = lvi1->gid - lvi2->gid;
            break;
        case 7: // links
            rc = lvi1->nlinks - lvi2->nlinks;
            break;
        case 8: // size
            rc = lvi1->size - lvi2->size;
            break;
        case 9: // time
            rc = lvi1->time - lvi2->time;
            break;
        }

    return atoz[ lParam ] ? -rc : rc;
    }

void CEricMinixView::DeleteFileList( void )
{
    // Delete file list
    //
    for ( int i = 0; i < m_FileList.GetItemCount (); i++ )
        delete (LVITEM_DATA_INODE*)m_FileList.GetItemData( i );

    m_FileList.DeleteAllItems ();
    }

void CEricMinixView::Directory_List( MINIX_FS* fs, const char* dirName, uint32 inode_index )
{
    DeleteFileList ();

    for ( int i = 0; i < 10; i++ )
        atoz[ i ] = true;

    MINIX_FILE file( fs, inode_index );

    if ( ! file.IsDir () )
        return;

    dir_entry dirent;

    for ( int nItem = 0 ;; nItem++ )
    {
        if ( nItem > 0 )
            inode_index = dirent.inode;

        inode* inode = fs->GetInode( inode_index );

        CString str;
        if ( nItem == 0 )
            str = ".";
        else
        {
            str.Format( "%-.14s", dirent.name );
            str.TrimRight ();
            }

        LVITEM_DATA_INODE* lvi_data = new LVITEM_DATA_INODE;
        lvi_data->fs = fs;
        lvi_data->inode_index = inode_index;
        strcpy( lvi_data->visible_name, str );
        strcpy( lvi_data->name, nItem == 0 ? dirName : str );
        lvi_data->mode = inode->mode;
        lvi_data->uid = inode->uid;
        lvi_data->gid = inode->gid;
        lvi_data->nlinks = inode->nlinks;
        lvi_data->size = inode->size;
        lvi_data->time = inode->time;
    
        LVITEM lvi;
        lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
        lvi.pszText = (char*)(const char*)str;
        lvi.iItem = nItem;
        lvi.iSubItem = 0;
        lvi.lParam = LPARAM( lvi_data );
        lvi.iImage = 7; // unknown by default

        switch ( inode->mode >> 12 )
        {
            case 1: lvi.iImage = 1; break;
            case 2: lvi.iImage = 2; break;
            case 4: lvi.iImage = nItem == 0 ? 8 : 3; break; // open current dir
            case 6: lvi.iImage = 4; break;
            case 8: lvi.iImage = 0; break;
            case 10: lvi.iImage = 5; break;
            case 12: lvi.iImage = 6; break;
            }

        m_FileList.InsertItem( &lvi );

        // type,ugo, size, inode, uid, gid, nlinks, timestamp, name
        //
        str.Format( "%s %s %s",
            inode->mode & S_IRUSR ? "r" : "-",
            inode->mode & S_IWUSR ? "w" : "-",
            inode->mode & S_IXUSR ? ( inode->mode & S_ISUID ? "s" : "x" ) : "-"
            );
        m_FileList.SetItemText( nItem, 1, str );

        str.Format( "%s %s %s",
            inode->mode & S_IRGRP ? "r" : "-",
            inode->mode & S_IWGRP ? "w" : "-",
            inode->mode & S_IXGRP ? ( inode->mode & S_ISGID ? "s" : "x" ) : "-"
            );
        m_FileList.SetItemText( nItem, 2, str );

        str.Format( "%s %s %s",
            inode->mode & S_IROTH ? "r" : "-",
            inode->mode & S_IWOTH ? "w" : "-",
            inode->mode & S_IXOTH ? "x" : "-"
            );
        m_FileList.SetItemText( nItem, 3, str );

        str.Format( "%lu", inode_index );
        m_FileList.SetItemText( nItem, 4, str );

        str.Format( "%hu", inode->uid );
        m_FileList.SetItemText( nItem, 5, str );

        str.Format( "%u", int( inode->gid ) );
        m_FileList.SetItemText( nItem, 6, str );

        str.Format( "%u", int( inode->nlinks ) );
        m_FileList.SetItemText( nItem, 7, str );

        if ( S_ISBLK( inode->mode ) || S_ISCHR( inode->mode ) )
        {
            str.Format( "%u, %u", ( inode->zone[ 0 ] >> 8 ) & 0xFF, inode->zone[ 0 ] & 0xFF ); // major, minor
            lvi_data->size = ( ( ( inode->zone[ 0 ] >> 8 ) & 0xFF ) << 8 ) | ( inode->zone[ 0 ] & 0xFF );
            }
        else 
        {
            str.Format( "%lu", inode->size );
            }
        m_FileList.SetItemText( nItem, 8, str );

        str.Format( "%s",(char*)aTimestamp( inode->time ) );
        m_FileList.SetItemText( nItem, 9, str );

        if ( file.IsEOF () || ! file.Read( &dirent ) )
            break;
        }
    }

void CEricMinixView::OnColumnclickFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    if ( pNMListView->iSubItem == 0 
        || ( pNMListView->iSubItem >= 4 && pNMListView->iSubItem <= 9 )
        )
    {
        atoz[ pNMListView->iSubItem ] = ! atoz[ pNMListView->iSubItem ];

        m_FileList.SortItems( MyCompareProc, pNMListView->iSubItem );
        }

	*pResult = 0;
    }

void CEricMinixView::OnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;
    }

int CEricMinixView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
 	// register drop target
	m_dropTarget.Register( this );
    
    CBitmap bmp;
    m_FileListImageList.Create( 16, 15, ILC_COLOR24 | ILC_MASK, 10, 1);
    bmp.LoadBitmap( IDB_BITMAP_FILE );
    m_FileListImageList.Add( &bmp, RGB( 255, 0, 255 ) );

    CBitmap bmpBig;
    m_FileListImageListBig.Create( 32, 32, ILC_COLOR24 | ILC_MASK, 10, 1);
    bmpBig.LoadBitmap( IDB_BITMAP_FILE_BIG );
    m_FileListImageListBig.Add( &bmpBig, RGB( 255, 0, 255 ) );

    font.CreateFont(
       16,                        // nHeight
       0,                         // nWidth
       0,                         // nEscapement
       0,                         // nOrientation
       FW_NORMAL,                 // nWeight
       FALSE,                     // bItalic
       FALSE,                     // bUnderline
       0,                         // cStrikeOut
       ANSI_CHARSET,              // nCharSet
       OUT_DEFAULT_PRECIS,        // nOutPrecision
       CLIP_DEFAULT_PRECIS,       // nClipPrecision
       DEFAULT_QUALITY,           // nQuality
       DEFAULT_PITCH,             // nPitchAndFamily
       "Courier New"                  // lpszFacename
       );

	return 0;
    }

void CEricMinixView::ContextMenu( CPoint point )
{
	CMenu menu;
	if ( ! menu.LoadMenu( IDM_MNU_CONTEXT ) )
        return;

    RECT rect;
    GetWindowRect( &rect );
    
    CMenu* pPopup = menu.GetSubMenu( 4 );
    if ( pPopup )
    {
        CPoint point;
        GetCursorPos( &point );
		pPopup->TrackPopupMenu( TPM_RIGHTBUTTON | TPM_LEFTALIGN,
						   point.x, point.y,
						   AfxGetMainWnd() // route commands through main window
                           );
        }
    }

static int CALLBACK BrowseCallbackProc
(
    HWND hwnd, 
    UINT uMsg,
    LPARAM lp, 
    LPARAM pData
    ) 
{
    TCHAR szDir[MAX_PATH];

    switch(uMsg) 
    {
        case BFFM_INITIALIZED: 
            if (GetCurrentDirectory(sizeof(szDir)/sizeof(TCHAR), szDir))
            {
                // WParam is TRUE since you are passing a path.
                // It would be FALSE if you were passing a pidl.
                SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
                }
            break;

        case BFFM_SELCHANGED: 
            // Set the status window to the currently selected path.
            if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir))
            {
                SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
                }
            break;
        }
    return 0;
    }

void CEricMinixView::OnFileCopyTo() 
{
    CWaitCursor _wait;

    POSITION pos = m_FileList.GetFirstSelectedItemPosition ();
    if ( ! pos )
        return;

    LPMALLOC pMalloc;
    if ( SUCCEEDED( SHGetMalloc( &pMalloc ) ) )
    {
        BROWSEINFO bi;
        ZeroMemory( &bi, sizeof( bi ) );
        bi.hwndOwner = m_hWnd;
        bi.pszDisplayName = 0;
        bi.lpszTitle = "Select destination folder...";
        bi.pidlRoot = 0;
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
        bi.lpfn = BrowseCallbackProc;

        LPITEMIDLIST pidl;
        pidl = SHBrowseForFolder( &bi );

        if (pidl)
        {
            TCHAR szDir[ MAX_PATH ];
            if ( SHGetPathFromIDList( pidl, szDir ) )
            {
                ShowReport ();

                ::Report( NULL );
                ReportAutoFlush = true;

                while( pos )
                {
                    int nItem = m_FileList.GetNextSelectedItem( pos );
                    ::Report( "Copying '%s' to '%s'\n", m_FileList.GetItemText( nItem, 0 ), szDir );

                    LVITEM_DATA_INODE* lvi_data = (LVITEM_DATA_INODE*)m_FileList.GetItemData( nItem );

                    if ( lvi_data )
                    {
                        CString destName = ( CString( szDir ) + "/" ) + lvi_data->name;
                        Report( "*** Forbidden! ***\n" );
                        //lvi_data->fs->Tree_Unpack( destName, lvi_data->inode_index );
                        }
                    }

                ::Report( "Copy completed.\n" );
                ReportAutoFlush = false;
                }

            pMalloc->Free( pidl );
            pMalloc->Release ();
            }
        }
    }

void CEricMinixView::OnFileDelete() 
{
    if ( m_FileList.GetSelectedCount () == 0 )
        return;

    CString prompt;
    if ( m_FileList.GetSelectedCount () > 1 )
        prompt.Format( "Are you sure you want to remove %d files?", m_FileList.GetSelectedCount () );
    else
    {
        POSITION pos = m_FileList.GetFirstSelectedItemPosition ();
        prompt.Format( "Are you sure you want to remove file '%s'?", 
            m_FileList.GetItemText( m_FileList.GetNextSelectedItem( pos ), 0 )
            );
        };

    if ( IDOK != MessageBox( prompt, "Delete files", MB_ICONQUESTION | MB_OKCANCEL ) )
        return;

    CWaitCursor _wait;

    POSITION pos = m_FileList.GetFirstSelectedItemPosition ();
    if ( ! pos )
        return;

    ShowReport ();

    ::Report( NULL );
    ReportAutoFlush = true;

    while( pos )
    {
        int nItem = m_FileList.GetNextSelectedItem( pos );
        ::Report( "Deleting '%s'\n", m_FileList.GetItemText( nItem, 0 ) );

        LVITEM_DATA_INODE* lvi_data = (LVITEM_DATA_INODE*)m_FileList.GetItemData( nItem );

        // FIXME put here real code
        }

    ::Report( "Delete files completed.\n" );
    ReportAutoFlush = false;
    }

void CEricMinixView::OnFileProperties() 
{
    if ( m_FileList.GetSelectedCount () != 1 )
        return;

    POSITION pos = m_FileList.GetFirstSelectedItemPosition ();
    int nItem = m_FileList.GetNextSelectedItem( pos );

    LVITEM_DATA_INODE* lvi_data = (LVITEM_DATA_INODE*)m_FileList.GetItemData( nItem );
    if ( ! lvi_data )
        return;

    // FIXME: NOTE if folder, should rescan subfolders for the total file size, 
    // number of subfolders and used zones
    //
    CFilePropertiesDialog dlg;

    dlg.m_FileName = lvi_data->name;

    if ( lvi_data->size >= 1024ul * 1024ul )
        dlg.m_FileSize.Format( "%.1lf MB (%lu bytes)", double( lvi_data->size ) / 1024 / 1024, lvi_data->size );
    else if ( lvi_data->size >= 1024ul )
        dlg.m_FileSize.Format( "%.1lf kB (%lu bytes)", double( lvi_data->size ) / 1024, lvi_data->size );
    else if ( lvi_data->size == 1 )
        dlg.m_FileSize.Format( "1 byte" );
    else
        dlg.m_FileSize.Format( "%lu bytes", lvi_data->size );

    uint32 sizeOnDisk = ( ( lvi_data->size + 4095 ) / 4096 ) * 4096;

    if ( sizeOnDisk >= 1024ul * 1024ul )
        dlg.m_SizeOnDisk.Format( "%.1lf MB (%lu bytes)", double( sizeOnDisk ) / 1024 / 1024, sizeOnDisk );
    else if ( sizeOnDisk >= 1024 )
        dlg.m_SizeOnDisk.Format( "%.1lf kB (%lu bytes)", double( sizeOnDisk ) / 1024, sizeOnDisk );
    else if ( sizeOnDisk == 1 )
        dlg.m_SizeOnDisk.Format( "1 byte" );
    else
        dlg.m_SizeOnDisk.Format( "%lu bytes", sizeOnDisk );

    static const char* inode_type_str [] =
    {
        "?", "Pipe", "Character Device", "?",
        "Directory", "?", "Block Device", "?",
        "Regular File", "?", "Symbolic Link", "?",
        "Socket", "?", "?", "?"
        };

    dlg.m_FileType.Format( "%s", inode_type_str[ ( lvi_data->mode >> 12 ) % 16 ]  );

    dlg.m_InodeIndex.Format( "%d", lvi_data->inode_index );
    dlg.m_NLinks.Format( "%d", lvi_data->nlinks );

    dlg.m_LastAccessTime.Format( "%s",(char*)aTimestamp( lvi_data->time ) );

    dlg.m_CheckUserRead = ( lvi_data->mode & S_IRUSR ) != 0;
    dlg.m_CheckUserWrite = ( lvi_data->mode & S_IWUSR ) != 0;
    dlg.m_CheckUserExecute = ( lvi_data->mode & S_IXUSR ) != 0;
    dlg.m_CheckGroupRead = ( lvi_data->mode & S_IRGRP ) != 0;
    dlg.m_CheckGroupWrite = ( lvi_data->mode & S_IWGRP ) != 0;
    dlg.m_CheckGroupExecute = ( lvi_data->mode & S_IXGRP ) != 0;
    dlg.m_CheckOtherRead = ( lvi_data->mode & S_IROTH ) != 0;
    dlg.m_CheckOtherWrite = ( lvi_data->mode & S_IWOTH ) != 0;
    dlg.m_CheckOtherExecute = ( lvi_data->mode & S_IXOTH ) != 0;

    dlg.m_UID.Format( "%d", lvi_data->uid );
    dlg.m_GID.Format( "%d", lvi_data->gid );

    switch( dlg.DoModal () )
    {
        case 777: // View
            OnFileView ();
            break;

        case IDOK:
            break; // Change values? Data should be verified in dialog!!!
        }
    }

void CEricMinixView::OnFileRename() 
{
    if ( m_FileList.GetSelectedCount () != 1 )
        return;

    POSITION pos = m_FileList.GetFirstSelectedItemPosition ();
    int nItem = m_FileList.GetNextSelectedItem( pos );

    LVITEM_DATA_INODE* lvi_data = (LVITEM_DATA_INODE*)m_FileList.GetItemData( nItem );
    if ( ! lvi_data )
        return;

    m_FileList.EditLabel( nItem );
    }

void CEricMinixView::OnFileView() 
{
    CWaitCursor _wait;

    // Doubleclick == View file; We can view only one file at time
    if ( m_FileList.GetSelectedCount () != 1 )
        return;

    POSITION pos = m_FileList.GetFirstSelectedItemPosition ();
    if ( ! pos )
        return;

    int nItem = m_FileList.GetNextSelectedItem( pos );

    LVITEM_DATA_INODE* lvi_data = (LVITEM_DATA_INODE*)m_FileList.GetItemData( nItem );

    if ( lvi_data && S_ISREG( lvi_data->mode ) )
    {
        char tempPath[ MAX_PATH ];
        GetTempPath( sizeof(tempPath), tempPath );

        CString destName = tempPath;
        destName += m_FileList.GetItemText( nItem, 0 ) + ".TXT";

        lvi_data->fs->Tree_Unpack( destName, lvi_data->inode_index );

        ShellExecute( NULL, "open", destName, NULL, tempPath, SW_SHOW );
        }
    }

void CEricMinixView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

    OnFileProperties ();
    }

void CEricMinixView::OnEditLabel() 
{
    OnFileRename ();
    }

void CEricMinixView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LV_DISPINFO* pLVDispInfo = (LV_DISPINFO*)pNMHDR;

    if ( m_FileList.GetSelectedCount () != 1 )
    {
        *pResult = 1; // not allowed
        return;
        }

    POSITION pos = m_FileList.GetFirstSelectedItemPosition ();
    if ( ! pos )
    {
        *pResult = 1; // not allowed
        return;
        }

    int nItem = m_FileList.GetNextSelectedItem( pos );

    if ( m_FileList.GetItemText( nItem, 0 ) == "." )
    {
        *pResult = 1; // not allowed to edit current folder
        return;
        }

    *pResult = 0; // allowed
    }

void CEricMinixView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pLVDispInfo = (LV_DISPINFO*)pNMHDR;

	*pResult = 0; // not accepted new name FIXME
    }

HGLOBAL CEricMinixView:: CopyToClipboard( COleDataSource* pData )
{
    CString dragfilelst;
/*
    for ( int iSel = -1 ;; )
    {
        iSel = m_Files.GetNextItem( iSel, LVNI_SELECTED );
        if ( iSel < 0 )
            break;

        CString ufoname;
        GetUfoViDPNT( ufoname, iSel );

        dragfilelst += ufoname + "\x01";
        }
*/
    dragfilelst = "test\x01test2\x01";

    DWORD dwAllocsize = sizeof( DROPFILES ) + 10 + dragfilelst.GetLength ();

    HGLOBAL hMem = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, dwAllocsize );

    DROPFILES* dropf = (DROPFILES*)GlobalLock( hMem );

    char* fname = (char*)dropf + sizeof( DROPFILES );
    strcpy( fname, dragfilelst );

    // now make double - null-terminated list of file names
    //
    for ( ; *fname; fname++ )
    {
        if ( *fname == '\x01' ) *fname = '\0';
        }

    dropf->pFiles = sizeof( DROPFILES ); // offset
    dropf->pt.x = 0;
    dropf->pt.y = 0;
    dropf->fNC = FALSE;
    dropf->fWide = FALSE;

    pData->CacheGlobalData( CF_HDROP, hMem );

    GlobalUnlock( hMem );

    return hMem;
    }

void CEricMinixView::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	*pResult = 0;

    COleDataSource ods;

    HGLOBAL hDropInfo = CopyToClipboard( &ods );

    if ( DROPEFFECT_MOVE == ods.DoDragDrop( DROPEFFECT_COPY | DROPEFFECT_MOVE ) )
    {
        // FillLstFiles ();
        }
    }

