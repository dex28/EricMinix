// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "EricMinix.h"

#include "EricMinixDoc.h"
#include "EricMinixView.h"
#include "LeftView.h"
#include "AddPartitionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char* cfImgFileFilter = 
    "NIU-CF Image Files (*.cfimg)\0*.cfimg\0"
    "Binary Files (*.bin)\0*.bin\0"
    "All Files (*.*)\0*.*\0"
    "\0";

/////////////////////////////////////////////////////////////////////////////

CCriticalSection lastDriveBitmapMutex;

DWORD lastDriveBitmap = 0x8000lu;

DWORD WINAPI RescanDisks_ThreadProc
(
    LPVOID lpParameter // == CLeftView* this
    )
{
    Sleep( 500 );

    for ( ;; )
    {
        DWORD driveBitmap = GetLogicalDrives () & ~3; // remove A: & B:

        for ( int i = 2; i < 32; i++ )
        {
            DWORD curDriveMask = ( DWORD(1) << i );

            if ( driveBitmap & curDriveMask ) // drive exist
            {
                char driveName[] = { 'A' + i, ':', '\0' };
                if ( DRIVE_REMOVABLE != GetDriveType( driveName ) )
                {
                    driveBitmap &= ~curDriveMask; // not "removable" media; exclude from list 
                    }
                else
                {
                    CString path = "\\\\.\\";
                    path += driveName; // e.g. for drive G: it will become \\.\G:

                    HANDLE hFile = CreateFile( path,
                        GENERIC_READ, // read only
                        FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0 
                        );
                    if ( ! hFile || hFile == INVALID_HANDLE_VALUE )
                    {
                        driveBitmap &= ~curDriveMask; // cannot be accessed
                        }
                    else
                    {
                        DWORD dwRead;
                        DISK_GEOMETRY geom;
                        BOOL rc = DeviceIoControl( hFile, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &geom, sizeof( geom ), &dwRead, NULL );
                        if ( ! rc || dwRead != sizeof( geom ) )
                        {
                            driveBitmap &= ~curDriveMask; // media not present
                            }

                        CloseHandle( hFile );
                        }
                    }
                }

            Sleep( 10 ); // relax CPU
            }

        // If there are changes, rescan systree
        //
        lastDriveBitmapMutex.Lock ();

        if ( lastDriveBitmap != driveBitmap )
        {
            lastDriveBitmap = driveBitmap;
            ( (CLeftView*)lpParameter )
                ->PostMessage( WM_COMMAND, ID_SYSTREE_ADD_REMOVE_DISKS, driveBitmap );
            }

        lastDriveBitmapMutex.Unlock ();

        Sleep( 500 ); // latency
        }

    return 0;
    }

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_COMMAND(ID_EDIT_LABEL, OnEditLabel)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_BN_CLICKED(IDC_CHECK_READ_ONLY, OnGlobalReadOnly)
	ON_COMMAND(ID_SYSTREE_RESCAN, OnSysTreeRescan)
	ON_COMMAND(ID_START_AUTOSCAN, OnStartAutoscan)
	ON_COMMAND(ID_SYSTREE_ADDDISK, OnSysTreeAddDiskImage)
	ON_COMMAND(ID_DISK_READ_ONLY, OnDiskReadOnly)
	ON_COMMAND(ID_DISK_ADDPARTITION, OnDiskAddPartition)
	ON_COMMAND(ID_DISK_COPYTOIMAGE, OnDiskCopyToImage)
	ON_COMMAND(ID_DISK_HIDE, OnDiskHide)
	ON_COMMAND(ID_DISK_EJECT, OnDiskEject)
	ON_COMMAND(ID_DISK_RESTOREIMAGE, OnDiskRestoreImage)
	ON_COMMAND(ID_DIRECTORY_COPYTO, OnDirectoryCopyTo)
	ON_COMMAND(ID_DIRECTORY_DELETE, OnDirectoryDelete)
	ON_COMMAND(ID_DIRECTORY_MERGE, OnDirectoryMerge)
	ON_COMMAND(ID_DIRECTORY_PROPERTIES, OnDirectoryProperties)
	ON_COMMAND(ID_DIRECTORY_RENAME, OnDirectoryRename)
	ON_COMMAND(ID_FILESYSTEM_COPYTOIMAGE, OnFilesystemCopyToImage)
	ON_COMMAND(ID_FILESYSTEM_DELETE, OnFilesystemDelete)
	ON_COMMAND(ID_FILESYSTEM_FORMAT, OnFilesystemFormat)
	ON_COMMAND(ID_FILESYSTEM_FSCK, OnFilesystemFSCK)
	ON_COMMAND(ID_FILESYSTEM_MOVEDOWN, OnFilesystemMoveDown)
	ON_COMMAND(ID_FILESYSTEM_MOVEUP, OnFilesystemMoveUp)
	ON_COMMAND(ID_FILESYSTEM_RESIZE, OnFilesystemResize)
	ON_COMMAND(ID_FILESYSTEM_RESTOREIMAGE, OnFilesystemRestoreImage)
	ON_UPDATE_COMMAND_UI(ID_DIRECTORY_COPYTO, OnUpdateDirectoryCopyTo)
	ON_UPDATE_COMMAND_UI(ID_DIRECTORY_DELETE, OnUpdateDirectoryDelete)
	ON_UPDATE_COMMAND_UI(ID_DIRECTORY_MERGE, OnUpdateDirectoryMerge)
	ON_UPDATE_COMMAND_UI(ID_DIRECTORY_PROPERTIES, OnUpdateDirectoryProperties)
	ON_UPDATE_COMMAND_UI(ID_DIRECTORY_RENAME, OnUpdateDirectoryRename)
	ON_UPDATE_COMMAND_UI(ID_DISK_ADDPARTITION, OnUpdateDiskAddPartition)
	ON_UPDATE_COMMAND_UI(ID_DISK_COPYTOIMAGE, OnUpdateDiskCopyToImage)
	ON_UPDATE_COMMAND_UI(ID_DISK_EJECT, OnUpdateDiskEject)
	ON_UPDATE_COMMAND_UI(ID_DISK_HIDE, OnUpdateDiskHide)
	ON_UPDATE_COMMAND_UI(ID_DISK_RESTOREIMAGE, OnUpdateDiskRestoreImage)
	ON_UPDATE_COMMAND_UI(ID_FILESYSTEM_COPYTOIMAGE, OnUpdateFilesystemCopyToImage)
	ON_UPDATE_COMMAND_UI(ID_FILESYSTEM_DELETE, OnUpdateFilesystemDelete)
	ON_UPDATE_COMMAND_UI(ID_FILESYSTEM_FORMAT, OnUpdateFilesystemFormat)
	ON_UPDATE_COMMAND_UI(ID_FILESYSTEM_FSCK, OnUpdateFilesystemFSCK)
	ON_UPDATE_COMMAND_UI(ID_FILESYSTEM_MOVEDOWN, OnUpdateFilesystemMoveDown)
	ON_UPDATE_COMMAND_UI(ID_FILESYSTEM_MOVEUP, OnUpdateFilesystemMoveUp)
	ON_UPDATE_COMMAND_UI(ID_FILESYSTEM_RESIZE, OnUpdateFilesystemResize)
	ON_UPDATE_COMMAND_UI(ID_FILESYSTEM_RESTOREIMAGE, OnUpdateFilesystemRestoreImage)
	ON_UPDATE_COMMAND_UI(ID_SYSTREE_ADDDISK, OnUpdateSysTreeAddDisk)
	ON_UPDATE_COMMAND_UI(ID_SYSTREE_RESCAN, OnUpdateSysTreeRescanDisks)
	ON_UPDATE_COMMAND_UI(ID_DISK_READ_ONLY, OnUpdateDiskReadOnly)
    ON_UPDATE_COMMAND_UI(IDC_CHECK_READ_ONLY, OnUpdateGlobalReadOnly)
	ON_COMMAND(ID_COPY_TO_IMAGE, OnCopyToImage)
	ON_UPDATE_COMMAND_UI(ID_COPY_TO_IMAGE, OnUpdateCopyToImage)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_FILESYSTEM_LIST_P_HEADERS, OnFilesystemListPHeaders)
	ON_UPDATE_COMMAND_UI(ID_FILESYSTEM_LIST_P_HEADERS, OnUpdateFilesystemListPHeaders)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView construction/destruction

