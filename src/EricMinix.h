// EricMinix.h : main header file for the ERICMINIX application
//

#if !defined(AFX_ERICMINIX_H__FE61F1DE_42BE_4237_A385_8470F78CFC36__INCLUDED_)
#define AFX_ERICMINIX_H__FE61F1DE_42BE_4237_A385_8470F78CFC36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CEricMinixApp:
// See EricMinix.cpp for the implementation of this class
//

class CEricMinixApp : public CWinApp
{
    HANDLE hExclusionMap;
    
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEricMinixApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEricMinixApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CEricMinixApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

extern class CLeftView* GetLeftView( void );

extern class CEricMinixView* GetMainView( void );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERICMINIX_H__FE61F1DE_42BE_4237_A385_8470F78CFC36__INCLUDED_)
