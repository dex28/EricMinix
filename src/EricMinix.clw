; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CListCtrl
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "EricMinix.h"
LastPage=0

ClassCount=10
Class1=CEricMinixApp
Class2=CEricMinixDoc
Class3=CEricMinixView
Class4=CMainFrame

ResourceCount=6
Resource1=IDD_FILE_PROPERTIES_DIALOG
Resource2=IDD_ERICMINIX_FORM
Class5=CLeftView
Class6=CAboutDlg
Class7=CSplashWnd
Resource3=IDD_ADD_PARTITION_DIALOG
Resource4=IDM_MNU_CONTEXT
Class8=CAddPartitionDialog
Resource5=IDD_ABOUTBOX
Class9=CFilePropertiesDialog
Class10=CFileListCtrl
Resource6=IDR_MAINFRAME

[CLS:CEricMinixApp]
Type=0
HeaderFile=EricMinix.h
ImplementationFile=EricMinix.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CEricMinixApp

[CLS:CEricMinixDoc]
Type=0
HeaderFile=EricMinixDoc.h
ImplementationFile=EricMinixDoc.cpp
Filter=N
LastObject=CEricMinixDoc
BaseClass=CDocument
VirtualFilter=DC

[CLS:CEricMinixView]
Type=0
HeaderFile=EricMinixView.h
ImplementationFile=EricMinixView.cpp
Filter=D
BaseClass=CFormView
VirtualFilter=VWC
LastObject=CEricMinixView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=ID_VIEW_LARGEICON



[CLS:CLeftView]
Type=0
HeaderFile=LeftView.h
ImplementationFile=LeftView.cpp
Filter=T
LastObject=ID_FILESYSTEM_LIST_P_HEADERS
BaseClass=CTreeView
VirtualFilter=VWC

[CLS:CAboutDlg]
Type=0
HeaderFile=EricMinix.cpp
ImplementationFile=EricMinix.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_SYSTREE_ADDDISK
Command2=ID_SYSTREE_RESCAN
Command3=ID_APP_EXIT
Command4=ID_DISK_READ_ONLY
Command5=ID_DISK_HIDE
Command6=ID_DISK_EJECT
Command7=ID_DISK_ADDPARTITION
Command8=ID_DISK_COPYTOIMAGE
Command9=ID_DISK_RESTOREIMAGE
Command10=ID_FILESYSTEM_FSCK
Command11=ID_FILESYSTEM_FORMAT
Command12=ID_FILESYSTEM_RESIZE
Command13=ID_FILESYSTEM_DELETE
Command14=ID_FILESYSTEM_MOVEUP
Command15=ID_FILESYSTEM_MOVEDOWN
Command16=ID_FILESYSTEM_COPYTOIMAGE
Command17=ID_FILESYSTEM_RESTOREIMAGE
Command18=ID_FILESYSTEM_LIST_P_HEADERS
Command19=ID_DIRECTORY_COPYTO
Command20=ID_DIRECTORY_MERGE
Command21=ID_DIRECTORY_RENAME
Command22=ID_DIRECTORY_DELETE
Command23=ID_DIRECTORY_PROPERTIES
Command24=ID_FILE_COPYTO
Command25=ID_FILE_VIEW
Command26=ID_FILE_RENAME
Command27=ID_FILE_DELETE
Command28=ID_FILE_PROPERTIES
Command29=ID_VIEW_TOOLBAR
Command30=ID_VIEW_STATUS_BAR
Command31=ID_VIEW_LARGEICON
Command32=ID_VIEW_SMALLICON
Command33=ID_VIEW_DETAILS
Command34=ID_APP_ABOUT
CommandCount=34

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_EDIT_LABEL
Command6=ID_NEXT_PANE
Command7=ID_PREV_PANE
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_EDIT_CUT
Command11=ID_EDIT_UNDO
CommandCount=11

[DLG:IDD_ERICMINIX_FORM]
Type=1
Class=CEricMinixView
ControlCount=2
Control1=IDC_FILE_LIST,SysListView32,1342243329
Control2=IDC_REPORT,edit,1345390788

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_SYSTREE_RESCAN
Command2=ID_SYSTREE_ADDDISK
Command3=ID_DISK_EJECT
Command4=ID_DISK_ADDPARTITION
Command5=ID_COPY_TO_IMAGE
Command6=ID_APP_ABOUT
CommandCount=6

[DLG:IDR_MAINFRAME]
Type=1
Class=?
ControlCount=1
Control1=IDC_CHECK_READ_ONLY,button,1342242819