CLeftView::CLeftView()
{
    hAutoscanThread = NULL;
    globalReadOnly = true;
    lastDriveBitmap = 0;
    }

CLeftView::~CLeftView()
{
    }

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTreeView::PreCreateWindow(cs);
    }

/////////////////////////////////////////////////////////////////////////////
// CLeftView drawing

void CLeftView::OnDraw(CDC* pDC)
{
	CEricMinixDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// add draw code for native data here
    }

/////////////////////////////////////////////////////////////////////////////
// CLeftView printing

BOOL CLeftView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
    }

void CLeftView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
    }

void CLeftView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
    }

void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// You may populate your TreeView with items by directly accessing
	//  its tree control through a call to tree->
    //
    view = GetMainView ();

    tree->SetImageList( &m_ImageList, TVSIL_NORMAL );
    }

/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
    }

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
    }

CEricMinixDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEricMinixDoc)));
	return (CEricMinixDoc*)m_pDocument;
    }
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

/////////////////////////////////////////////////////////////////////////////

struct TVITEM_DATA_INODE
{
    MINIX_FS* fs;
    uint32 inode_index;
    };

void CLeftView::SysTree_List( MINIX_FS* fs, HTREEITEM hParent, const char* name, uint32 inode_index, int level )
{
    inode* inode = fs->GetInode( inode_index );

    MINIX_FILE file( fs, inode_index );

    if ( ! file.IsDir () )
        return;

    CString str;
    str.Format( "%-.14s", name );
    str.TrimRight ();

    TVITEM_DATA_INODE* tvi_data = new TVITEM_DATA_INODE;
    tvi_data->fs = fs;
    tvi_data->inode_index = inode_index;

    TVINSERTSTRUCT tvi;
    tvi.hParent = hParent;
    tvi.hInsertAfter = TVI_LAST;
    tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
    tvi.item.iImage = 3;
    tvi.item.iSelectedImage = 4;
    tvi.item.pszText = (char*)(const char*)str;
    tvi.item.lParam = LPARAM( tvi_data );

    hParent = tree->InsertItem( &tvi );
    if ( level <= 1 )
    {
        tree->EnsureVisible( hParent );
        //tree->UpdateWindow ();
        }

    dir_entry dirent;

    while( ! file.IsEOF () && file.Read( &dirent ) )
    {
        if ( S_ISDIR( fs->GetInode( dirent.inode )->mode ) )
            SysTree_List( fs, hParent, dirent.name, dirent.inode, level + 1 );
        }
    }

