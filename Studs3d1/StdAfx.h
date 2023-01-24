#ifndef __STDAFX_H
#define __STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


//#ifndef _AFX_NO_DB_SUPPORT
//#include <afxdb.h>          // MFC ODBC database classes
//#endif // _AFX_NO_DB_SUPPORT


//#ifndef _AFX_NO_DAO_SUPPORT
//#include <afxdao.h>         // MFC DAO database classes
//#endif // _AFX_NO_DAO_SUPPORT


//#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>         // MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT

#ifndef __LIBTOOL_H
#include <libtool.h>
#endif 

#ifndef __LHEAD3D_H
#include <LHead3d.h>
#endif

#ifndef __LIBDB_H
#include <libdb.h>
#endif

#ifndef  _INC_MATH
#include "math.h"
#endif  

#pragma warning( disable : 4192 )
#pragma warning( disable : 4278 )

#import <ksConstants.tlb>   no_namespace named_guids
#import <ksConstants3D.tlb> no_namespace named_guids
#import <kAPI2D5COM.tlb>    no_namespace named_guids
#import <kAPI3D5COM.tlb>    no_namespace named_guids 
#import <kAPI7.tlb>         rename( "KompasAPI7", "ksAPI7" ) named_guids

#pragma warning( once : 4192 ) 
#pragma warning( once : 4278 )



//{{AFX_INSERT_LOCATION}}
#endif // !defined(__STDAFX_H)