[MNU:IDM_MNU_CONTEXT]
Type=1
Class=?
Command1=ID_SYSTREE_ADDDISK
Command2=ID_SYSTREE_RESCAN
Command3=ID_DISK_READ_ONLY
Command4=ID_DISK_HIDE
Command5=ID_DISK_EJECT
Command6=ID_DISK_ADDPARTITION
Command7=ID_DISK_COPYTOIMAGE
Command8=ID_DISK_RESTOREIMAGE
Command9=ID_FILESYSTEM_FSCK
Command10=ID_FILESYSTEM_FORMAT
Command11=ID_FILESYSTEM_RESIZE
Command12=ID_FILESYSTEM_DELETE
Command13=ID_FILESYSTEM_MOVEUP
Command14=ID_FILESYSTEM_MOVEDOWN
Command15=ID_FILESYSTEM_COPYTOIMAGE
Command16=ID_FILESYSTEM_RESTOREIMAGE
Command17=ID_FILESYSTEM_LIST_P_HEADERS
Command18=ID_DIRECTORY_COPYTO
Command19=ID_DIRECTORY_MERGE
Command20=ID_DIRECTORY_RENAME
Command21=ID_DIRECTORY_DELETE
Command22=ID_DIRECTORY_PROPERTIES
Command23=ID_FILE_COPYTO
Command24=ID_FILE_VIEW
Command25=ID_FILE_RENAME
Command26=ID_FILE_DELETE
Command27=ID_FILE_PROPERTIES
CommandCount=27

[CLS:CSplashWnd]
Type=0
HeaderFile=Splash.h
ImplementationFile=Splash.cpp
BaseClass=CWnd
LastObject=ID_DIRECTORY_DELETE

[DLG:IDD_ADD_PARTITION_DIALOG]
Type=1
Class=CAddPartitionDialog
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,button,1342177287
Control3=IDC_FS_SIZE,edit,1350631552
Control4=IDC_CHECK_USE_ALL_SPACE,button,1342259203
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816

[CLS:CAddPartitionDialog]
Type=0
HeaderFile=AddPartitionDialog.h
ImplementationFile=AddPartitionDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CHECK_USE_ALL_SPACE
VirtualFilter=dWC

[DLG:IDD_FILE_PROPERTIES_DIALOG]
Type=1
Class=CFilePropertiesDialog
ControlCount=37
Control1=IDC_FILE_TYPE,static,1342308352
Control2=IDC_FILE_NAME,edit,1350631552
Control3=IDC_STATIC,static,1342177298
Control4=IDC_STATIC,static,1342308352
Control5=IDC_FILE_SIZE,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_SIZE_ON_DISK,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_INODE_INDEX,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_NLINKS,static,1342308352
Control12=IDC_STATIC,static,1342177296
Control13=IDC_STATIC,static,1342308352
Control14=IDC_ACCESS_TIME,edit,1350631552
Control15=IDC_STATIC,static,1342177296
Control16=IDC_STATIC,static,1342308352
Control17=IDC_CHECK_USER_R,button,1342242819
Control18=IDC_CHECK_USER_W,button,1342242819
Control19=IDC_CHECK_USER_X,button,1342242819
Control20=IDC_STATIC,static,1342308352
Control21=IDC_CHECK_GROUP_R,button,1342242819
Control22=IDC_CHECK_GROUP_W,button,1342242819
Control23=IDC_CHECK_GROUP_X,button,1342242819
Control24=IDC_STATIC,static,1342308352
Control25=IDC_CHECK_OTHER_R,button,1342242819
Control26=IDC_CHECK_OTHER_W,button,1342242819
Control27=IDC_CHECK_OTHER_X,button,1342242819
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342177296
Control30=IDC_STATIC,static,1342308352
Control31=IDC_USER_ID,edit,1350631552
Control32=IDC_STATIC,static,1342308352
Control33=IDC_GROUP_ID,edit,1350631552
Control34=IDC_STATIC,static,1342308352
Control35=IDOK,button,1342242817
Control36=IDCANCEL,button,1342242816
Control37=IDVIEW,button,1342242816

[CLS:CFilePropertiesDialog]
Type=0
HeaderFile=FilePropertiesDialog.h
ImplementationFile=FilePropertiesDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CFilePropertiesDialog
VirtualFilter=dWC

[CLS:CFileListCtrl]
Type=0
HeaderFile=ericminixview.h
ImplementationFile=ericminixview.cpp
BaseClass=CListCtrl
Filter=W
VirtualFilter=FWC
LastObject=CFileListCtrl