void CLeftView::ListHeaders( MINIX_FS* fs, const char* name, uint32 inode_index )
{
    inode* inode = fs->GetInode( inode_index );

    MINIX_FILE file( fs, inode_index );

    if ( file.IsReg () )
    {
        CString str;
        str.Format( "%-14s", name );
        str.TrimRight ();
    
        char name2[ 32 ];
        strncpy( name2, name, 14 ); // max len 14
        name2[ 14 ] = 0;

        // If regular file with name ending in '.P','.p', '.D', '.d'
        // this means program/data; dump header (40 bytes allways)
        //
        CString suffix = str.Right( 2 );
        if ( suffix == ".P" || suffix == ".p" || suffix == ".D" || suffix == ".d" )
        {
            char buffer[ 80 ];
            file.Read( buffer, 40 );
            buffer[ 40 ] = 0;

            Report( "%-14s %s\n", name, buffer );
            }
        }

    if ( ! file.IsDir () )
        return;

    dir_entry dirent;

    while( ! file.IsEOF () && file.Read( &dirent ) )
    {
        if ( S_ISDIR( fs->GetInode( dirent.inode )->mode )
            || S_ISREG( fs->GetInode( dirent.inode )->mode ) 
            )
            ListHeaders( fs, dirent.name, dirent.inode );
        }
    }

void CLeftView::Add_Disk( const char* shortName, bool isImage, const char* fullPath )
{
    if ( isImage && fullPath == NULL )
        return; // fullPath must be given

    if ( isImage )
    {
        // First, try to see if the disk is already in the tree
        //
        HTREEITEM hDisk = tree->GetNextItem( NULL, TVGN_CHILD );
        while( hDisk )
        {
            MINIX_DASD* dasd = (MINIX_DASD*)tree->GetItemData( hDisk );

            if ( dasd && ! dasd->IsPhysicalDrive () )
                if ( stricmp( dasd->GetName (), fullPath ) == 0 )
                    return; // already in the Tree

            hDisk = tree->GetNextItem( hDisk, TVGN_NEXT );
            }
        }
    else switch( GetDriveType( shortName ) )
    {
        case DRIVE_REMOVABLE:
            break; // accept only removable drives

        case DRIVE_UNKNOWN:
        case DRIVE_NO_ROOT_DIR:
        case DRIVE_FIXED:
        case DRIVE_REMOTE:
        case DRIVE_CDROM:
        case DRIVE_RAMDISK:
        default:
            return;
        }

    Report( "Loading disk %s\n", shortName );
    ReportFlush ();

    MINIX_DASDWIN* dasd = new MINIX_DASDWIN( 
        isImage ? fullPath : (const char*)( CString( "\\\\.\\" ) + shortName ) 
        );

    if ( ! dasd->IsOpen () )
    {
        delete dasd;
        return;
        }

    if ( ! isImage & ! dasd->IsPhysicalDrive () )
    {
        delete dasd;
        return;
        }

    CString str;
    str.Format( "Disk (%s)", shortName );

    TVINSERTSTRUCT tvi;
    tvi.hParent = TVI_ROOT;
    tvi.hInsertAfter = TVI_LAST;
    tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE  | TVIF_PARAM;
    tvi.item.iImage = isImage ? 1 : 0;
    tvi.item.iSelectedImage = isImage ? 1 : 0;
    tvi.item.pszText = (char*)(const char*)str;
    tvi.item.lParam = LPARAM( dasd );

    HTREEITEM hDisk = tree->InsertItem( &tvi );

    dasd->ReadPartitions ();
    ReportFlush ();

    Report( "    Found %d partitions with MINIX filesystem\n", dasd->fs_len );

    for ( int i = 0; i < dasd->fs_len; i++ )
    {
        Report( "  Partition #%d:\n", i + 1 );

        CString str;
        str.Format( "Partition #%d (%.1lfMB)", i + 1, dasd->fs[ i ]->GetPartSizeInMB () );

        TVINSERTSTRUCT tvi;
        tvi.hParent = hDisk;
        tvi.hInsertAfter = TVI_LAST;
        tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE  | TVIF_PARAM;
        tvi.item.iImage = 2;
        tvi.item.iSelectedImage = 5;
        tvi.item.pszText = (char*)(const char*)str;
        tvi.item.lParam = LPARAM( dasd->fs[ i ] );

        HTREEITEM hTI = tree->InsertItem( &tvi );
        tree->EnsureVisible( hTI );

        dasd->fs[ i ]->LoadMapsAndInodes ();
        ReportFlush ();

        Report( "    Loading structure...\n" );
        SysTree_List( dasd->fs[ i ], hTI );
        ReportFlush ();

        tree->Expand( hTI, TVE_COLLAPSE );
        tree->UpdateWindow ();
        }

    tree->UpdateWindow ();

    tree->SetFocus ();

    if ( dasd->fs_len == 0 && ! dasd->IsRemovableMedia () )
    {
        tree->DeleteItem( hDisk );
        delete dasd;
        }
    else if ( isImage )
    {
        tree->EnsureVisible( hDisk );
        }
    }

