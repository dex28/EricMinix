// EricMinixDoc.cpp : implementation of the CEricMinixDoc class
//

#include "stdafx.h"
#include "EricMinix.h"

#include "EricMinixDoc.h"
#include "EricMinixView.h"
#include "LeftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEricMinixDoc

IMPLEMENT_DYNCREATE(CEricMinixDoc, CDocument)

BEGIN_MESSAGE_MAP(CEricMinixDoc, CDocument)
	//{{AFX_MSG_MAP(CEricMinixDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEricMinixDoc construction/destruction

CEricMinixDoc::CEricMinixDoc()
{
	// TODO: add one-time construction code here

}

CEricMinixDoc::~CEricMinixDoc()
{
}

BOOL CEricMinixDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	return TRUE;
}

BOOL CEricMinixDoc::OnNewDocument()
{
	if ( ! CDocument::OnNewDocument() )
		return FALSE;

	// (SDI documents will reuse this document)

    this->SetTitle( "Eric-MINIX" );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CEricMinixDoc serialization

void CEricMinixDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// add storing code here
	}
	else
	{
		// add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEricMinixDoc diagnostics

#ifdef _DEBUG
void CEricMinixDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEricMinixDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEricMinixDoc commands


void CEricMinixDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	CDocument::SetPathName(lpszPathName, bAddToMRU);

    CLeftView* view = GetLeftView ();
    if ( view )
    {
        Report( NULL );
        view->Add_Disk( GetTitle (), true, lpszPathName );
        }
    }
