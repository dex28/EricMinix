// AddPartitionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "EricMinix.h"
#include "AddPartitionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddPartitionDialog dialog

CAddPartitionDialog::CAddPartitionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAddPartitionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddPartitionDialog)
	m_UseAllAvailableSpace = FALSE;
	m_FS_Size = _T("");
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    }

void CAddPartitionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddPartitionDialog)
	DDX_Control(pDX, IDC_FS_SIZE, m_FS_SizeCtrl);
	DDX_Text(pDX, IDC_FS_SIZE, m_FS_Size);
	DDX_Check(pDX, IDC_CHECK_USE_ALL_SPACE, m_UseAllAvailableSpace);
	//}}AFX_DATA_MAP
    }

BEGIN_MESSAGE_MAP(CAddPartitionDialog, CDialog)
	//{{AFX_MSG_MAP(CAddPartitionDialog)
	ON_BN_CLICKED(IDC_CHECK_USE_ALL_SPACE, OnCheckUseAllSpace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddPartitionDialog message handlers

BOOL CAddPartitionDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    if ( m_UseAllAvailableSpace )
    {
        m_FS_SizeCtrl.ModifyStyle( 0, WS_DISABLED, 0 );
        m_FS_SizeCtrl.SetReadOnly ();
        }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
    }

void CAddPartitionDialog::OnCheckUseAllSpace() 
{
    UpdateData( TRUE );

    if ( m_UseAllAvailableSpace )
    {
        m_FS_SizeCtrl.ModifyStyle( 0, WS_DISABLED, 0 );
        m_FS_SizeCtrl.SetReadOnly ();
        }
    else // remove read only style
    {
        m_FS_SizeCtrl.ModifyStyle( WS_DISABLED, 0, 0 );
        m_FS_SizeCtrl.SetReadOnly( FALSE );
        m_FS_SizeCtrl.SetSel( 0, -1 ); // select all text
        m_FS_SizeCtrl.SetFocus ();
        }

    UpdateData( FALSE );
    }