void CLeftView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	*pResult = 0;

    HTREEITEM hCur = tree->GetSelectedItem ();
    if ( ! hCur )
        return;

    CString str = tree->GetItemText( hCur );
    LPARAM data = tree->GetItemData( hCur );
    if ( ! data )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    switch( nImage )
    {
        case 0: // dasd
        case 1: // dasd
            view->ShowReport ();
            Report( NULL );
            Report( "%s\n", str );
            ((MINIX_DASD*)data)->DumpInfo ();
            break;

        case 2: // partition / filesystem
            view->ShowReport ();
            Report( NULL );
            Report( "%s\n", str );
            ((MINIX_FS*)data)->DumpInfo ();
            break;

        case 3: // directory
            view->ShowFileList ();
            view->Directory_List( ((TVITEM_DATA_INODE*)data)->fs, str, ((TVITEM_DATA_INODE*)data)->inode_index );
            break;
        }
    }

void CLeftView::RescanDisks( DWORD driveBitmapADD, DWORD driveBitmapREMOVE )
{
    view->ShowReport ();
    view->DeleteFileList ();

    if ( driveBitmapREMOVE )
    {
        // Delete drives marked to be removed!
        //
        HTREEITEM hDisk = tree->GetNextItem( NULL, TVGN_CHILD );
        while( hDisk )
        {
            MINIX_DASD* dasd = (MINIX_DASD*)tree->GetItemData( hDisk );
            HTREEITEM hNext = tree->GetNextItem( hDisk, TVGN_NEXT );

            if ( ! dasd || ( dasd && dasd->IsPhysicalDrive () ) )
            {
                // Get drive letter extracted from the name \\.\X:
                // as the first character of the Right(2) of the name.
                //
                int driveLetter = CString( dasd->GetName () ).Right( 2 )[ 0 ]; 
                DWORD driveMask = DWORD( 1 ) << ( driveLetter - 'A' );

                if ( driveBitmapREMOVE & driveMask ) // scheduled to be deleted !
                {
                    tree->DeleteItem( hDisk );

                    if ( dasd )
                        delete dasd;

                    // FIXME: Delete also TVITEM_DATA_INODEs associated to subtree !!!
                    //
                    }
                }

            hDisk = hNext;
            }

        tree->UpdateData ();
        }

    Report( NULL ); // clear

    // Scan disks i.e. all logical drives, but skip A: and B:
    //
    for ( int i = 2; i < 32; i++ )
    {
        if ( driveBitmapADD & ( DWORD(1) << i ) )
        {
            char driveName[] = { 'A' + i, ':', '\0' };
            Add_Disk( driveName );
            }
        }

    // Find first item
    //
    HTREEITEM hFirst = tree->GetNextItem( NULL, TVGN_CHILD );
    if ( hFirst )
    {
        tree->EnsureVisible( hFirst );

        Report( NULL );
        Report( "MINIX filesystem scan completed.\n\n"
            "To browse: left-click on the item in the tree.\n"
            "To modify: right-click on the item in the tree.\n\n"
            "Note:\n"
            "    Do not hot-swap flash media or USB/FireWare device:\n"
            "        1) during copy or any other I/O activity\n"
            "        2) if flash media or device contains other file system\n"
            "           than MINIX that is mounted by the Windows\n"
            "           unless you have 'ejected' media first\n"
            "    Otherwise, you can hot-swap flash media at any time!\n"
            );
        }
    else if ( view->m_strReport.IsEmpty () )
    {
        Report( "MINIX filesystem scan completed.\r\n\r\n"
            "*** No filesystems found ***\r\n\r\n"
            "Please, load media into drive.\r\n\r\n"
            "Note that media could be loaded, but in 'ejected' state.\r\n"
            "In this case, you should re-load media into drive\r\n\r\n"
            );
        }
    else
    {
        view->ReportPrefix( "MINIX filesystem scan completed.\r\n\r\n"
            "*** No filesystems found ***\r\n\r\n"
            "Please, load media into drive.\r\n\r\n"
            "Note that media could be loaded, but in 'ejected' state.\r\n"
            "In this case, you should re-load media into drive\r\n\r\n"
            "See loading report for possible errors:\r\n"
            "---------------------------------------\r\n"
            );
        }
    }

void CLeftView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_DROPHILITE );
    if ( hCur )
        tree->Select( hCur, TVGN_CARET );
	
	*pResult = 0;
    }

void CLeftView::OnTimer(UINT nIDEvent) 
{
	CTreeView::OnTimer(nIDEvent);
    }

void CLeftView::OnGlobalReadOnly ()
{
    globalReadOnly = ! globalReadOnly;
    }

void CLeftView::OnSysTreeRescan() 
{
    lastDriveBitmapMutex.Lock ();
    
    DWORD driveBitmapRemove = lastDriveBitmap;
    lastDriveBitmap = 0;
    ::lastDriveBitmap = 0; // indicate to thread that we have nothing!

    lastDriveBitmapMutex.Unlock ();

    RescanDisks( 0, driveBitmapRemove ); // remove all, add nothing
    }

int CLeftView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
 	// register drop target
	m_dropTarget.Register( this );
    
    tree = &GetTreeCtrl ();

    m_ImageList.Create( 16, 16, ILC_COLOR24 | ILC_MASK, 10, 1);
    CBitmap bmp;
    bmp.LoadBitmap( IDB_BITMAP_DISK );
    m_ImageList.Add( &bmp, RGB( 255, 0, 255 ) );

    tree->ModifyStyle( 0, TVS_HASBUTTONS | TVS_EDITLABELS  | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS, 0 );

    tree->ModifyStyleEx( 0, WS_EX_ACCEPTFILES );

	return 0;
    }

