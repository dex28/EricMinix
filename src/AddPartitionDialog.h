#if !defined(AFX_ADDPARTITIONDIALOG_H__24615B0D_BBD4_4054_8D1C_9F8FF2C5C15F__INCLUDED_)
#define AFX_ADDPARTITIONDIALOG_H__24615B0D_BBD4_4054_8D1C_9F8FF2C5C15F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddPartitionDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddPartitionDialog dialog

class CAddPartitionDialog : public CDialog
{
// Construction
public:
	CAddPartitionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddPartitionDialog)
	enum { IDD = IDD_ADD_PARTITION_DIALOG };
	CEdit	m_FS_SizeCtrl;
	BOOL	m_UseAllAvailableSpace;
	CString	m_FS_Size;
	//}}AFX_DATA

	HICON m_hIcon;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddPartitionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddPartitionDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckUseAllSpace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDPARTITIONDIALOG_H__24615B0D_BBD4_4054_8D1C_9F8FF2C5C15F__INCLUDED_)
