// FilePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "EricMinix.h"
#include "FilePropertiesDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilePropertiesDialog dialog


CFilePropertiesDialog::CFilePropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFilePropertiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilePropertiesDialog)
	m_LastAccessTime = _T("");
	m_CheckGroupRead = FALSE;
	m_CheckGroupWrite = FALSE;
	m_CheckGroupExecute = FALSE;
	m_CheckOtherRead = FALSE;
	m_CheckOtherWrite = FALSE;
	m_CheckOtherExecute = FALSE;
	m_CheckUserRead = FALSE;
	m_CheckUserWrite = FALSE;
	m_CheckUserExecute = FALSE;
	m_FileName = _T("");
	m_FileSize = _T("");
	m_FileType = _T("");
	m_InodeIndex = _T("");
	m_NLinks = _T("");
	m_SizeOnDisk = _T("");
	m_GID = _T("");
	m_UID = _T("");
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    }

void CFilePropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilePropertiesDialog)
	DDX_Control(pDX, IDC_FILE_NAME, m_FileNameCtrl);
	DDX_Control(pDX, IDVIEW, m_ViewButton);
	DDX_Text(pDX, IDC_ACCESS_TIME, m_LastAccessTime);
	DDX_Check(pDX, IDC_CHECK_GROUP_R, m_CheckGroupRead);
	DDX_Check(pDX, IDC_CHECK_GROUP_W, m_CheckGroupWrite);
	DDX_Check(pDX, IDC_CHECK_GROUP_X, m_CheckGroupExecute);
	DDX_Check(pDX, IDC_CHECK_OTHER_R, m_CheckOtherRead);
	DDX_Check(pDX, IDC_CHECK_OTHER_W, m_CheckOtherWrite);
	DDX_Check(pDX, IDC_CHECK_OTHER_X, m_CheckOtherExecute);
	DDX_Check(pDX, IDC_CHECK_USER_R, m_CheckUserRead);
	DDX_Check(pDX, IDC_CHECK_USER_W, m_CheckUserWrite);
	DDX_Check(pDX, IDC_CHECK_USER_X, m_CheckUserExecute);
	DDX_Text(pDX, IDC_FILE_NAME, m_FileName);
	DDX_Text(pDX, IDC_FILE_SIZE, m_FileSize);
	DDX_Text(pDX, IDC_FILE_TYPE, m_FileType);
	DDX_Text(pDX, IDC_INODE_INDEX, m_InodeIndex);
	DDX_Text(pDX, IDC_NLINKS, m_NLinks);
	DDX_Text(pDX, IDC_SIZE_ON_DISK, m_SizeOnDisk);
	DDX_Text(pDX, IDC_GROUP_ID, m_GID);
	DDX_Text(pDX, IDC_USER_ID, m_UID);
	//}}AFX_DATA_MAP
    }

BEGIN_MESSAGE_MAP(CFilePropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(CFilePropertiesDialog)
	ON_BN_CLICKED(IDVIEW, OnView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilePropertiesDialog message handlers

BOOL CFilePropertiesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    SetWindowText( m_FileType + " " + m_FileName + " Properties" );
    UpdateData( FALSE );
    
    if ( m_FileType != "Regular File" )
        m_ViewButton.ModifyStyle( 0, WS_DISABLED, 0 );

    if ( m_FileName == "/" ) // root directory: name cannot be changed
        m_FileNameCtrl.SetReadOnly ();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
    }

void CFilePropertiesDialog::OnOK() 
{
	CDialog::OnOK();
    }

void CFilePropertiesDialog::OnView() 
{
    EndDialog( 777 );
    }

void CFilePropertiesDialog::OnCancel() 
{
	CDialog::OnCancel();
    }