void CLeftView::ContextMenu( CPoint point )
{
	CMenu menu;
	if ( ! menu.LoadMenu( IDM_MNU_CONTEXT ) )
        return;

    RECT rect;
    tree->GetWindowRect( &rect );
    
    UINT uFlags;
    HTREEITEM hCur = tree->HitTest( 
        CPoint( point.x - rect.left, point.y - rect.top ), &uFlags );

    CMenu* pPopup = NULL;

    if ( hCur == NULL || ( TVHT_NOWHERE & uFlags ) )
    {
        pPopup = menu.GetSubMenu( 0 );
        }
    else
    {
        int nImage = -1, nSelImage = -1;
        tree->GetItemImage( hCur, nImage, nSelImage );
        switch( nImage )
        {
            case 0: // dasd
            case 1: // dasd
                pPopup = menu.GetSubMenu( 1 );
                break;

            case 2: // partition / filesystem
                pPopup = menu.GetSubMenu( 2 );
                break;

            case 3: // directory
                pPopup = menu.GetSubMenu( 3 );
                break;
            }
		}

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

void CLeftView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
    // should open directory properties

	*pResult = 0;
    }

void CLeftView::OnStartAutoscan() 
{
    if ( hAutoscanThread )
        return; // already started

    DWORD dwThreadID;
    hAutoscanThread = CreateThread( NULL, 0, RescanDisks_ThreadProc, this, 0, &dwThreadID );
    }

void CLeftView::OnSysTreeAddDiskImage() 
{
    CFileDialog dlg( TRUE /*=open*/, ".cfimg" );
    dlg.m_ofn.Flags |= OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST;
    dlg.m_ofn.lpstrFilter = cfImgFileFilter;
    dlg.m_ofn.lpstrInitialDir = ".";
    if ( IDOK == dlg.DoModal () )
    {
        view->ShowReport ();

        Add_Disk( dlg.GetFileName (), true, dlg.GetPathName () );
        }
    }

void CLeftView::OnDiskReadOnly() 
{
    globalReadOnly = ! globalReadOnly;
    }

void CLeftView::OnDiskAddPartition() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 0 && nImage != 1 )
        return;

    MINIX_DASDWIN* dasd = (MINIX_DASDWIN*)tree->GetItemData( hCur );
    if ( ! dasd )
        return;

    if ( dasd->fs_len == 0 )
        return;

    long availableSpace = dasd->GetSize () 
        - dasd->fs[ dasd->fs_len - 1 ]->GetPartOffset ()
        - dasd->fs[ dasd->fs_len - 1 ]->GetPartSize ();;

    if ( availableSpace <= 0 )
    {
        MessageBox( "No available space on the disk!", "Info", MB_ICONEXCLAMATION | MB_OK );
        return;
        }

    CAddPartitionDialog dlg;

    dlg.m_UseAllAvailableSpace = true;
    dlg.m_FS_Size.Format( "%u", availableSpace );

    dlg.DoModal ();
    }

void CLeftView::OnDiskCopyToImage() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 0 && nImage != 1 )
        return;

    MINIX_DASDWIN* dasd = (MINIX_DASDWIN*)tree->GetItemData( hCur );
    if ( ! dasd )
        return;

    CFileDialog dlg( FALSE /*=saveas*/, ".cfimg" );
    dlg.m_ofn.Flags |= OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT;
    dlg.m_ofn.lpstrFilter = cfImgFileFilter;
    dlg.m_ofn.lpstrInitialDir = ".";
    if ( IDOK == dlg.DoModal () )
    {
        AfxGetMainWnd()->UpdateWindow ();
        //tree->UpdateWindow ();
        view->ShowReport ();

        CWaitCursor _wait;

        Report( NULL );
        Report( "Copying disk '%s' to binary image '%s'\n", tree->GetItemText( hCur ), dlg.GetFileName () );
        ReportAutoFlush = true;

        dasd->CopyTo( dlg.GetPathName () );

        ReportAutoFlush = false;

        Add_Disk( dlg.GetFileName (), true, dlg.GetPathName () );
        }
    }

void CLeftView::OnDiskEject() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 0 && nImage != 1 )
        return;

    MINIX_DASD* dasd = (MINIX_DASD*)tree->GetItemData( hCur );
    if ( ! dasd )
        return;

    dasd->Eject ();

    delete dasd;
    tree->DeleteItem( hCur );

    view->DeleteFileList ();

    view->ShowReport ();
    view->Report( NULL );
    }

void CLeftView::OnDiskHide() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 0 && nImage != 1 )
        return;

    MINIX_DASD* dasd = (MINIX_DASD*)tree->GetItemData( hCur );
    if ( ! dasd )
        return;

    delete dasd;
    tree->DeleteItem( hCur );

    view->DeleteFileList ();

    view->ShowReport ();
    view->Report( NULL );
    }

