// LeftView.h : interface of the CLeftView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEFTVIEW_H__AB0B31F1_C37A_459D_A77F_2DC7F01BB121__INCLUDED_)
#define AFX_LEFTVIEW_H__AB0B31F1_C37A_459D_A77F_2DC7F01BB121__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "minix_fs.h"

class CEricMinixDoc;

class CLeftView : public CTreeView
{
protected: // create from serialization only
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
	CEricMinixDoc* GetDocument();

    CImageList m_ImageList;

    class CEricMinixView* view;
    CTreeCtrl* tree;
    HANDLE hAutoscanThread;

    bool globalReadOnly;

    HGLOBAL CopyToClipboard( COleDataSource* pData );

    COleDropTarget m_dropTarget;

    DWORD lastDriveBitmap;

// Operations
public:

    void SysTree_List( MINIX_FS* fs, HTREEITEM hParent, const char* name = "/", uint32 inode_index = 1, int level = 0 );
    void Add_Disk( const char* shortName, bool isImage = false, const char* fullPath = NULL );
    void RescanDisks( DWORD driveBitmapADD, DWORD driveBitmapREMOVE );

    void ListHeaders( MINIX_FS* fs, const char* name = "/", uint32 inode_index = 1 );

    void ContextMenu( CPoint point );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLeftView)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditLabel();
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGlobalReadOnly();
	afx_msg void OnSysTreeRescan();
	afx_msg void OnStartAutoscan();
	afx_msg void OnSysTreeAddDiskImage();
	afx_msg void OnDiskReadOnly();
	afx_msg void OnDiskAddPartition();
	afx_msg void OnDiskCopyToImage();
	afx_msg void OnDiskHide();
	afx_msg void OnDiskEject();
	afx_msg void OnDiskRestoreImage();
	afx_msg void OnDirectoryCopyTo();
	afx_msg void OnDirectoryDelete();
	afx_msg void OnDirectoryMerge();
	afx_msg void OnDirectoryProperties();
	afx_msg void OnDirectoryRename();
	afx_msg void OnFilesystemCopyToImage();
	afx_msg void OnFilesystemDelete();
	afx_msg void OnFilesystemFormat();
	afx_msg void OnFilesystemFSCK();
	afx_msg void OnFilesystemMoveDown();
	afx_msg void OnFilesystemMoveUp();
	afx_msg void OnFilesystemResize();
	afx_msg void OnFilesystemRestoreImage();
	afx_msg void OnUpdateDirectoryCopyTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirectoryDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirectoryMerge(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirectoryProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDirectoryRename(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDiskAddPartition(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDiskCopyToImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDiskEject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDiskHide(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDiskRestoreImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilesystemCopyToImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilesystemDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilesystemFormat(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilesystemFSCK(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilesystemMoveDown(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilesystemMoveUp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilesystemResize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilesystemRestoreImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSysTreeAddDisk(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSysTreeRescanDisks(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDiskReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGlobalReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnCopyToImage();
	afx_msg void OnUpdateCopyToImage(CCmdUI* pCmdUI);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnFilesystemListPHeaders();
	afx_msg void OnUpdateFilesystemListPHeaders(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CEricMinixDoc* CLeftView::GetDocument()
   { return (CEricMinixDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTVIEW_H__AB0B31F1_C37A_459D_A77F_2DC7F01BB121__INCLUDED_)
