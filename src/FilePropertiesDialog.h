#if !defined(AFX_FILEPROPERTIESDIALOG_H__6489E086_3BCC_4BA4_8D94_43B2522B62D9__INCLUDED_)
#define AFX_FILEPROPERTIESDIALOG_H__6489E086_3BCC_4BA4_8D94_43B2522B62D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilePropertiesDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilePropertiesDialog dialog

class CFilePropertiesDialog : public CDialog
{
// Construction
public:
	CFilePropertiesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilePropertiesDialog)
	enum { IDD = IDD_FILE_PROPERTIES_DIALOG };
	CEdit	m_FileNameCtrl;
	CButton	m_ViewButton;
	CString	m_LastAccessTime;
	BOOL	m_CheckGroupRead;
	BOOL	m_CheckGroupWrite;
	BOOL	m_CheckGroupExecute;
	BOOL	m_CheckOtherRead;
	BOOL	m_CheckOtherWrite;
	BOOL	m_CheckOtherExecute;
	BOOL	m_CheckUserRead;
	BOOL	m_CheckUserWrite;
	BOOL	m_CheckUserExecute;
	CString	m_FileName;
	CString	m_FileSize;
	CString	m_FileType;
	CString	m_InodeIndex;
	CString	m_NLinks;
	CString	m_SizeOnDisk;
	CString	m_GID;
	CString	m_UID;
	//}}AFX_DATA

	HICON m_hIcon;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilePropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilePropertiesDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnView();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEPROPERTIESDIALOG_H__6489E086_3BCC_4BA4_8D94_43B2522B62D9__INCLUDED_)