void CLeftView::OnDiskRestoreImage() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 0 && nImage != 1 )
        return;

    MINIX_DASD* dasd = (MINIX_DASD*)tree->GetItemData( hCur );
    if ( ! dasd )
        return;

    CFileDialog dlg( TRUE /*=open*/, ".cfimg" );
    dlg.m_ofn.Flags |= OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST;
    dlg.m_ofn.lpstrFilter = cfImgFileFilter;
    dlg.m_ofn.lpstrInitialDir = ".";
    if ( IDOK == dlg.DoModal () )
    {
        view->ShowReport ();
        Report( NULL );
        Report( "*** Limited Functionallity Version. FORBIDDEN!!! ***\n" );
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

void CLeftView::OnDirectoryCopyTo() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 3 )
        return;

    CString str = tree->GetItemText( hCur );
    TVITEM_DATA_INODE* tvi_data = (TVITEM_DATA_INODE*)tree->GetItemData( hCur );
    if ( ! tvi_data )
        return;

    LPMALLOC pMalloc;
    if ( SUCCEEDED( SHGetMalloc( &pMalloc ) ) )
    {
        CWaitCursor _wait;

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
                CString destPath = szDir;
                if ( str != "/" )
                {
                    destPath += "/";
                    destPath += str;
                    }

                AfxGetMainWnd()->UpdateWindow ();
                view->ShowReport ();

                Report( NULL );
                Report( "Copying '%s' to '%s'\n", str, destPath );
                ReportAutoFlush = true;

                Report( "*** Forbidden! ***\n" );
                //tvi_data->fs->Tree_Unpack( destPath, tvi_data->inode_index );

                Report( "Copy completed.\n" );
                ReportAutoFlush = false;
                }

            pMalloc->Free( pidl );
            pMalloc->Release ();
            }
        }
    }

void CLeftView::OnDirectoryDelete() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 3 )
        return;

    TVITEM_DATA_INODE* tvi_data = (TVITEM_DATA_INODE*)tree->GetItemData( hCur );
    if ( ! tvi_data )
        return;

    CString prompt;
    prompt.Format( "Are you sure you want to remove directory '%s'\r\n"
        "and its subdirectories?", tree->GetItemText( hCur ) );

    if ( IDOK != MessageBox( prompt, "Delete directory tree", MB_ICONQUESTION | MB_OKCANCEL ) )
        return;
    }

void CLeftView::OnDirectoryMerge() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 3 )
        return;

    TVITEM_DATA_INODE* tvi_data = (TVITEM_DATA_INODE*)tree->GetItemData( hCur );
    if ( ! tvi_data )
        return;

    CFileDialog dlg( TRUE /*=open*/ );
    dlg.m_ofn.Flags |= OFN_ENABLESIZING | OFN_EXPLORER | OFN_EXPLORER | OFN_ALLOWMULTISELECT;
    dlg.m_ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
    dlg.m_ofn.lpstrInitialDir = ".";
    if ( IDOK == dlg.DoModal () )
    {
        CWaitCursor _wait;

        tree->UpdateWindow ();
        view->ShowReport ();

        Report( NULL );
        ReportAutoFlush = true;

        POSITION pos = dlg.GetStartPosition ();
        while( pos )
        {
            Report( "Loading %s into %s\n", dlg.GetNextPathName( pos ), tree->GetItemText( hCur ) );
            }

        ReportAutoFlush = false;
        }
    }

void CLeftView::OnDirectoryProperties() 
{
    CEricMinixView* view = GetMainView();
    if ( ! view )
        return;

    // Trick: select the first item in the file list (which is current directory)
    // and do File Properties command in the file list view
    //
    view->m_FileList.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );
    view->SendMessage( WM_COMMAND, ID_FILE_PROPERTIES, 0 );
    }

void CLeftView::OnDirectoryRename() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 3 )
        return;

    TVITEM_DATA_INODE* tvi_data = (TVITEM_DATA_INODE*)tree->GetItemData( hCur );
    if ( ! tvi_data )
        return;

    tree->EditLabel( hCur );
    }

void CLeftView::OnFilesystemCopyToImage() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 2 )
        return;

    MINIX_FS* fs = (MINIX_FS*)tree->GetItemData( hCur );
    if ( ! fs )
        return;

    CFileDialog dlg( FALSE /*=saveas*/, ".cfimg" );
    dlg.m_ofn.Flags |= OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT;
    dlg.m_ofn.lpstrFilter = cfImgFileFilter;
    dlg.m_ofn.lpstrInitialDir = ".";
    if ( IDOK == dlg.DoModal () )
    {
        CWaitCursor _wait;

        AfxGetMainWnd()->UpdateWindow ();
        //tree->UpdateWindow ();
        view->ShowReport ();

        Report( NULL );
        Report( "Copying MINIX filesystem '%s' to binary image '%s'\n", tree->GetItemText( hCur ), dlg.GetFileName () );
        ReportAutoFlush = true;

        fs->CopyTo( dlg.GetPathName () );

        ReportAutoFlush = false;

        Add_Disk( dlg.GetFileName (), true, dlg.GetPathName () );
        }
    }

void CLeftView::OnFilesystemDelete() 
{
    view->ShowReport ();
    Report( NULL );
    Report( "*** Limited Functionallity Version. FORBIDDEN!!! ***\n" );
    }

void CLeftView::OnFilesystemResize() 
{
    view->ShowReport ();
    Report( NULL );
    Report( "*** Limited Functionallity Version. FORBIDDEN!!! ***\n" );
    }

void CLeftView::OnFilesystemFormat() 
{
    view->ShowReport ();
    Report( NULL );
    Report( "*** Limited Functionallity Version. FORBIDDEN!!! ***\n" );
    }

