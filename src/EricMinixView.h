// EricMinixView.h : interface of the CEricMinixView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERICMINIXVIEW_H__E8EDA7B6_2C83_4F99_9AC8_A0EBA7EC0818__INCLUDED_)
#define AFX_ERICMINIXVIEW_H__E8EDA7B6_2C83_4F99_9AC8_A0EBA7EC0818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "minix_fs.h"


/////////////////////////////////////////////////////////////////////////////
// CFileListCtrl window

class CFileListCtrl : public CListCtrl
{
// Construction
public:
	CFileListCtrl();

// Attributes
public:

    class CEricMinixView* view;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileListCtrl)
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CFileListCtrl form view

class CEricMinixView : public CFormView
{
protected: // create from serialization only
	CEricMinixView();
	DECLARE_DYNCREATE(CEricMinixView)

public:
	//{{AFX_DATA(CEricMinixView)
	enum { IDD = IDD_ERICMINIX_FORM };
	CEdit	m_Report;
	CFileListCtrl	m_FileList;
	CString	m_strReport;
	//}}AFX_DATA

// Attributes
public:
	CEricMinixDoc* GetDocument();

    CFont font;
    CImageList m_FileListImageList;
    CImageList m_FileListImageListBig;
    bool reportIsVisible;

    bool isInitialized;

    COleDropTarget m_dropTarget;

// Operations
public:

    void ContextMenu( CPoint point );

    void DeleteFileList( void );

    void Directory_List( MINIX_FS* fs, const char* name, uint32 inode_index );

    void ShowReport( void )
    {
        reportIsVisible = true;

        if ( ::IsWindow( m_FileList.m_hWnd ) )
            m_FileList.ShowWindow( SW_HIDE );

        if ( ::IsWindow( m_Report.m_hWnd ) )
            m_Report.ShowWindow( SW_SHOW );
        }

    void ShowFileList( void )
    {
        reportIsVisible = false;

        if ( ::IsWindow( m_Report.m_hWnd ) )
            m_Report.ShowWindow( SW_HIDE );

        if ( ::IsWindow( m_FileList.m_hWnd ) )
            m_FileList.ShowWindow( SW_SHOW );
        }

    void ReportPrefix( const char* str )
    {
        if ( ::IsWindow( m_Report.m_hWnd ) )
            UpdateData( TRUE );

        m_strReport = CString( str ) + m_strReport;

        if ( ::IsWindow( m_Report.m_hWnd ) )
            UpdateData( FALSE );
        }

    void Report( const char* str = NULL )
    {
        if ( ::IsWindow( m_Report.m_hWnd ) )
            UpdateData( TRUE );

        if ( str == NULL )
            m_strReport = "";
        else
            m_strReport += str;

        if ( ::IsWindow( m_Report.m_hWnd ) )
            UpdateData( FALSE );
        }

    void ReportUpdateWindow( bool showBottom = false )
    {
        if ( ! ::IsWindow( m_Report.m_hWnd ) )
            return;

        if ( showBottom )
        {
            // select last position (to ensure it's visible
            m_Report.SetSel( m_strReport.GetLength (), m_strReport.GetLength () );
            }

        m_Report.UpdateWindow ();
        }

    // SetView - sets a list-view's window style to change the view. 
    // dwView - value specifying the new view style.
    //  
    VOID SetViewStyle( DWORD dwView ) 
    { 
        // Retrieve the current window style. 
        DWORD dwStyle = GetWindowLong( m_FileList, GWL_STYLE); 

        // Only set the window style if the view bits have changed. 
        if ((dwStyle & LVS_TYPEMASK) != dwView) 
            SetWindowLong( m_FileList, GWL_STYLE, 
                (dwStyle & ~LVS_TYPEMASK) | dwView); 
        } 

    // SetView - sets a list-view's window style to change the view. 
    // dwView - value specifying the new view style.
    //  
    DWORD GetViewStyle( void ) 
    { 
        return GetWindowLong( m_FileList, GWL_STYLE) & LVS_TYPEMASK;
        } 

    HGLOBAL CopyToClipboard( COleDataSource* pData );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEricMinixView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEricMinixView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEricMinixView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnColumnclickFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditLabel();
	afx_msg void OnFileCopyTo();
	afx_msg void OnFileDelete();
	afx_msg void OnFileProperties();
	afx_msg void OnFileRename();
	afx_msg void OnFileView();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EricMinixView.cpp
inline CEricMinixDoc* CEricMinixView::GetDocument()
   { return (CEricMinixDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERICMINIXVIEW_H__E8EDA7B6_2C83_4F99_9AC8_A0EBA7EC0818__INCLUDED_)
