// EricMinixDoc.h : interface of the CEricMinixDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERICMINIXDOC_H__B53462BF_ADF2_4C6A_A6D5_F7C479E2E3C3__INCLUDED_)
#define AFX_ERICMINIXDOC_H__B53462BF_ADF2_4C6A_A6D5_F7C479E2E3C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEricMinixDoc : public CDocument
{
protected: // create from serialization only
	CEricMinixDoc();
	DECLARE_DYNCREATE(CEricMinixDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEricMinixDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEricMinixDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEricMinixDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERICMINIXDOC_H__B53462BF_ADF2_4C6A_A6D5_F7C479E2E3C3__INCLUDED_)