void CLeftView::OnFilesystemFSCK() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 2 )
        return;

    MINIX_FS* fs = (MINIX_FS*)tree->GetItemData( hCur );
    if ( ! fs )
        return;

    view->ShowReport ();
    Report( NULL );
    Report( "Checking filesystem...\n\n" );

    ReportAutoFlush = true;
    fs->Check ();
    ReportAutoFlush = false;
    }

void CLeftView::OnFilesystemMoveDown() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 2 )
        return;

    MINIX_FS* fs = (MINIX_FS*)tree->GetItemData( hCur );
    if ( ! fs )
        return;

    view->ShowReport ();
    Report( NULL );
    Report( "*** Limited Functionallity Version. FORBIDDEN!!! ***\n" );
    }

void CLeftView::OnFilesystemMoveUp() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 2 )
        return;

    MINIX_FS* fs = (MINIX_FS*)tree->GetItemData( hCur );
    if ( ! fs )
        return;

    view->ShowReport ();
    Report( NULL );
    Report( "*** Limited Functionallity Version. FORBIDDEN!!! ***\n" );
    }

void CLeftView::OnFilesystemRestoreImage() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 2 )
        return;

    MINIX_FS* fs = (MINIX_FS*)tree->GetItemData( hCur );
    if ( ! fs )
        return;

    CFileDialog dlg( TRUE /*=open*/, ".cfimg" );
    dlg.m_ofn.Flags |= OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST;
    dlg.m_ofn.lpstrFilter = cfImgFileFilter;
    dlg.m_ofn.lpstrInitialDir = ".";
    if ( IDOK == dlg.DoModal () )
    {
        view->ShowReport ();
        Report( NULL );
        Report( "*** Limited Functionallity Version. FORBIDDEN!!! ***\n" );
        }
    }

void CLeftView::OnFilesystemListPHeaders() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 2 )
        return;

    MINIX_FS* fs = (MINIX_FS*)tree->GetItemData( hCur );
    if ( ! fs )
        return;

    view->ShowReport ();
    Report( NULL );
    Report( "Listing headers of all program unit code (.P) and data (.D) files in filesystem.\n\n" );

    ReportAutoFlush = true;

    ListHeaders( fs );

    Report( "\nList of all program unit code (.P) and data (.D) file headers completed.\n" );

    ReportAutoFlush = false;
    }

void CLeftView::OnUpdateGlobalReadOnly(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( true );
    pCmdUI->SetCheck( globalReadOnly ? 1 : 0 );
    }

void CLeftView::OnUpdateSysTreeAddDisk(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( true );
    }

void CLeftView::OnUpdateSysTreeRescanDisks(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( true );
    }

void CLeftView::OnUpdateDirectoryCopyTo(CCmdUI* pCmdUI) 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
    {
        pCmdUI->Enable( false );
        return;
        }

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 3 )
    {
        pCmdUI->Enable( false );
        return;
        }

    CString str = tree->GetItemText( hCur );
    TVITEM_DATA_INODE* tvi_data = (TVITEM_DATA_INODE*)tree->GetItemData( hCur );
    if ( ! tvi_data )
    {
        pCmdUI->Enable( false );
        return;
        }

    pCmdUI->Enable( true );
    }

void CLeftView::OnUpdateDirectoryDelete(CCmdUI* pCmdUI) 
{
    OnUpdateDirectoryCopyTo( pCmdUI );
    }

void CLeftView::OnUpdateDirectoryMerge(CCmdUI* pCmdUI) 
{
    OnUpdateDirectoryCopyTo( pCmdUI );
    }

void CLeftView::OnUpdateDirectoryProperties(CCmdUI* pCmdUI) 
{
    OnUpdateDirectoryCopyTo( pCmdUI );
    }

void CLeftView::OnUpdateDirectoryRename(CCmdUI* pCmdUI) 
{
    // FIXME: detect if "/"
    OnUpdateDirectoryCopyTo( pCmdUI );
    }

void CLeftView::OnUpdateDiskAddPartition(CCmdUI* pCmdUI) 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
    {
        pCmdUI->Enable( false );
        return;
        }

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 0 && nImage != 1 )
    {
        pCmdUI->Enable( false );
        return;
        }

    MINIX_DASDWIN* dasd = (MINIX_DASDWIN*)tree->GetItemData( hCur );
    if ( ! dasd )
    {
        pCmdUI->Enable( false );
        return;
        }

    pCmdUI->Enable( true );
    }

void CLeftView::OnUpdateDiskReadOnly(CCmdUI* pCmdUI) 
{
    // FIXME: track localized read-only flag; if global is disabled, local is in effect
    pCmdUI->SetCheck( globalReadOnly );
    OnUpdateDiskAddPartition( pCmdUI );
    }

void CLeftView::OnUpdateDiskCopyToImage(CCmdUI* pCmdUI) 
{
    OnUpdateDiskAddPartition( pCmdUI );
    }

void CLeftView::OnUpdateDiskEject(CCmdUI* pCmdUI) 
{
    OnUpdateDiskAddPartition( pCmdUI );
    }

void CLeftView::OnUpdateDiskHide(CCmdUI* pCmdUI) 
{
    // FIXME detect if image?
    OnUpdateDiskAddPartition( pCmdUI );
    }

void CLeftView::OnUpdateDiskRestoreImage(CCmdUI* pCmdUI) 
{
    OnUpdateDiskAddPartition( pCmdUI );
    }

void CLeftView::OnUpdateFilesystemCopyToImage(CCmdUI* pCmdUI) 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
    {
        pCmdUI->Enable( false );
        return;
        }

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage != 2 )
    {
        pCmdUI->Enable( false );
        return;
        }

    MINIX_FS* fs = (MINIX_FS*)tree->GetItemData( hCur );
    if ( ! fs )
    {
        pCmdUI->Enable( false );
        return;
        }

    pCmdUI->Enable( true );
    }

void CLeftView::OnUpdateFilesystemDelete(CCmdUI* pCmdUI) 
{
    OnUpdateFilesystemCopyToImage( pCmdUI );
    }

void CLeftView::OnUpdateFilesystemFormat(CCmdUI* pCmdUI) 
{
    OnUpdateFilesystemCopyToImage( pCmdUI );
    }

void CLeftView::OnUpdateFilesystemFSCK(CCmdUI* pCmdUI) 
{
    OnUpdateFilesystemCopyToImage( pCmdUI );
    }

void CLeftView::OnUpdateFilesystemMoveDown(CCmdUI* pCmdUI) 
{
    // detect if last
    OnUpdateFilesystemCopyToImage( pCmdUI );
    }

void CLeftView::OnUpdateFilesystemMoveUp(CCmdUI* pCmdUI) 
{
    // detect if first
    OnUpdateFilesystemCopyToImage( pCmdUI );
    }

void CLeftView::OnUpdateFilesystemResize(CCmdUI* pCmdUI) 
{
    OnUpdateFilesystemCopyToImage( pCmdUI );
    }

void CLeftView::OnUpdateFilesystemRestoreImage(CCmdUI* pCmdUI) 
{
    OnUpdateFilesystemCopyToImage( pCmdUI );
    }

void CLeftView::OnUpdateFilesystemListPHeaders(CCmdUI* pCmdUI) 
{
    OnUpdateFilesystemCopyToImage( pCmdUI );
    }

void CLeftView::OnEditLabel() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    tree->EditLabel( hCur );
    }

void CLeftView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );
    if ( nImage == 3 && tree->GetItemText( hCur ) != "/" )
    {
        *pResult = 0; // allow editing
        return;
        }

    *pResult = 1; // not allowed
    }

void CLeftView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	*pResult = 0; // not accepted new name FIXME
    }

void CLeftView::OnCopyToImage() 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
        return;

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );

    if ( nImage == 0 || nImage == 1 )
        OnDiskCopyToImage ();
    else if ( nImage == 2 )
        OnFilesystemCopyToImage ();
    }

void CLeftView::OnUpdateCopyToImage(CCmdUI* pCmdUI) 
{
    HTREEITEM hCur = tree->GetNextItem( NULL, TVGN_CARET );
    if ( ! hCur )
    {
        pCmdUI->Enable( false );
        return;
        }

    int nImage = -1, nSelImage = -1;
    tree->GetItemImage( hCur, nImage, nSelImage );

    // allowed only for disks and filesystems
    if ( nImage != 0 && nImage != 1 && nImage != 2 )
    {
        pCmdUI->Enable( false );
        return;
        }

    pCmdUI->Enable( true );
    }


HGLOBAL CLeftView:: CopyToClipboard( COleDataSource* pData )
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
    dragfilelst = "test\x01";

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

void CLeftView::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	*pResult = 0;

    // FIXME: rendered / delayed data ?

    COleDataSource ods;

    // FIXME: drag & drop works for everything

    HGLOBAL hDropInfo = CopyToClipboard( &ods );

    if ( DROPEFFECT_MOVE == ods.DoDragDrop( DROPEFFECT_COPY | DROPEFFECT_MOVE ) )
    {
        // FillLstFiles ();
        }
    }

BOOL CLeftView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
    view->ShowReport ();
    Report( NULL );

    Report( "CLeftView::OnDrop / Merging files\n" );

    tree->SelectDropTarget( NULL );

	return CTreeView::OnDrop(pDataObject, dropEffect, point);
    }

DROPEFFECT CLeftView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	return CTreeView::OnDragEnter(pDataObject, dwKeyState, point);
    }

void CLeftView::OnDragLeave() 
{
    tree->SelectDropTarget( NULL );
	CTreeView::OnDragLeave();
    }

DROPEFFECT CLeftView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
    HTREEITEM hDrop = tree->HitTest( point );
    if ( hDrop )
        tree->SelectDropTarget( hDrop );

	return CTreeView::OnDragOver(pDataObject, dwKeyState, point);
    }

void CLeftView::OnDropFiles(HDROP hDropInfo) 
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

    tree->SelectDropTarget( NULL );

	CTreeView::OnDropFiles(hDropInfo);
    }

BOOL CLeftView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    if ( wParam == ID_SYSTREE_ADD_REMOVE_DISKS )
    {
        // lParam contains new driveBitmap
        //
        lastDriveBitmapMutex.Lock ();

        DWORD driveBitmapADD = ~lastDriveBitmap & lParam; // '0'-bit in last, '1'-bit in new
        DWORD driveBitmapREMOVE = lastDriveBitmap & ~lParam; // '1'-bit in last, '0'-bit in new

        lastDriveBitmap = lParam;

        lastDriveBitmapMutex.Unlock ();

        RescanDisks( driveBitmapADD, driveBitmapREMOVE );

        return TRUE;
        }

	return CTreeView::OnCommand(wParam, lParam);
    }

