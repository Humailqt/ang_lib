////////////////////////////////////////////////////////////////////////////////
//
// studs3d.cpp - ��������� ������� �� Visual C++ - �������
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxdllx.h>
#include <string>
#include "resource.h"
#include "ResourceDir.h"
//#include "panel.h"
#include <iostream>

#ifndef  __STUDS3D_H
#include "studs3d.h"
#endif   
#include <SafeArrayUtils.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

      
//-------------------------------------------------------------------------------
// 
// ---
AFX_EXTENSION_MODULE StepDLL = { NULL, NULL };


//------------------------------------------------------------------------------
//
// ---
ksAPI7::IApplicationPtr pNewKompasAPI( NULL );



//----------------------------------------------------------------------------------------------
// ��������������� �������, ������� �������� � ������
// ---
LPCTSTR DoubleToStr( double value ) 
{
  static TCHAR res[20];
  _stprintf_s( res, _T("%g"), value );
  return res;
}


//------------------------------------------------------------------------------
// ��������������� �������, ��������� ������ �� �������
// ---
LPTSTR LoadStr( int id ) 
{ 
  static TCHAR buf[512]; 
  // �������������� ������ � ������������ � ������� ��������
  ksConvertLangStrExT( StepDLL.hModule, id, buf, 512 );
  return buf;
}


//------------------------------------------------------------------------------
// ��������������� �������, ��������� ������ �� �������
// ---
LPTSTR _LoadStr( int id ) 
{ 
  static TCHAR buf[512]; 
  // �������������� ������ � ������������ � ������� ��������
  LoadString( StepDLL.hModule, id, buf, 512 );
  return buf;
}


//------------------------------------------------------------------------------
// ��������������� �������, ����� ���������
// ---
int LibMessage( LPCTSTR str, int flags = 0 ) 
{
  int res = 0;

  if ( str && str[0] )                  // ������ ��������
  {
    int enabse = IsEnableTaskAccess();  // �������� �������
    if ( enabse )                       // ���� ������ � ������ ��������
      EnableTaskAccess(0);              // ��������� ������

    // ����� ���������  ��������� ���������
    res = MessageBox( (HWND) GetHWindow(), str, LoadStr(IDR_LIBID), flags );

    if ( enabse )                       // ���� ������ � ������ ��� ��������
      EnableTaskAccess(1);              // ��������� ������ � ������
  }
  return res;
}


//------------------------------------------------------------------------------
// ��������������� �������, ����� ���������
// ---
int LibMessage( int strId, int flags ) 
{
  return LibMessage( LoadStr(strId), flags );
}


//------------------------------------------------------------------------------
// ��������������� �������, ������������ ���������� �� ������
// ---
void DumpError(_com_error& e)
{
  _bstr_t bstrSource(e.Source());
  _bstr_t bstrDescription(e.Description());
  CString str;
  str.Format( _T("����� ������= %08lx"),   e.Error());
  str += _T("\n���������:");       
  str += e.ErrorMessage();
  str += _T("\n��������:");        
  str += (LPCTSTR)bstrSource;
  str += _T("\n�������� ������:"); 
  str += (LPCTSTR)bstrDescription;
  LibMessage( str, MB_OK | MB_ICONERROR);	
}


//-------------------------------------------------------------------------------
// ��������������� �������, �������� ������ � ������ API
// ---
void GetNewKompasAPI() 
{
  if ( !(ksAPI7::IApplication* )pNewKompasAPI )
  {
    CString filename;
    if( GetModuleFileName(NULL, filename.GetBuffer(255), 255) ) 
    {
      filename.ReleaseBuffer( 255 );
      CString libname;

      #ifdef __LIGHT_VERSION__
        libname = _LoadStr( IDS_API7LT ); // klAPI7.dll
      #else
        libname = _LoadStr( IDS_API7 );   // kAPI7.dll
      #endif

      filename.Replace( filename.Right(filename.GetLength() - (filename.ReverseFind(_T('\\')) + 1)), libname );

      HINSTANCE hAppAuto = LoadLibrary( filename ); // ������������� kAPI7.dll
      if(  hAppAuto ) 
      {
        // ��������� �� ������� ������������ ��������� KompasApplication  
        typedef LPDISPATCH ( WINAPI *FCreateKompasApplication )(); 
				 
        FCreateKompasApplication pCreateKompasApplication = 
          (FCreateKompasApplication)GetProcAddress( hAppAuto, "CreateKompasApplication" );	
        if ( pCreateKompasApplication )
          pNewKompasAPI = IDispatchPtr( pCreateKompasApplication(), false/*AddRef*/ ); // �������� ��������� Application
        FreeLibrary( hAppAuto );  
      }
    }
  }
}

//-------------------------------------------------------------------------------
// ����������� ����� �����
// ������������� � ���������� DLL
// ---
extern "C" int APIENTRY
DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
{
  UNREFERENCED_PARAMETER( lpReserved );

  if ( dwReason == DLL_PROCESS_ATTACH )
  {
    TRACE0( "DLL Initializing!" );
 
    if ( !AfxInitExtensionModule( StepDLL, hInstance ) )
      return 0;

    new CDynLinkLibrary( StepDLL );
  }
  else if ( dwReason == DLL_PROCESS_DETACH )
  {

    BaseEvent::TerminateEvents();
    BaseEvent::DestroyList(); // ������� ������ �����������
    if ( pNewKompasAPI )
      pNewKompasAPI = NULL;    // ���������� ������

    TRACE0( "DLL Terminating!" );
    AfxTermExtensionModule( StepDLL );
  }
  return 1;
}


//-------------------------------------------------------------------------------
// ������ ������������� ��������
// ---
unsigned int WINAPI LIBRARYID()
{
  return IDR_LIBID;
}


//-------------------------------------------------------------------------------
// �������� ������� ����������
// ---
void WINAPI LIBRARYENTRY( unsigned int ) 
{
  GetNewKompasAPI();



  IDocument3DPtr pDocument3d( ksGetActive3dDocument(), false/*AddRef*/ ); // �������� ��������� �� �������� �������� ���������� ������
  if ( (bool)pDocument3d && !pDocument3d->IsDetail() )
  {
      try
      {
        Shpeel* shpeel = new Shpeel();
	    if ( shpeel ) 
        {
	      shpeel->Draw3D();
	    }

      }
      catch (const std::exception&e)
      {
          LibMessage((CString(e.what())),0);
      }
//     pDocument3d->RebuildDocument();
  }
  else
  {
    // �������� �� ������������� ��� �� �������� 3D-�������
    ErrorT( LoadStr( IDS_3DDOCERROR ) ); // ������ ��������� � ������
  }

  if ( ReturnResult() == etError10 ) // ����������� ������
	ResultNULL();
}


////////////////////////////////////////////////////////////////////////////////
//
// ���������� �������
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// ���������� ����� �������� �� �����
// ---
static short GetClassStrForGost( short gost ) 
{
  short n = 0; 
  switch ( gost ) 
  {
    case 22032 :
    case 22034 :
    case 22036 :
    case 22038 :
    case 22040 :
    case 22042 : n = IDS_STUD_B; break; // B 
    case 22033 :
    case 22035 :
    case 22037 :
    case 22039 :
    case 22041 :
    case 22043 : n = IDS_STUD_A; break; // A
  }
  return n;
}


//------------------------------------------------------------------------------
// ���������� ����� �������� �� �����
// ---
short GetGostForTypeAndKlass( short type, short klass ) 
{
  short n = 0;
  switch ( type ) 
  {
    case 0 : n = klass ? 22032 : 22033; break; // 1d    
    case 1 : n = klass ? 22034 : 22035; break; // 1,25 d         
    case 2 : n = klass ? 22036 : 22037; break; // 1,6 d
    case 3 : n = klass ? 22038 : 22039; break; // 2d         
    case 4 : n = klass ? 22040 : 22041; break; // 2,5 d 
    case 5 : n = klass ? 22042 : 22043; break; // b0         
  }                                                
  return n;                                             
}                                               
                                                        
                                                   
//------------------------------------------------------------------------------
// ���������� ����� ������������� ����� �� �����
// ---
static short GetTypeStrForGost( short gost ) 
{
  short n = 0; 
  switch ( gost ) 
  {
    case 22032 :
    case 22033 : n = IDS_STUD_1D;   break; // 1d               
    case 22034 :                                           
    case 22035 : n = IDS_STUD_125D; break; // 1,25 d                       
    case 22036 :                                    
    case 22037 : n = IDS_STUD_16D;  break; // 1,6 d      
    case 22038 :                                    
    case 22039 : n = IDS_STUD_2D;   break; // 2d    
    case 22040 :                                    
    case 22041 : n = IDS_STUD_25D;  break; // 2,5 d 
    case 22042 :                                    
    case 22043 : n = IDS_STUD_B0;   break; // b0    
  }
  return n;
}


//------------------------------------------------------------------------------
// ������ �� ����� (����� *.loa)
// ---
static void GetGostLoa( short gost, short *gNumb, short *stNumb ) 
{
  switch ( gost ) 
  {
    case 22032 : *gNumb = STR_200; *stNumb = STR_220; break;
    case 22033 : *gNumb = STR_201; *stNumb = STR_221; break;
    case 22034 : *gNumb = STR_202; *stNumb = STR_222; break;
    case 22035 : *gNumb = STR_203; *stNumb = STR_223; break;
    case 22036 : *gNumb = STR_204; *stNumb = STR_224; break;
    case 22037 : *gNumb = STR_205; *stNumb = STR_225; break;
    case 22038 : *gNumb = STR_206; *stNumb = STR_226; break;
    case 22039 : *gNumb = STR_207; *stNumb = STR_227; break;
    case 22040 : *gNumb = STR_208; *stNumb = STR_228; break;
    case 22041 : *gNumb = STR_209; *stNumb = STR_229; break;
    case 22042 : *gNumb = STR_210; *stNumb = STR_230; break;
    case 22043 : *gNumb = STR_211; *stNumb = STR_231; break;
    default    : *gNumb = 0; *stNumb = 0;
  }
}


//------------------------------------------------------------------------------
// ������ �� ����� (��������)
// ---
static short GetGostStr( short gost ) 
{
  short n = 0;
  switch ( gost ) 
  {
    case 22032 : n = STR_101; break;
    case 22033 : n = STR_102; break;
    case 22034 : n = STR_103; break;
    case 22035 : n = STR_104; break;
    case 22036 : n = STR_105; break;
    case 22037 : n = STR_106; break;
    case 22038 : n = STR_107; break;
    case 22039 : n = STR_108; break;
    case 22040 : n = STR_109; break;
    case 22041 : n = STR_110; break;
    case 22042 : n = STR_111; break;
    case 22043 : n = STR_112; break;
  }              
  return n;
}


//-------------------------------------------------------------------------------
// ����������� � ����
// ---
int _ConnectDB( reference bd, LPCTSTR name ) 
{
  static TCHAR fullName[512];
  bool res = false;
  if( GetModuleFileName( StepDLL.hModule, fullName, 512 ) ) 
  {
    LPTSTR c = _tcsrchr( fullName, _T('\\'));
    if ( c ) 
    {
      *(c + 1) = _T('\0');
      _tcscat_s( fullName, 255, name );
      OFSTRUCT ofs;
      if ( OpenFile(_bstr_t(fullName), &ofs, OF_EXIST) != HFILE_ERROR )
        res = true;
      else 
      {
        *(c + 1) = _T('\0');
        _tcscat_s( fullName, 255, _T("load\\") );
        _tcscat_s( fullName, 255, name );
        if ( OpenFile(_bstr_t(fullName), &ofs, OF_EXIST) != HFILE_ERROR )
          res = true;
      }
    }
  }
  return res ? ConnectDBT( bd, fullName ) : ConnectDBT( bd, (LPTSTR)name );
}


//------------------------------------------------------------------------------
// ����������� ���������� � ������ varName �������� val
// ---
void SetVarValue( IVariableCollectionPtr& varArr, TCHAR* varName, double val ) 
{
	IVariablePtr var( varArr->GetByName( _bstr_t(varName), true, false), false/*AddRef*/ ); // ������� ����������
  if ( var )
    var->SetValue( val ); // ������� ��������
}  


//------------------------------------------------------------------------------
// ���������� �������� �� ������ ������������ ( planar = true - ����������,
// planar = false - ���������� ������������ )
// ---
bool IsSurface( IEntityPtr entity, bool planar = true, double * = NULL ) 
{
	bool res = false;
// �� K8+ ����� ������� �������� ����� �� ������, ��� ������� ����� ������� � ��� ����� �������� ������� �� � �����
  if ( (bool)entity && entity->IsCreated() ) 
  {
    IFaceDefinitionPtr faceDef( IUnknownPtr( entity->GetDefinition(), false/*AddRef*/ ) ); 
    if ( faceDef ) 
    {
      if ( planar && faceDef->IsPlanar() )
        res = true;
      if ( !planar && faceDef->IsCylinder() )
        res = true;
    }
  }
  return res;
} 


//------------------------------------------------------------------------------
// ���������� �������� �� ������ ����
// ---
bool IsAxis( IEntityPtr entity ) 
{
  if ( entity ) 
  {
    short  type = entity->GetType();
    return type == o3d_axis2Planes              || // ��� �� ���� ����������
           type == o3d_axisOperation            || // ��� ��������
           type == o3d_axis2Points              || // ��� �� ���� ������
           type == o3d_axisConeFace             || // ��� ���������� �����������
           type == o3d_axisEdge                 || // ��� �� �����
           type == o3d_AxisFromPointByDirection || // ��� ����� ����� �� �����������
           type == o3d_axisOX                   || // ��� X
           type == o3d_axisOY                   || // ��� Y
           type == o3d_axisOZ                   ;  // ��� Z
  }
  return false;
}


//------------------------------------------------------------------------------
// ������ ��
// ---
int ReadShpeelBase( float d, ShpeelBase &base, SHPEEL * shpeel ) 
{
  TCHAR buf[256];
  ShpeelTmp shpeelTmp; 
  memset( &shpeelTmp, 0, sizeof(shpeelTmp) );

  _stprintf_s( buf, _T("t = 1 && d=%g"), d );
  if ( !ConditionT(base.bg,  base.rg2, buf) ) 
    return 0;

  int res = ReadRecord( base.bg, base.rg2, &shpeelTmp );
  if( res ) 
  {
    shpeel->d  = shpeelTmp.d;  // ������� ������
    shpeel->p1 = shpeelTmp.p1; // ��� ������ 
    shpeel->p2 = shpeelTmp.p2; // ��� ������ 
    shpeel->b1 = shpeelTmp.b1; // ������������ �����
    shpeel->c  = shpeelTmp.c;  // ������ ����� 
  }

  return res;
}


//------------------------------------------------------------------------------
// ������ ��������� �������
// ���������� 1 - ����� 0 - �� ������� ������ -1 - ������ ����� � ��
// ---
int ReadShpeelStBase( ShpeelBase &base, SHPEEL * shpeel )
{
  TCHAR buf[256];
  ShpeelSTmp shpeelSTmp;
  memset( &shpeelSTmp, 0, sizeof(shpeelSTmp) );

  _stprintf_s( buf, _T("t = 1 && L=%g && d=%g"), shpeel->l, shpeel->d );
  if ( !ConditionT(base.bs,  base.rs2, buf) ) 
    return -1;
  
  int i = ReadRecord(  base.bs, base.rs2, &shpeelSTmp );
  if ( !i )  
  {
    _stprintf_s( buf, _T("t = 1 && d=%.1f"), shpeel->d);

    if ( !::ConditionT(base.bs,  base.rs2, buf) ) 
      return -1;

    ReadRecord( base.bs, base.rs2, &shpeelSTmp );
  }

  shpeel->l  = shpeelSTmp.l;
  shpeel->b  = shpeelSTmp.b;
  shpeel->m1 = shpeelSTmp.m1;
  shpeel->m2 = shpeelSTmp.m2;

  // ALLST - ������ �� �������
  if( shpeel->gost > 22041 && !(fabs(shpeel->b) > EPSILON) ) 
  {
    shpeel->f |= ALLST;
    shpeel->f &= ~ISPOLN; // ���������� 1 ���������� �������, ��. ���������� 2 ����������
  }
  else
    shpeel->f &=~ ALLST;
  
  return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
// ������ � ��
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// �������� ��
// ---
int Shpeel::OpenBase() 
{
  bBase.bg = CreateDBT( _T("TXT_DB") );
  bBase.bs = CreateDBT( _T("TXT_DB") );

  short gNumb, stNumb;
  GetGostLoa( tmp.gost, &gNumb, &stNumb );
  if( !_ConnectDB( bBase.bg, LoadStr( gNumb ) ) ) // 22032.loa ..
    return 0;
  else 
  {
    if( !_ConnectDB( bBase.bs, LoadStr( stNumb ) ) )  // st22032.loa ..
      return 0;
    else 
    {
      bBase.rg1 = Relation( bBase.bg );
        RIntT  (_T("t"));
        RFloatT(_T("d"));
      EndRelation();
      if ( !DoStatementT(bBase.bg, bBase.rg1, _T("1 2") ) ) 
        return 0;
      else 
      {
        bBase.rg2 = Relation( bBase.bg );
          RIntT  (_T("t"));
          RFloatT(_T("d"));
          RFloatT(_T(""));
          RFloatT(_T(""));
          RFloatT(_T(""));
          RFloatT(_T(""));
        EndRelation();
        if ( !DoStatementT( bBase.bg, bBase.rg2, _T("")) )
          return 0;
        else 
        {
          bBase.rs1 = Relation( bBase.bs );
            RIntT ( _T("t") );
            RFloatT( _T("L") );
            RFloatT( _T("d") );
          EndRelation();
          if ( !DoStatementT(bBase.bs, bBase.rs1, _T("1 2 3") ) )
            return 0;
          else 
          {
            bBase.rs2 = Relation( bBase.bs );
              RIntT (_T("t"));
              RFloatT(_T("L"));
              RFloatT(_T("d"));
              RFloatT(_T(""));
              RFloatT(_T(""));
              RFloatT(_T(""));
            EndRelation();
            if ( !DoStatementT( bBase.bs, bBase.rs2, _T("")) )
              return 0;
            else 
            {
              openBase = true;
              return 1;
            }
          }
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
// �������� ��
// ---
void Shpeel::CloseBase()
{
  if ( bBase.bg )
    DeleteDB( bBase.bg );
  if ( bBase.bs )
    DeleteDB( bBase.bs );
  openBase = false;
}


//------------------------------------------------------------------------------
// �������� ���������
// ---
int Shpeel::GetParam() 
{
  return m_part ? m_part->GetUserParam( &par, Size() ) : 0;
}


//------------------------------------------------------------------------------
// �����������
// ---
Shpeel::Shpeel() 
 : refCount      ( 0     )
 , m_part          ( NULL  )
// , collect       ( NULL  )
 , collectChanged( false )
 , changed       ( true  )
 , openBase      ( false )
 , flagMode      ( false )
{
  par.flagAttr    = 1;
  par.typeSwitch  = 0;
  par.ang         = 0;
  par.drawType    = 0;

  doc.Attach(ksGetActive3dDocument() );
  if ( doc )
    flagMode = doc->IsEditMode() ;
  //callbackCurrentShpeel = this;

  // ����� ������
  m_part = IPartPtr( doc->GetPart( flagMode ? pEdit_Part : pNew_Part ), false/*AddRef*/ );

  if( !GetParam() ) 
  {

    Init();

  }
  else 
  {
    if ( !tmp.ver )
      tmp.ver = 1;
  }
}


//------------------------------------------------------------------------------
// ����������� �����
// ---
Shpeel::~Shpeel()
{
  ASSERT(!refCount);
  if ( openBase ) // ���� �� �������
    CloseBase();  // ��������� ������ ��
}


//------------------------------------------------------------------------------
// �������������
// ---
void Shpeel::Init() 
{

  tmp.gost       = 22032;
  par.drawType   = ID_VID;
  par.ang        = 0;
  tmp.f          = 0;
  tmp.f         |= TAKEISPOLN;
  tmp.d          = 20;
  tmp.p1         = 2.5;
  tmp.p2         = 1.5;
  tmp.c          = 2.5;
  tmp.indexMassa = 0; // 0 - ������ 1 - ������ ����� 3 - ������ 2 - ������
  tmp.ver        = 1;
  tmp.b1         = 20;
  tmp.klass      = 2; // klass=B
  tmp.l          = 90;
  tmp.b          = 46;
  tmp.m1         = 245.9f;
  tmp.m2         = 228.9f;

}


//------------------------------------------------------------------------------
// ��� ������� �� ���������� ������� ����������� ������ ���� � ����� ���������� � ���� ������ ���������� �������
// ---
CString& Shpeel::GetFileName() 
{        
  static TCHAR fullName[512];
  bool res = false;
  if( GetModuleFileName( StepDLL.hModule, fullName, sizeof(fullName) ) ) //��������� ���� stud.l3d
  {

    TCHAR *c = _tcsrchr( fullName, _T('\\'));
    if ( c ) 
    {
      *(c + 1) = _T('\0');
      _tcscat_s( fullName, 255, STUDS_FILE );
      OFSTRUCT ofs;
      if ( OpenFile(_bstr_t(fullName), &ofs, OF_EXIST) != HFILE_ERROR )
        res = true;
      else 
      {
        *(c + 1) = _T('\0');
        _tcscat_s( fullName, 255, _T("model\\") );
        _tcscat_s( fullName, 255, STUDS_FILE );
        if ( OpenFile(_bstr_t(fullName), &ofs, OF_EXIST) != HFILE_ERROR )
          res = true;
      }
    }
  }

  if( res )
  {
    _tcscat_s( fullName, 255, STUDS_INPUT_PATH );
    fileName = fullName;
  }
  
  return fileName;
}

#include "draw_obj.h"
//------------------------------------------------------------------------------
// ����������� ����������
// ---
void Shpeel::SetParam(IPartPtr& pPart )
{
  // ����������� ������� ����������
    if (!(bool)pPart)
    {
        //pPart = m_part;
        //draw_panel(NULL, pPart);
        pPart->SetFileName(L"D:/ang_lib/ang_detile/d.m3d");
        pPart->Update();

    }

  if (pPart)
  {
      draw_panel(NULL, pPart);
  }

}


//------------------------------------------------------------------------------
// �������� ����������
// ---
bool Shpeel::AddMate( short constraintType, 
                      short direction, 
                      short fixed, 
                      double val,
                      IEntity* ent, 
                      LPCTSTR surfaceName, 
                      IPart* p,
                      IMateConstraintCollection* mCol ) 
{
  bool res = false;

  if (p && mCol) // ��������� ����������
  { 
    IMateConstraintPtr mate=doc->GetMateConstraint();

    if ( mate ) 
    {

      mate->SetConstraintType( constraintType );
      mate->SetDirection( direction );
      mate->SetFixed(2 );
      mate->SetBaseObj(1, ent );
      // 
      mate->SetBaseObj(2, GetEntityByName(surfaceName, IPartPtr(p) ) );


      if (mate->Create()) 
      {
        mCol->AddMateConstraint( mate );
        res = true;


      }
    }
  }
  return res;
}


template <class T>
void FillDispArray( const _variant_t & var, PArray<T> & dispArr )
{
  if ( var.vt == VT_DISPATCH )
  {
    dispArr.Add( new T(var.pdispVal) ); 
  }
  else
  {
    if ( var.vt == (VT_ARRAY | VT_DISPATCH) )
    {
      LPDISPATCH HUGEP * pvar = NULL;
      ::SafeArrayAccessData( var.parray, (void HUGEP* FAR*)&pvar );
      if ( pvar ) {
        int count = var.parray->rgsabound[0].cElements;

        // ��������� ����������� � ���������
        for ( long i = 0; i < count; i++ ) 
        {
          dispArr.Add( new T(pvar[i]) );
        }
        ::SafeArrayUnaccessData( var.parray );
      }
    }
  }
}


//------------------------------------------------------------------------------
// �������������� ������� ������������
// ---
reference Shpeel::EditSpcObj( reference spcObj )
{
  //ISpecification3DPtr specification( doc->GetSpecification(), false/*AddRef*/ );
  //if ( (!flagMode && !par.flagAttr) || !(bool)specification )
  //  return 0;

  ksAPI7::ISpecificationBaseObjectPtr base;
  ksAPI7::IKompasDocumentPtr kompasDoc = pNewKompasAPI->ActiveDocument;

  ksAPI7::IKompasDocument1Ptr kompasDoc1( kompasDoc );
  ksAPI7::ISpecificationObjectCreateParamPtr ObjectCreateParam( kompasDoc1->GetInterface( ksSpecificationObjectCreateParam ) );
  
  
  ////////////////////////////////////////

  CString patch;
  patch = get_value_from_list(this, ID_CHOSE_DETAIL);
  auto& part = GetPart();
  part->ClearAllObj();
  part->SetFileName((LPWSTR)(LPCTSTR)patch);
  part->Update();
  RedrawPhantom();



  ////////////////////////////////////////
  

  ksAPI7::IPart7Ptr part7 = IUnknownPtr( ksTransferInterface( m_part, ksAPI7Dual, 0 ), false );
  ksAPI7::IPropertyKeeperPtr propertyKeeperIn1( part7 );
 
  ObjectCreateParam->Section = STANDART_SECTION;
  ObjectCreateParam->AttributeNumber = 81764182256.0;

  if ( m_spcBaseObjects == NULL ) 
  {
    ksAPI7::ISpecificationDescriptionsPtr descriptions = kompasDoc->SpecificationDescriptions;
    ksAPI7::ISpecificationDescriptionPtr description = descriptions->Active;
  
    if ( !description )
      description = descriptions->Add( _T( "" ), 0, _T( "" ) );
  
    if( description )
      m_spcBaseObjects = description->BaseObjects;
  }
  
  ksAPI7::ISpecificationBaseObjectPtr obj1;
  ksAPI7::ISpecificationBaseObjectsPtr Objects;

  if ( flagMode && m_spcBaseObjects ) 
  {
    _variant_t var;
    PArray<ksAPI7::ISpecificationBaseObjectPtr> arr;
    var = m_spcBaseObjects->GetSpecificationObjectsForGeom( (LPDISPATCH)propertyKeeperIn1, FALSE, STANDART_SECTION, 81764182256.0 );
    FillDispArray( var, arr );
    if( arr.Count() > 0 )
      obj1 = *arr[0];
  }

  if ( m_spcBaseObjects ) 
  {  
    if ( !obj1 ) 
    {
      // ������� ������ �����������.
      obj1 = m_spcBaseObjects->AddWithParam( ObjectCreateParam, propertyKeeperIn1, FALSE, (LPDISPATCH)propertyKeeperIn1 );
    }
  }
 
  if ( flagMode || obj1 )
  {
    ksAPI7::ISpecificationColumnsPtr columns = obj1->GetColumns();
    ksAPI7::ISpecificationColumnPtr column = columns->GetColumn( ksSColumnName, 1, 0 );
    ksAPI7::ISpecificationColumnItemsPtr columnItems = column->GetColumnItems();
  
    ksAPI7::ISpecificationColumnItemPtr item;
    if ( columnItems ) 
    {
      item = columnItems->GetItem( 1 );
      UINT uBuf = !!(tmp.f & ISPOLN );

      if( item )
        item->Visible = uBuf;

      if ( uBuf ) 
      {
        uBuf = 2;
        if( item )
          item->PutValue( uBuf );
      }
 
      item = columnItems->GetItem( 3 );
      if( item )
        item->PutValue( tmp.d );
 

      item = columnItems->GetItem( 4 );
      if ( item )
        item->PutVisible( FALSE );

      item = columnItems->GetItem( 5 );
      if ( item )
        item->PutVisible( FALSE );

      item = columnItems->GetItem( 6 );
      if ( item )
        item->PutVisible( FALSE );
 
      // ������� �����
      uBuf = (UINT)tmp.l;

      item = columnItems->GetItem( 8 );
      if( item )
        item->PutValue( uBuf );

      item = columnItems->GetItem( 6 );
      if ( item )
        item->PutVisible( FALSE );

      item = columnItems->GetItem( 9 );
      if ( item )
        item->PutVisible( FALSE );

      item = columnItems->GetItem( 10 );
      if ( item )
        item->PutVisible( FALSE );
 
      item = columnItems->GetItem( 11 );
      if ( item )
        item->PutVisible( FALSE );
 
      // ������� ����
      uBuf = tmp.gost;
      item = columnItems->GetItem( 13 );
      if ( item )
        item->PutValue( uBuf );
 
      float massa = tmp.f & ISPOLN ? tmp.m2 : tmp.m1;
      // 0 - ������ 1 - ������ ����� 3 - ������ 2 - ������
      massa = massa*( !tmp.indexMassa ? 1 : tmp.indexMassa==1 ? 0.356f : tmp.indexMassa==3 ? 0.97f : 1.08f )/COUNT_MASSA;
      columns = obj1->AdditionalColumns;
      column = columns->GetColumn( ksSColumnMass, 1, 0 );
      if ( column )
      {
        columnItems = column->GetColumnItems();
        if ( columnItems ) 
          columnItems->GetItem( 0 )->PutValue( massa );
      }

      obj1->Update();
      return obj1->GetReference();
    }
  }

  return 0;
}


//-------------------------------------------------------------------------------
// ��������� ������� ������������ 
// ---
bool Shpeel::DrawSpcObj( reference & spcObj ) 
{
  spcObj = 0;
  if( IsSpcObjCreate() ) 
  {
    if ( ReturnResult() == etError10 ) // ����������� ������
      ResultNULL();

    spcObj = EditSpcObj( spcObj );
    if( !par.flagAttr && spcObj ) 
    {
      DeleteObj( spcObj );
      spcObj = 0;
    }
  }
  return !!spcObj;
}


//---------------------------------------------------------------------------------------------------- 
// ����� ���������� 
// ---
void Shpeel::ShowParam()
{
    // ������������� ����
  if ( paramGrid ) 
  {
    //rowIndex = 1;

    //// "b(��) ������� �����"
    //AddDoubleToGrig( ID_PROP_WRENCH_ENDING, tmp.b ); 
    //// "b0(��) ������������ �����"
    //AddDoubleToGrig( ID_PROP_SCREW_ENDING, tmp.b1 ); 
    //// "p(��) ��� ������"
    //float step = !(tmp.f & PITCHOFF || !(tmp.f & PITCH)) ? tmp.p1 : tmp.p2;
    //AddDoubleToGrig( ID_PROP_PITCH, step ); 
    //// "c(��) �����"
    //AddDoubleToGrig( ID_PROP_FACET, tmp.c ); 
    //// "m(��) ����� 1000 ��"
    //float massa = tmp.f & ISPOLN ? tmp.m2 : tmp.m1;
    //// 0 - ������ 1 - ������ ����� 3 - ������ 2 - ������
    //massa = massa*( !tmp.indexMassa ? 1 : tmp.indexMassa==1 ? 0.356f : tmp.indexMassa==3 ? 0.97f : 1.08f );
    //AddDoubleToGrig( ID_PROP_MASSA_1000, massa ); 
  
    //paramGrid->RowCount = rowIndex;
    //paramGrid->UpdateParam();
  }   
}


//-------------------------------------------------------------------------------------
// ������������� ������
// ---
long Shpeel::GetSlideID()
{
  if( tmp.gost > 22041 ) 
  {
    if ( tmp.f & ALLST ) // ALLST - ������ �� �������
      return SH22042_3;
    else 
    {
      if ( tmp.f & ISPOLN )
        return SH22042_2;
      else
        return SH22042_1;
    }
  }
  else 
  {
    if ( tmp.f & ISPOLN )
      return SH22032_2;
    else
      return SH22032_1;
  }
}


//-------------------------------------------------------------------------------------
// ��������� �������� ��������
// ---
void Shpeel::OnChangeControlValue( long ctrlID, const VARIANT& newVal ) 
{


 switch ( ctrlID ) 
  {
    case IDP_SPC_OBJ: // ������ ������������
      par.flagAttr = !par.flagAttr;
      break;

    case ID_PROP_GOST: // ����
    { 
      CString strGost = newVal.bstrVal;
      short gost = (short)_ttoi( strGost.Left(5) );
      if ( gost && gost != tmp.gost ) 
      {


        tmp.gost = gost;
        CloseBase();
        OpenBase();
        if ( ReadShpeelBase( tmp.d, bBase, &tmp ) && ReadShpeelStBase(bBase, &tmp ) ) // ������ ��
        {

          ksAPI7::IPropertyListPtr diamList( GetPropertyControl(ID_PROP_DIAM) );
          if( diamList )
            FillDiametr( diamList ); // ��������� ������ ���������

          ksAPI7::IPropertyListPtr lenghtList( GetPropertyControl(ID_PROP_LENGHT) );
          if( lenghtList )
            FillLenght( lenghtList ); // ��������� ������ ����
        
          ksAPI7::IPropertyListPtr screwEndingList( GetPropertyControl(ID_PROP_SCREW_ENDING_LENGHT) );
          if( screwEndingList )
            // �������� � ������ 
            screwEndingList->Value =  LoadStr( GetTypeStrForGost( tmp.gost ));

          ksAPI7::IPropertyListPtr classList( GetPropertyControl(ID_PROP_CLASS) );
          if( classList )
            // �������� � ������ 
            classList->Value = LoadStr( GetClassStrForGost( tmp.gost ) ); 

		      ksAPI7::IPropertyMultiButtonPtr ispButtons( GetPropertyControl(ID_PROP_ISP) );
          if( ispButtons )
          {
            // �������� ����������� ���������� 2
            bool enable = tmp.f & ALLST ? false : true; // ALLST - ������ �� �������
            ispButtons->ButtonEnable [ IDP_BMP_G_I2 ] = enable;
            if( !enable )
              ispButtons->ButtonChecked[ IDP_BMP_G_I1 ] = !(tmp.f & ISPOLN ); // ���������� 1 ���������� �������, ��. ���������� 2 ����������
          }

          // ����� ���������� � ����� ( ���� ������� - ����� ��������� )
          ShowParam(); 

          // ������������ ������ � �����
          SetChanged(); 

        }
      }
      break;
    }      

    case ID_PROP_DIAM: // ������� 
    { 
      float diam = (float)newVal.dblVal; 
      if ( fabs(diam - tmp.d ) > EPSILON ) 
      {
        if ( ReadShpeelBase( diam, bBase, &tmp ) && ReadShpeelStBase(bBase, &tmp ) ) // ������ ��
        {
		      ksAPI7::IPropertyListPtr lenghtList( GetPropertyControl(ID_PROP_LENGHT) );
          if( lenghtList )
            FillLenght( lenghtList ); // ��������� ������ ����
          
		      ksAPI7::IPropertyMultiButtonPtr ispButtons( GetPropertyControl(ID_PROP_ISP) );
          if( ispButtons )
          {
            // �������� ����������� ���������� 2
            bool enable = tmp.f & ALLST ? false : true; // ALLST - ������ �� �������
            ispButtons->ButtonEnable [ IDP_BMP_G_I2 ] = enable;
            if( !enable )
              ispButtons->ButtonChecked[ IDP_BMP_G_I1 ] = !(tmp.f & ISPOLN ); // ���������� 1 ���������� �������, ��. ���������� 2 ����������
          }

          // ����� ���������� � ����� ( ������� ������� - ����� ��������� )
          ShowParam();
          // ������������ ������ � �����
          SetChanged(); 
        }
      }
      break;
    }

    case ID_POINT_3D_X:
    case ID_POINT_3D_Y:
    case ID_POINT_3D_Z:
    {
      ksAPI7::IPropertyPoint3DPtr point3D(GetPropertyControl(ID_POINT_3D));
      if (  point3D && m_part )
      {
        double x= point3D->CoordinateValue[0];
        double y= point3D->CoordinateValue[1];
        double z= point3D->CoordinateValue[2];


        IPlacementPtr PartPlace(m_part->GetPlacement());
        if ( PartPlace )
        {
          PartPlace->SetOrigin(x,y,z);
          m_part->UpdatePlacement();
        }

        if (  m_baseMan )
        {
          ksAPI7::IPlacement3DPtr IPlace3D(m_baseMan->Placement);
          if ( IPlace3D )
          {
            IPlace3D->SetOrigin(x,y,z);
            m_baseMan->UpdatePlacement( true );
          }
        }
        InitPoint3D(true,true);
      }
      break;
    }

	
    case ID_PROP_LENGHT: // ����� ������� 
    { 
      float lenght = (float)newVal.dblVal; 
      if ( fabs(lenght - tmp.l ) > EPSILON ) 
      {
        tmp.l = lenght;

        // ������������ ������ � �����
        SetChanged(); 
      }
      break;
    }
    case ID_PROP_SCREW_ENDING_LENGHT: // ����� ������������� ����� ������� 
    {
      CString strScrewEnding = newVal.bstrVal;    
      for ( int i = 0; i < 6; i++ )
      {
        if( strScrewEnding == LoadStr( IDS_STUD_1D + i ) )  
        {
          short gost = GetGostForTypeAndKlass( i, tmp.klass ); 
          if( gost != tmp.gost )
          { 
			      ksAPI7::IPropertyListPtr gostList( GetPropertyControl(ID_PROP_GOST) );
            if( gostList )
            {
              // �������� � ������ 
              LPCTSTR str = LoadStr( GetGostStr( gost ) );
              gostList->Value = str;
              OnChangeControlValue( ID_PROP_GOST, _variant_t( str ) );
              return;
            }
          }
        }       
      } 
      break;
    }
    case ID_PROP_CLASS: // ����� �������� ������� 
    {
      CString strClass = newVal.bstrVal;    
      for ( int j = 0; j < 2; j++ )
      {
        if( strClass == LoadStr( IDS_STUD_A + j ) )
        {
		      ksAPI7::IPropertyListPtr screwEndingList( GetPropertyControl(ID_PROP_SCREW_ENDING_LENGHT) );
          if( screwEndingList ) 
          {
            CString strScrewEnding = screwEndingList->Value.bstrVal;
            for ( int i = 0; i < 6; i++ )
            {
              if( strScrewEnding == LoadStr( IDS_STUD_1D + i ) )  
              {
                short gost = GetGostForTypeAndKlass( i, j );
                if( gost != tmp.gost )
                {
				          ksAPI7::IPropertyListPtr gostList( GetPropertyControl(ID_PROP_GOST) );
                  if( gostList )
                  {
                    // �������� � ������ 
                    LPCTSTR str = LoadStr( GetGostStr( gost ) );
                    gostList->Value = str;
                    OnChangeControlValue( ID_PROP_GOST, _variant_t( str ) );
                    return;
                  }
                }
              }
            }             
          }
        }
      }
      break;
    }
    case ID_PROP_MATERIAL: // �������� �������  
    {
      CString strMaterial = newVal.bstrVal;    
      for ( int i = 0; i < 4; i++ )
      {
        if( strMaterial == LoadStr( IDS_MATERIAL_STEEL + i ) )  
        {
          if( i != tmp.indexMassa )
          {
            tmp.indexMassa = i;

            // ����� ���������� � ����� ( ����� )
            ShowParam(); 
            break;
          }
        }
      } 
      break;
    }
  }
} 


//---------------------------------------------------------------------------------------------------- 
// ��������� ������� ������
// ---
void Shpeel::OnButtonClick( long buttonID ) 
{ 
  switch ( buttonID ) 
  { 
    case IDP_BMP_G_I1: // ���������� 1       
      tmp.f &= ~ISPOLN;

      // ����� ���������� � ����� ( ���������� ������ �� ����� ) 
      ShowParam(); 
      // ������������ ������ � �����
      SetChanged(); 

      break;
       
    case IDP_BMP_G_I2: // ���������� 2
      tmp.f |= ISPOLN; 

      // ����� ���������� � ����� ( ���������� ������ �� ����� ) 
      ShowParam(); 
      // ������������ ������ � �����
      SetChanged(); 

      break;

    case ID_PROP_G_STEP : // ��� 
    { 
      tmp.f & PITCH ? tmp.f &= ~PITCH : tmp.f |= PITCH;

      // ����� ���������� � ����� ( ��� ������ ) 
      ShowParam();

      break; 
    }
    case ID_SIMPLE : // ���������         
    {
      tmp.f & SIMPLE ? tmp.f &= ~SIMPLE : tmp.f |= SIMPLE;
      
      // ������������ ������ � �����
      SetChanged();       
      
      break;
    }

    case pbEnter: // ������� ������ � ������ � ��������� ��������
    {
      fixingPart = TRUE;

      m_process->Stop();

      break;
    }

    case IDS_TAKE_POINT:
    {
      if (doc)
      {
        bool lost = true;
        if (!m_takePoint)
        {
          IPartPtr topPart(doc->GetPart(pTop_Part), false);
          if (topPart)
            m_takePoint.Attach(topPart->NewEntity(o3d_point3D));
          lost = !!flagMode;
        }
        ksAPI7::IPoint3DPtr point3D(IUnknownPtr(ksTransferInterface(m_takePoint, ksAPI7Dual, 0), false));

        if (point3D)
        {
          IPlacementPtr place(m_part->GetPlacement(), false);

          if (place)
          {
            double x = 0.0, y = 0.0, z = 0.0;
            place->GetOrigin(&x, &y, &z);
            point3D->X = x;
            point3D->Y = y;
            point3D->Z = z;
          }
        }

        ksAPI7::IModelObjectPtr obj(IUnknownPtr(ksTransferInterface(m_takePoint, ksAPI7Dual, 0), false));
        m_process3D->RunTakeCreateObjectProcess(prPoint3D, obj, false, lost); // ������� ����������� �������� 3D- �����
      }
      break;
    }

    case ID_CHOSE_DETAIL:
    {
        LibMessage(_T("ID_CHOSE_DETAIL"));
        break;
    }
  }
}


//------------------------------------------------------------------------------
// ��������� ������ ���������
// ---
void Shpeel::FillDiametr( SArray<double> & diamList, float & curDiam ) 
{
  if( openBase )
  {
    float lMin         = MAX_LENGTH; // ����������� �������� � ������
    float lMax         = MIN_LENGTH; // ������������ �������� � ������
    bool  enterInRange = false;      // ������� �������� �� ������ � ����� ��������

    ShpeelTmp2 tmpD;
    memset( &tmpD, 0, sizeof(tmpD) );

    if ( ConditionT(bBase.bg, bBase.rg1, _T("t = 1")) ) 
    {
      int i = 1;
      while( i ) // ������������� ��� ������ ���� ������
      {                                 
        i = ReadRecord( bBase.bg, bBase.rg1, &tmpD ); // ��������� ������
        if ( i ) // ���� ������ �������
        {                                     
          // ������� ��� � ������
          diamList.Add( tmpD.d );

          if ( tmpD.d == curDiam )
            enterInRange = true; // ������� �������� �������� ������ � ����� ��������
          if ( tmpD.d < lMin )
            lMin = tmpD.d;       // ������� ����������� ��������
          if ( tmpD.d > lMax )                          
            lMax = tmpD.d;       // ������� ������������ ��������
        }
      }
      if ( !enterInRange )  // ���� ������� �������� �������� �� ������ � � ����� ��������
        curDiam = lMin;     // ����������� �������� ����������� ��������
      if ( curDiam < lMin ) // ���� ������� ������� �� ����������� ������� ����� ��������
        curDiam = lMin;     // ����������� ��� ����������� ��������
      if ( tmp.d > lMax )   // ���� ������� ������� �� ������������ ������� ����� ��������
        curDiam = lMax;     // ����������� ��� ������������ ��������
    }
  } 
}


//------------------------------------------------------------------------------
// ��������� ������ ���������
// ---
void Shpeel::FillDiametr(ksAPI7::IPropertyListPtr & diamList )
{
  SArray<double> list;
  FillDiametr( list, tmp.d );
  _variant_t values;
  CopySArrayToVariant(list, values);
  diamList->Add( values );
  diamList->Value = tmp.d;
}


//------------------------------------------------------------------------------
// ��������� ������ ����
// ---
void Shpeel::FillLenght( SArray<double> & lenList, float & curLen )
{
  if( openBase )
  {
    float lMin         = MAX_LENGTH; // ����������� �������� � ������
    float lMax         = MIN_LENGTH; // ������������ �������� � ������
    bool  enterInRange = false;      // ������� �������� �� ������ � ����� ��������

    ShpeelTmp3 tmpL;
    memset( &tmpL, 0, sizeof(tmpL) ); 

    TCHAR buf[128];
    _stprintf_s( buf, _T("t = 1 && d=%.1f"), tmp.d );

    if ( ConditionT(bBase.bs, bBase.rs1, buf) ) 
    {
      int i = 1;
      while( i ) // ������������� ��� ������ ���� ������
      {    
        i = ReadRecord( bBase.bs, bBase.rs1, &tmpL ); // ��������� ������
        if ( i ) // ���� ������ �������
        {      
          // ������� ��� � ������
          lenList.Add( tmpL.L ); // ������� ��� � ������

          if ( tmpL.L == curLen )
            enterInRange = true; // ������� �������� �������� ������ � ����� ��������
          if ( tmpL.L < lMin )
            lMin = tmpL.L;       // ������� ����������� ��������
          if ( tmpL.L > lMax )                          
            lMax = tmpL.L;       // ������� ������������ ��������
        }
      }
      if ( !enterInRange ) // ���� ������� �������� �������� �� ������ � � ����� ��������
        curLen = lMin;      // ����������� �������� ����������� ��������
      if ( curLen < lMin )  // ���� ������� ������� �� ����������� ������� ����� ��������
        curLen = lMin;      // ����������� ��� ����������� ��������
      if ( curLen > lMax )  // ���� ������� ������� �� ������������ ������� ����� ��������
        curLen = lMax;      // ����������� ��� ������������ ��������
    }
  }
}


//------------------------------------------------------------------------------
// ��������� ������ ����
// ---
void Shpeel::FillLenght(ksAPI7::IPropertyListPtr & lenghtList )
{
  SArray<double> list;
  FillLenght( list, tmp.l );
  _variant_t values;
  CopySArrayToVariant(list, values);
  lenghtList->Add( values );
  lenghtList->Value = tmp.d;
}

//-------------------------------------------------------------------------------
// ��������� ������ ���� ������������� �����
// ---
void Shpeel::FillScrewEnding( PArray<_bstr_t> & screwEndingList )
{
  for ( int i = 0; i < 6; i++ )
    screwEndingList.Add( new _bstr_t(LoadStr( IDS_STUD_1D + i )) );
}

//-------------------------------------------------------------------------------
// ��������� ������ ���� ������������� �����
// ---
void Shpeel::FillScrewEnding(ksAPI7::IPropertyListPtr & screwEndingList )
{
  PArray<_bstr_t> list;
  FillScrewEnding(list);
  _variant_t values;
  FillBSTRSafeArray(values, list);
  screwEndingList->Add( values );
  screwEndingList->Value = LoadStr(GetTypeStrForGost(tmp.gost));
}

//-------------------------------------------------------------------------------
// ��������� ������ ���� ������������� �����
// ---
void Shpeel::FillClass( PArray<_bstr_t> & classList )
{
  for ( int i = 0; i < 2; i++ )
    classList.Add( new _bstr_t(LoadStr( IDS_STUD_A + i )) );
}

//-------------------------------------------------------------------------------
// ��������� ������ ������ �������� 
// ---
void Shpeel::FillClass(ksAPI7::IPropertyListPtr & classList )
{
  PArray<_bstr_t> list;
  FillClass(list);
  _variant_t values;
  FillBSTRSafeArray(values, list);
  classList->Add( values );
  classList->Value = LoadStr(GetClassStrForGost(tmp.gost));
}


//-------------------------------------------------------------------------------
// ��������� ������ ������
// ---
void Shpeel::FillGost( PArray<_bstr_t>  & gostList )
{
  for ( int i = 0; i < 12; i++ )
    gostList.Add( new _bstr_t(LoadStr( STR_101 + i )) );  
}

//-------------------------------------------------------------------------------
// ��������� ������ ������
// ---
void Shpeel::FillGost(ksAPI7::IPropertyListPtr & gostList )
{
  PArray<_bstr_t> list;
  FillGost(list);
  _variant_t values;
  FillBSTRSafeArray(values, list);
  gostList->Add( values );
  // �������� � ������ 
  gostList->Value = LoadStr(GetGostStr(tmp.gost));
}


//-------------------------------------------------------------------------------
// ���������� �������� �� indexMassa
// 0 - ������ 1 - ������ ����� 2 - ������ 3 - ������
// ---
static short GetMaterialStr( short indexMassa )
{
  short n = 0;
  if( indexMassa >= 0 && indexMassa <= 3 )
    n = IDS_MATERIAL_STEEL + indexMassa;
  return n;
}

//-------------------------------------------------------------------------------
// ��������� ������ ������
// ---
void Shpeel::FillMaterial( PArray<_bstr_t>  & materialList )
{
  for ( int i = 0; i < 4; i++ )
    materialList.Add( new _bstr_t(LoadStr( IDS_MATERIAL_STEEL + i )) );  
}


//-------------------------------------------------------------------------------
// ��������� ������ ����������  
// ---
void Shpeel::FillMaterial(ksAPI7::IPropertyListPtr & materialList )
{
  PArray<_bstr_t> list;
  FillMaterial(list);
  _variant_t values;
  FillBSTRSafeArray(values, list);
  materialList->Add( values );
  // �������� � ������ 
  materialList->Value = LoadStr(GetMaterialStr(tmp.indexMassa));
}


//-------------------------------------------------------------------------------
// ��������� ������ ����������
// ---
void Shpeel::ShowControls() 
{
  if ( curentCollection )
  {
      ksAPI7::IPropertyListPtr chose_ditail = curentCollection->Add(ksControlListStr);
      chose_ditail->Name = _T("������� ������");
      chose_ditail->Id = ID_CHOSE_DETAIL;
      info_list inf_l; 
      inf_l.id = ID_CHOSE_DETAIL;
      inf_l.order_id = 0; v_info_list.push_back(inf_l);
      
      _upload_list(chose_ditail, patch_resure_detales);

      ksAPI7::IPropertyPoint3DPtr point3D(curentCollection->Add(ksControlPoint3D));
      if (point3D)
      {
          InitPropertyControl(point3D, ID_POINT_3D, ID_POINT_3D, ID_POINT_3D);
          ksAPI7::IPropertyEditPtr xEdit = point3D->Coordinate[0];
          if (xEdit)
          {
              xEdit->Id = ID_POINT_3D_X;
              inf_l.id = ID_POINT_3D_X;
              inf_l.order_id = 1; v_info_list.push_back(inf_l);
          }
          ksAPI7::IPropertyEditPtr yEdit = point3D->Coordinate[1];
          if (yEdit)
          {
              yEdit->Id = ID_POINT_3D_Y;
              inf_l.id = ID_POINT_3D_Y;
              inf_l.order_id = 2; v_info_list.push_back(inf_l);
          }
          ksAPI7::IPropertyEditPtr zEdit = point3D->Coordinate[2];
          if (zEdit)
          {
              zEdit->Id = ID_POINT_3D_Z;
              inf_l.id = ID_POINT_3D_Z;
              inf_l.order_id = 3; v_info_list.push_back(inf_l);
          }
          ksAPI7::IPropertyControl1Ptr ctrl1(point3D);
          if (ctrl1)
              ctrl1->AddAdditionButton(IDS_TAKE_POINT, 0xE1A0, LoadStr(IDS_TAKE_POINT), _T(""));
          if (flagMode && m_part)
          {
              IPlacementPtr place(m_part->GetPlacement(), false);
              double x = 0.0, y = 0.0, z = 0.0;
              if (place && place->GetOrigin(&x, &y, &z))
              {
                  point3D->CoordinateValue[0] = x;
                  point3D->CoordinateValue[1] = y;
                  point3D->CoordinateValue[2] = z;
                  point3D->CoordinateState[0] = ksCheckFixed;
                  point3D->CoordinateState[1] = ksCheckFixed;
                  point3D->CoordinateState[2] = ksCheckFixed;
              }
              procParam->DefaultControlFix = ksAllFix;
          }
      }

      //��������� ����� ������� ������ 
      ksAPI7::IPropertyEditPtr edit_h = curentCollection->Add(ksControlEditReal);
      edit_h->Name = _T("������");
      edit_h->Id = ID_H_3D_PLATE;
      ksAPI7::IPropertyEditPtr edit_w = curentCollection->Add(ksControlEditReal);
      edit_w->Name = _T("������");
      edit_w->Id = ID_W_3D_PLATE;
      ksAPI7::IPropertyEditPtr edit_z = curentCollection->Add(ksControlEditReal);
      edit_z->Name = _T("�������");
      edit_z->Id = ID_Z_3D_PLATE;

  }
  //// ��������� �����
  //ksAPI7::IPropertyListPtr gostList( CreateStringList() );
  //if ( gostList ) 
  //{
  //  InitPropertyControl( gostList, ID_PROP_GOST, IDS_GOST_TIPS, IDS_GOST_TIPS );
  //  FillGost( gostList ); // ��������� ������ ������
  //}
  //
  //if ( openBase ) // ���� �� �������
  //  CloseBase();  // ��������� ������ ��

  //// ��������� ����� �� � ��������� ������� ���������
  //if ( OpenBase() )
  //{ 
  //  // ��������� ��������� ������
  //  ksAPI7::IPropertyListPtr diamList( CreateRealList() );
  //  if ( diamList ) 
  //  {
  //    InitPropertyControl( diamList, ID_PROP_DIAM, IDS_DIAM_TIPS, IDS_DIAM_TIPS );
  //    FillDiametr( diamList ); // ��������� ������ ���������
  //  }

  //  // ��������� ���� �������
  //  ksAPI7::IPropertyListPtr lenghtList( CreateRealList() );
  //  if ( lenghtList ) 
  //  {
  //    InitPropertyControl( lenghtList, ID_PROP_LENGHT, IDS_LENGHT_TIPS, IDS_LENGHT_TIPS );
  //    FillLenght( lenghtList ); // ��������� ������ ����
  //  }

  //  // ��������� ���� ������������� �����
  //  ksAPI7::IPropertyListPtr screwEndingList( CreateStringList() );
  //  if ( screwEndingList ) 
  //  {
  //    InitPropertyControl( screwEndingList, ID_PROP_SCREW_ENDING_LENGHT, IDS_SCREW_ENDING_TIPS, IDS_SCREW_ENDING_TIPS );
  //    FillScrewEnding( screwEndingList ); // ��������� ������ ���� ������������� �����
  //  }
  //}

  //// ��������� ������ ��������
  //ksAPI7::IPropertyListPtr classList( CreateStringList() );
  //if ( classList ) 
  //{
  //  InitPropertyControl( classList, ID_PROP_CLASS, IDS_CLASS_TIPS, IDS_CLASS_TIPS );
  //  FillClass( classList ); // ��������� ������ ������ ��������
  //}


  // ��������� ���������
  ksAPI7::IPropertyListPtr materialList( CreateStringList() );
  if ( materialList ) 
  {
    InitPropertyControl( materialList, ID_PROP_MATERIAL, IDS_MATERIAL_TIPS, IDS_MATERIAL_TIPS );
    FillMaterial( materialList ); // ��������� ������ ����������
  }
  
  //CreateSeparator(ksSeparatorDownName);

  //// ����������
  //ksAPI7::IPropertyMultiButtonPtr ispButtons( CreateMultiButton(ksRadioButton) );
  //if ( ispButtons ) 
  //{
  //  InitPropertyControl( ispButtons, ID_PROP_ISP, IDS_ISP_HINT, IDS_ISP_TIPS, TRUE, ksNameVerticalVisible );
  //  
  //  // ������ ���������� 1
  //  AddButton2( ispButtons, IDP_BMP_G_I1, IDF_BMP_G_I1, !(tmp.f & ISPOLN ) );
  //  // �������� ����������� ���������� 2
  //  bool enable = tmp.f & ALLST ? false : true; // ALLST - ������ �� �������
  //  // ������ ���������� 2
  //  AddButton2( ispButtons, IDP_BMP_G_I2, IDF_BMP_G_I2, !!(tmp.f & ISPOLN ), enable );
  //}

  //CreateSeparator(ksSeparatorDownName);

  //// �������������� ���������
  //ksAPI7::IPropertyMultiButtonPtr paramButtons( CreateMultiButton(ksCheckButton) );
  //if ( paramButtons ) 
  //{
  //  InitPropertyControl( paramButtons, IDP_FLAGS, IDS_FLAGS_TIPS, IDS_FLAGS_TIPS, TRUE, ksNameVerticalVisible );
  //  // ��� ������
  //  AddButton2( paramButtons, ID_PROP_G_STEP, IDF_PROP_G_STEP, !!(tmp.f & PITCH), !(tmp.f & PITCHOFF) );
  //  // ���������
  //  AddButton2( paramButtons, ID_PROP_SIMPLE, IDF_PROP_SIMPLE, !!(tmp.f & SIMPLE), true );
  //}

  //// ��������� ������ ������������
  //if ( IsSpcObjCreate() ) 
  //{
  //  ksAPI7::IPropertyCheckBoxPtr checkBox( CreateCheckBox(!!par.flagAttr) );
  //  if ( checkBox ) 
  //    InitPropertyControl( checkBox, IDP_SPC_OBJ, IDS_SPC_OBJ_TIPS, IDS_SPC_OBJ_TIPS );
  //}

  //CreateTab( ID_PROP_ELEMENT_PARAM );
}


//-------------------------------------------------------------------------------------
// ������������ ������, �����, ������� ����� ���������  
// ---
void Shpeel::RedrawPhantom()
{
  if( Changed() )
  {
    // ��������� �������
    if ( m_part )
    {
      SetParam( m_part ); 
      m_part->Update();
    }
    
    // �������������� ����� � ����
    if ( slideBox ) 
    {
      long slideID    = (long)GetSlideID();
      long slideIDOld = slideBox->DrawingSlide;
      if (slideID != slideIDOld) 
      {
        slideBox->DrawingSlide = slideID;
        slideBox->UpdateParam();
      }
    }
    SetChanged( false );
  }
}

//-------------------------------------------------------------------------------------
// ������������ ������, �����, ������� ����� ���������  
// ---
void Shpeel::get_part()
{

}

#include "draw_obj.h"
//-------------------------------------------------------------------------------------
// ��������� 3D ��������
// ---
void Shpeel::Draw3D()
{
  if (doc && m_part) // ���� ���� �������� � � ������ � ������ 
  {
    long toolBarID  = IDP_STUDS;
    long firstTabID = IDP_STUDS;
    _bstr_t bstr;
    if (flagMode)  //��������� ������ 
      GetParam();
    else
    {
      load_default_panel();
      /*LibMessage((L"Studs3d:" + std::to_wstring(__LINE__)).c_str());
      bstr = GetFileName();
      m_part->SetFileName(bstr);*/
    }
    draw_panel(doc, m_part);


    SpecPropertyToolBarEnum toolBarType = pnEnterEscHelp;

    if (InitProcessParam(toolBarID, toolBarType, firstTabID))
    {
      procParam->AutoReduce = false;

      ksAPI7::IKompasDocument3D1Ptr doc3D(pNewKompasAPI->ActiveDocument);

      ksAPI7::IPart7Ptr IPart7(IUnknownPtr(ksTransferInterface(m_part, ksAPI7Dual, 0), false));


      if (doc3D)
      {
        ksAPI7::IModelObjectPtr Model(IPart7);
        


        m_process = doc3D->LibProcess[ksProcess3DPlacementAndEntity];
        m_process3D = m_process;
        if ( m_process3D )
        {
          ksAPI7::IManipulatorsPtr manColl( m_process3D->Manipulators ); // �������� ��������� ������������� ��������
          if ( manColl )
          {

            m_baseMan = manColl->Add(ksPlacement3DManipulator); // ��������� ����� ����������� � ��������� 
            m_manipulator = m_baseMan;
            if (  m_manipulator )
            {

                m_baseMan->Id = IDR_BASEMANID;
              m_baseMan->Active  =  true;
              m_baseMan->Visible = false;
          
              // ����������� ��������� ������������ ���������� ������������
              m_manipulator->PrimitiveVisible[ksMPPlaceXOY] =  false;
              m_manipulator->PrimitiveVisible[ksMPPlaceXOZ] =  false;
              m_manipulator->PrimitiveVisible[ksMPPlaceYOZ] =  false;
              m_manipulator->PrimitiveVisible[ksMPTextX]    =  false;
              m_manipulator->PrimitiveVisible[ksMPTextY]    =  false;
              m_manipulator->PrimitiveVisible[ksMPTextZ]    =  false;
              m_manipulator->PrimitiveVisible[ksMPOriginal] =  true ;
          
              m_baseMan->UpdatePlacement(true);
              new Process3DManipulatorsEvent( manColl, *this, true );

            }
          }

          m_process3D->PhantomObject = Model;


          m_process->ProcessParam = procParam;


          new Process3DEvent( m_process3D, *this, true );
          SetChanged();

          RedrawPhantom();

          m_process->Run( false, false );


        }
      }
    }
  }
}


//------------------------------------------------------------------------------
// ������� ������
// ---
BOOL Shpeel::FilterObject( LPDISPATCH entity ) 
{
  bool res = false;  // ������� ������������� �������
  ksAPI7::IModelObject1Ptr Model1( entity );

  if( !Model1->GetIsExternalObject())  // ���������� ������ ���������� �������
  {
    IEntityPtr entity1(IUnknownPtr(ksTransferInterface(entity, ksAPI3DCom, o3d_entity),false));
    if (IsSurface(entity1) || IsSurface(entity1, false) || IsAxis(entity1)) 
      res = true;
  }
  return res; 
}


//-------------------------------------------------------------------------------------
// ksEndProcess    -��������� ��������.
// ---
BOOL Shpeel::EndProcess()
{
  if ( fixingPart ) // ���������� ������
  {
    SetParam(m_part);
    if ( !flagMode )
    {
      _bstr_t fileName = GetFileName();
      doc->SetPartFromFile( fileName, m_part, false );
    }
    m_part->UpdatePlacement();  // ��������� ��������� ������

    PArray<ksAPI7::IModelObjectPtr> R;

    FillObjects( Objects, R );

    for ( unsigned int Idx=0; Idx<R.Count(); Idx++ )
    {
      IEntityPtr next = IUnknownPtr(ksTransferInterface(*R[Idx], ksAPI3DCom, o3d_entity), false);
    // ������� ���������� ���������� � ����������� �� �����������
      if (IsSurface(next))
        doc->AddMateConstraint (mc_Coincidence, next , GetEntityByName(_T("Plane"), m_part), GetDirectionElem(), 2, 0);
      
      if (IsSurface(next, false))
        doc->AddMateConstraint (mc_Concentric , next , GetEntityByName(_T("Axis"),  m_part), 0, 2, 0);
      
      if (IsAxis(next))
        doc->AddMateConstraint(mc_Concentric , next , GetEntityByName(_T("Axis"),  m_part), 0, 2, 0);
    }
    // ������������� �������, ����������� �������
    m_part->SetStandardComponent(true);

    reference spcObj;
    if (DrawSpcObj(spcObj)) // ����� ������� ��
    {
      // ����������� ���������
      ksEditWindowSpcObject(spcObj);
    }
  }

  return PropertyManagerObject::EndProcess();
}

void Shpeel::UnadviseEvents()
{
  if ( m_process3D )
    BaseEvent::TerminateEvents( m_process3D->Manipulators ); 
  PropertyManagerObject::UnadviseEvents();
}

//-------------------------------------------------------------------------------------
// ksEndProcess    -������� �������� ������� � - --����������� ������.
// ---
BOOL Shpeel::TakeObject(LPUNKNOWN  obj)
{
  m_takePoint = obj;

  ksAPI7::IPoint3DPtr point3D(obj);

  if (point3D)
  {
    IPlacementPtr place(m_part->GetPlacement(), false);
    if (place)
    {
      double x = point3D->X, y = point3D->Y, z = point3D->Z;
      place->SetOrigin(x, y, z);
      if ( m_baseMan )
      {
        ksAPI7::IPlacement3DPtr IPlace3D( m_baseMan->Placement );
        if ( IPlace3D )
        {
          IPlace3D->SetOrigin( x, y, z );
          m_baseMan->UpdatePlacement( true );
        }
      }
      InitPoint3D( true, true );
    }
  }
  return TRUE;
}

//-------------------------------------------------------------------------------------
// ksPlacementChanged    -�������� ��������� �������
// ---
BOOL Shpeel::PlacementChanged(LPDISPATCH Object)
{
  ksAPI7::IKompasDocument3DPtr   Doc3D(pNewKompasAPI->ActiveDocument);
  if ( !Doc3D )
    return TRUE;

  ksAPI7::IChooseManagerPtr ChooseMan = Doc3D->ChooseManager;
  if ( !ChooseMan )
    return TRUE;

  ksAPI7::IModelObjectPtr  ModelObject(Object);

  if ( ModelObject && FilterObject( Object ) ) // ���� ����������� ��� �������� �������� 
  {
    if ( ChooseMan->IsChoosen(ModelObject) )   // ���������, ������� �� ����������� 
      ChooseMan->Unchoose((LPDISPATCH)ModelObject);
    else 
      ChooseMan->Choose((LPDISPATCH)ModelObject);

    if ( m_mCol )
      m_mCol->Clear();

    Objects = ChooseMan->ChoosenObjects;
    bool visible = Objects.vt == VT_EMPTY; 
    InitPoint3D(visible/*visible*/, false/*fix*/);

    PArray<ksAPI7::IModelObjectPtr> Array;

    FillObjects(Objects, Array);

    for (unsigned int Idx = 0; Idx < Array.Count(); Idx++)
    {
      MateProcess( *Array[Idx] );
    }

    m_baseMan->Visible = FALSE;

  }
  else 
  {
    ChooseMan->UnchooseAll();

    ksAPI7::IPlacement3DPtr newPlace = m_process3D->Placement;  // �������������� ������� ��� �����
    if (  newPlace )
    {
      double x = 0.0, y = 0.0, z = 0.0;

      newPlace->GetOrigin( &x, &y, &z );     // ������� ���������� 

      IPlacementPtr partPlace( m_part->GetPlacement() );
      if ( partPlace )
      {
        partPlace->SetOrigin( x, y, z );
        m_part->UpdatePlacement();
      }

      if (  m_baseMan )
      {
        ksAPI7::IPlacement3DPtr Place3D( m_baseMan->Placement );
        if ( Place3D )
        {
          Place3D->SetOrigin( x, y, z );
          m_baseMan->UpdatePlacement( TRUE );  // ������� �������������� ������������
        }
        m_baseMan->Visible = TRUE;         // ����������� ����� 
      }

      InitPoint3D( true/*visible*/, true/*fix*/ );

      procParam->DefaultControlFix=ksAllFix; // ����������� �������

    }
  }
  return TRUE;
}

//-------------------------------------------------------------------------------------
//������� ��������� ���������� � ������������ � ��������� ������������
// ---
int Shpeel::MateProcess(LPDISPATCH entity)
{
  ksAPI7::IMateConstraints3DPtr mateCollection(m_process3D->MateConstraints);

  IEntityPtr entity1(IUnknownPtr(ksTransferInterface(entity, ksAPI3DCom, o3d_entity), false));

  m_mCol=IUnknownPtr(ksTransferInterface(mateCollection, ksAPI3DCom, o3d_mateConstraintCollection), false);  

  // �������� ���������� ���������� � ����������� �� ����������� 
  if (IsSurface(entity1)) 
  {
    AddMate(mc_Coincidence, GetDirectionElem(), 1, 0, entity1, _T("Plane"), m_part, m_mCol);
  }
  if (IsSurface(entity1, false))
  {
    AddMate(mc_Concentric, 0, 1, 0, entity1, _T("Axis"), m_part, m_mCol);
  }
  if (IsAxis(entity1)) 
  {
    AddMate(mc_Concentric, 0, 1, 0, entity1, _T("Axis"), m_part, m_mCol);
  }

  return 1;
}

//-------------------------------------------------------------------------------------
//������������� 3D-�����
// ---
bool Shpeel::InitPoint3D( bool visible, bool fix )
{
  if ( m_part )
  {
     IPlacementPtr place( m_part->GetPlacement(), false );
     ksAPI7::IPropertyPoint3DPtr point3D(GetPropertyControl(ID_POINT_3D));

    if ( place )
    {
      double x = 0.0, y = 0.0, z = 0.0;
      place->GetOrigin( &x, &y, &z );

      if ( point3D )
      {
        if ( visible != !!point3D->Visible )
          point3D->Visible = visible;
        point3D->CoordinateValue[0] = x;
        point3D->CoordinateValue[1] = y;
        point3D->CoordinateValue[2] = z;
        point3D->CoordinateState[0] = fix ? ksCheckFixed : ksCheckVariable; 
        point3D->CoordinateState[1] = fix ? ksCheckFixed : ksCheckVariable; 
        point3D->CoordinateState[2] = fix ? ksCheckFixed : ksCheckVariable;
  //    point3D->CheckState = ksCheckFixed;
        procParam->DefaultControlFix = fix ? ksAllFix : ksAllFixOff;
      }
    }
  }
  return true;
}


//------------------------------------------------------------------------------
// ������� � ������ ����������� � �������� ������
// ---
IEntityPtr Shpeel::GetEntityByName( const TCHAR * name, IPartPtr& p ) 
{
  IEntityPtr entity( NULL );
  if (!(bool)p )
    p = m_part;
  if ( p ) 
    entity.Attach( p->GetObjectByName(_bstr_t(name), o3d_unknown, TRUE, TRUE) );
  return entity;
}


// ILibPropertyObject
//-----------------------------------------------------------------------------
// ��� ������ �������� �� ��������� �������������
// ---
LPOLESTR _IFUNC Shpeel::GetGroupName()
{
  CString res(_T("������"));
  return res.AllocSysString();
}

//-----------------------------------------------------------------------------
// �������� �������� � ������ �������
// ---
BOOL _IFUNC Shpeel::GetProperty( int index, PropertyParam * param )
{
  if ( param )
  {
    param->propertyId = GetPropertyID ( index );
    param->propertyInstance = StepDLL.hModule;
    param->enable = true;
    switch ( param->propertyId )
    {
      case ID_PROP_ELEMENT_PARAM:
      case ID_PROP_OTHER_PARAM:
      {
        param->propertyType = ksOPControlGroup;      // ������
        break;
      }
      case ID_PROP_GOST               :
      case ID_PROP_SCREW_ENDING_LENGHT:
      case ID_PROP_CLASS              :
      case ID_PROP_MATERIAL           :
      {
        param->propertyType = ksOPControlListString;
        break;
      }
      case ID_PROP_DIAM   :
      case ID_PROP_LENGHT :
      {
        param->propertyType = ksOPControlListDouble;
        break;
      }
      case ID_PROP_ISP    :
      {
        param->propertyType = ksOPControlListBmp;
        break;
      }
      case ID_PROP_G_STEP: // ������ ���
      case ID_PROP_SIMPLE: // ���������
      {
        param->propertyType = ksOPControlListCheck;
        break; 
      }
      case ID_PROP_WRENCH_ENDING: // b(��) ������� �����
      case ID_PROP_SCREW_ENDING:  // b0(��) ������������ �����
      case ID_PROP_PITCH:         // p(��) ��� ������
      case ID_PROP_FACET:         // c(��) �����
      case ID_PROP_MASSA_1000:    // m(��) ����� 1000 ��
      {
        param->propertyType = ksOPControlEditDouble;
        param->enable       = false;
        break;
      }
    }
  }
  return param && param->propertyId;
}


//-----------------------------------------------------------------------------
// �������� ��������� ��������
// ---
BOOL _IFUNC Shpeel::UpdateProperty( PropertyParam * param )
{
  BOOL res = TRUE;
  if ( param )
  {
    if ( !openBase ) // ���� �� �������
      OpenBase();  // ��������� ������ ��

    param->propertyInstance = StepDLL.hModule;
    switch ( param->propertyId )
    {
      case ID_PROP_GOST               :
      {
        param->propertyValue.bstrVal = CString(LoadStr( GetGostStr( tmp.gost ) )).AllocSysString(); 
        if ( param->additionData.vt == VT_EMPTY )
        {
          PArray<_bstr_t> list;
          FillGost(list);
          FillBSTRSafeArray( param->additionData, list );
        }
        break;
      }
      case ID_PROP_DIAM   :
      {
        SArray<double> list;
        FillDiametr( list, tmp.d );
        VariantClear(&param->additionData);
        CopySArrayToVariant(list, param->additionData);
        param->propertyValue.dblVal = tmp.d;
        break;
      }
      case ID_PROP_LENGHT :
      {
        SArray<double> list;
        FillLenght( list, tmp.l );
        VariantClear(&param->additionData);
        CopySArrayToVariant(list, param->additionData);
        param->propertyValue.dblVal = tmp.l;
        break;
      }
      case ID_PROP_SCREW_ENDING_LENGHT:
      {
        param->propertyValue.bstrVal = CString(LoadStr(GetTypeStrForGost(tmp.gost))).AllocSysString();
        if ( param->additionData.vt == VT_EMPTY )
        {
          PArray<_bstr_t> list;
          FillScrewEnding(list);
          FillBSTRSafeArray(param->additionData, list);
        }
        break;
      }
      case ID_PROP_CLASS              :
      {
        param->propertyValue.bstrVal = CString(LoadStr(GetClassStrForGost(tmp.gost))).AllocSysString();
        if ( param->additionData.vt == VT_EMPTY )
        {
          PArray<_bstr_t> list;
          FillClass(list);
          FillBSTRSafeArray(param->additionData, list);
        }
        break;
      }
      case ID_PROP_MATERIAL           :
      {
        param->propertyValue.bstrVal = CString(LoadStr(GetMaterialStr(tmp.indexMassa))).AllocSysString();
        if ( param->additionData.vt == VT_EMPTY )
        {
          PArray<_bstr_t> list;
          FillMaterial(list);
          FillBSTRSafeArray(param->additionData, list);
        }
        break;
      }
      case ID_PROP_ISP    :
      {
        param->enable = tmp.f & ALLST ? false : true; // ALLST - ������ �� �������
        param->emptyValue = !param->enable;
        param->propertyValue.lVal = tmp.f & ISPOLN ? IDP_BMP_G_I2 : IDP_BMP_G_I1; // ���������� 1 ���������� �������, ��. ���������� 2 ����������
        break;
      }
      case ID_PROP_G_STEP: param->propertyValue.boolVal = tmp.f & PITCH;  break; // ������ ���
      case ID_PROP_SIMPLE: param->propertyValue.boolVal = tmp.f & SIMPLE; break; // ���������

      case ID_PROP_WRENCH_ENDING: param->propertyValue.dblVal = tmp.b;  break; // b(��) ������� �����
      case ID_PROP_SCREW_ENDING:  param->propertyValue.dblVal = tmp.b1; break; // b0(��) ������������ �����
      case ID_PROP_PITCH:         param->propertyValue.dblVal = !(tmp.f & PITCHOFF || !(tmp.f & PITCH)) ? tmp.p1 : tmp.p2; break; // p(��) ��� ������
      case ID_PROP_FACET:         param->propertyValue.dblVal = tmp.c;  break;// c(��) �����
      case ID_PROP_MASSA_1000:    // m(��) ����� 1000 ��
      {
        float massa = tmp.f & ISPOLN ? tmp.m2 : tmp.m1;
        // 0 - ������ 1 - ������ ����� 3 - ������ 2 - ������
        massa = massa*( !tmp.indexMassa ? 1 : tmp.indexMassa==1 ? 0.356f : tmp.indexMassa==3 ? 0.97f : 1.08f );
        param->propertyValue.dblVal = massa;
        break;
      }
    }
  }
  return param && param->propertyId;
}


//-----------------------------------------------------------------------------
// �������� ��������
// ---
int _IFUNC Shpeel::ApplyProperty( PropertyParam * param )
{
  BOOL res = FALSE;
  bool setParam = TRUE;
  if ( param )
  {
    if ( !openBase ) // ���� �� �������
      OpenBase();  // ��������� ������ ��
    switch ( param->propertyId )
    {
      case ID_PROP_ISP:
      {
        param->propertyValue.intVal == IDP_BMP_G_I1 ? tmp.f &= ~ISPOLN : tmp.f |= ISPOLN;
        res = TRUE;
        break;
      }

      case ID_PROP_G_STEP : // ��� 
      { 
        param->propertyValue.boolVal ? tmp.f |= PITCH : tmp.f &= ~PITCH;
        res = TRUE;
        break;
      }
      case ID_PROP_SIMPLE : // ���������         
      {
        param->propertyValue.boolVal ? tmp.f |= SIMPLE : tmp.f &= ~SIMPLE;
        res = TRUE;
        break;
      }
      break;
      case ID_PROP_DIAM: // ������� 
      { 
        float diam = (float)param->propertyValue.dblVal; 
        if ( fabs(diam - tmp.d ) > EPSILON ) 
          res = ReadShpeelBase( diam, bBase, &tmp ) && ReadShpeelStBase(bBase, &tmp ); // ������ ��
      }
      case ID_PROP_LENGHT: // ����� ������� 
      { 
        float lenght = (float)param->propertyValue.dblVal; 
        if ( fabs(lenght - tmp.l ) > EPSILON ) 
        {
          tmp.l = lenght;
          res = TRUE;
        }
        break;
      }
      case ID_PROP_MATERIAL:
      {
        CString strMaterial = param->propertyValue.bstrVal;
        for ( int i = 0; i < 4; i++ )
        {
          if( strMaterial == LoadStr( IDS_MATERIAL_STEEL + i ) )  
          {
            if( i != tmp.indexMassa )
            {
              tmp.indexMassa = i;
              res = TRUE;
              break;
            }
          }
        }
        break;
      }
      case ID_PROP_SCREW_ENDING_LENGHT:
      case ID_PROP_CLASS:
      {
        CString newVal = param->propertyValue.bstrVal;
        int classId = GetClassStrForGost( tmp.gost );
        int typeId  = GetTypeStrForGost ( tmp.gost );

        int classIndex = classId - IDS_STUD_A;
        int typeIndex  = typeId  - IDS_STUD_1D;

        if ( param->propertyId == ID_PROP_CLASS )
        {
          for ( int i = 0; i < 2; i++ )
          {
            if( newVal == LoadStr( IDS_STUD_A + i ) )  
            {
              classIndex = i;
              break;
            }
          }
        }
        else
        {
          for ( int i = 0; i < 6; i++ )
          {
            if( newVal == LoadStr( IDS_STUD_1D + i ) )  
            {
              typeIndex = i;
              break;
            }
          }
        }
        short newGoct = GetGostForTypeAndKlass( typeIndex, classIndex );
        if ( newGoct != tmp.gost )
        {
          CloseBase();
          PropertyParam tmp;
          tmp.propertyId = ID_PROP_GOST;
          tmp.propertyValue.vt = VT_BSTR;
          tmp.propertyValue.bstrVal = CString(LoadStr(GetGostStr(newGoct))).AllocSysString();
          res = ApplyProperty( &tmp );
          VariantClear(&tmp.propertyValue);
          setParam = FALSE; // ��������� ����������� � ApplyProperty
        }
        break;
      }
      case ID_PROP_GOST:
      {
        CString strGost = param->propertyValue.bstrVal;
        short gost = (short)_ttoi( strGost.Left(5) );
        if ( gost && gost != tmp.gost ) 
        {
          tmp.gost = gost;
          CloseBase();
          OpenBase();
          res = ReadShpeelBase( tmp.d, bBase, &tmp ) && ReadShpeelStBase( bBase, &tmp ); // ������ ��
        }
        break;
      }
    }
    if ( res && setParam )
      SetParam(m_part);
  }
  return res;
}


//-----------------------------------------------------------------------------
// ������ ������������� �������� �� �������
// ---
int Shpeel::GetPropertyID ( int index )
{
  int res = 0;
  switch ( index )
  {
    case 0  : res = ID_PROP_ELEMENT_PARAM;       break; // ��������� ��������
    case 1  : res = ID_PROP_GOST;                break; // ����
    case 2  : res = ID_PROP_DIAM;                break; // �������
    case 3  : res = ID_PROP_LENGHT;              break; // �����
    case 4  : res = ID_PROP_SCREW_ENDING_LENGHT; break; // ����� ������������� �����
    case 5  : res = ID_PROP_CLASS;               break; // ����� ��������
    case 6  : res = ID_PROP_MATERIAL;            break; // ��������
    case 7  : res = ID_PROP_ISP;                 break; // ����������
    case 8  : res = ID_PROP_G_STEP;              break; // ������ ���
    case 9  : res = ID_PROP_SIMPLE;              break; // ���������

    case 10 : res = ID_PROP_OTHER_PARAM;         break; // ���������� ������
    case 11 : res = ID_PROP_WRENCH_ENDING;       break; // b(��) ������� �����
    case 12 : res = ID_PROP_SCREW_ENDING;        break; // b0(��) ������������ �����
    case 13 : res = ID_PROP_PITCH;               break; // p(��) ��� ������
    case 14 : res = ID_PROP_FACET;               break; // c(��) �����
    case 15 : res = ID_PROP_MASSA_1000;          break; // m(��) ����� 1000 ��
  }
  return res;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// ksRotateManipulator - ������� ������������
// ---
bool Shpeel::RotateManipulator(double X0, double Y0, double Z0, double AxisZX, double AxisZXY, double AxisZZ, double angle, bool FromEdit)
{
  ksAPI7::IPart7Ptr IPart7(IUnknownPtr(ksTransferInterface(m_part, ksAPI7Dual, 0), false));
  ksAPI7::IPlacement3DPtr Place(IPart7->Placement);
  
  Place->Rotate(X0, Y0, Z0, AxisZX, AxisZXY, AxisZZ, angle );
  
  IPart7->UpdatePlacement(true);
  
  ksAPI7::IPlacement3DPtr PlaceBase(m_baseMan->Placement);
  PlaceBase->InitByMatrix3D(Place->GetMatrix3D());  // ��������� ����� ������� ��������� �� ������� 
  m_baseMan->UpdatePlacement(true);

  InitPoint3D(true/*visible*/, true/*fix*/);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// ksMoveManipulator   - ������������ ������������
// ---
bool Shpeel::MoveManipulator(long ManipulatorId, double VX, double VY, double VZ, double Delta, bool FromEdit)
{
  ksAPI7::IPart7Ptr IPart7(IUnknownPtr(ksTransferInterface(m_part, ksAPI7Dual, 0), false));

  ksAPI7::IPlacement3DPtr Place(IPart7->Placement);
 
  ksAPI7::IPlacement3DPtr PlaceBase(m_baseMan->Placement);
  double x = 0.0, y = 0.0, z = 0.0;
  Place->GetOrigin(&x, &y, &z);
  PlaceBase->SetOrigin(VX+x, VY+y, VZ+z);  // ��������� ����� ��������� ������������ � ����������� �������� 
  
  Place->SetOrigin(VX + x, VY + y, VZ + z);
  m_baseMan->UpdatePlacement(true);
  IPart7->UpdatePlacement(true);
  InitPoint3D( true/*visible*/, true/*fix*/ );
  return true;
}


//-------------------------------------------------------------------------------------------
// ksBeginDragManipulator - ������ �������������� ������������
// ---
bool Shpeel::BeginDragManipulator(long ManipulatorId, long PrimitiveType)
{

  return true;
}


//-------------------------------------------------------------------------------------------
// ksEndDragManipulator - ��������� �������������� ������������
// ---
bool Shpeel::EndDragManipulator(long ManipulatorId, long PrimitiveType)
{
  InitPoint3D(true/*visible*/, true/*fix*/);
  return true;
}


//------------------------------------------------------------------------------
// AddRef
// ---
unsigned long _IFUNC Shpeel::AddRef() 
{
  refCount++;
  return refCount;
}

//------------------------------------------------------------------------------
// Release
// ---
unsigned long _IFUNC Shpeel::Release() 
{
  if ( refCount ) 
    refCount--;
  if( !refCount )
  {
    delete this;
    return 0;
  }
  return refCount;
}

//------------------------------------------------------------------------------
// QueryInterface
// ---
HRESULT _IFUNC Shpeel::QueryInterface( const GUID far& iid, void far*far* iface )
{
  if ( iid == IID_IUnknown || iid == IID_ILibraryObject ) 
  {
    *iface = static_cast<ILibraryObject*>(this);
    AddRef();
    return S_OK;
  }
  if ( iid == IID_ILibPropertyObject ) 
  {
    *iface = static_cast<ILibPropertyObject*>(this);
    AddRef();
    return S_OK;
  }

  *iface = NULL;
  return E_NOINTERFACE;
}


//------------------------------------------------------------------------------
// QueryInterface
// ---
HRESULT _IFUNC Shpeel::LtQueryInterface( int idType, void far*far* iface )
{
  if ( idType == idd_ILibPropertyObject3D ) 
  {
    *iface = static_cast<ILibPropertyObject*>(this);
    AddRef();
    return S_OK;
  }

  *iface = NULL;
  return E_NOINTERFACE;
}


//-------------------------------------------------------------------------------
// ������� ��������������� ������ ��� ������ � Hot �������
// ---
void WINAPI LibObjInterfaceEntry( int idType, unsigned int comm, void** object ) 
{
  if ( object ) 
  {
    if ( comm == 0 && idType == idd_ILibPropertyObject3D ) // ���� �� Hot ����� ��������
    { 
      Shpeel * obj = new Shpeel();
      obj->LtQueryInterface( idType, object );
    }
  }
}  

////-------------------------------------------------------------------------------
//// ������� ��������������� ������ ��� ������ � Hot �������
//// ---
//void WINAPI LibObjInterfaceEntry( int idType, unsigned int comm, void** object ) 
//{
//  if ( object ) 
//  {
//    if ( comm == 0 && idType == idd_ILibPropertyObject3D ) // ���� �� Hot ����� ��������
//    { 
//      Shpeel * obj = new Shpeel();
//      obj->LtQueryInterface( idType, object );
//    }
//  }
//}  
//



bool Shpeel::CheckDir(CString lib)
//sourse_proj ����� ������ ������� 
{
    CString filename;
    const size_t LENG_BUF = 256;
    WCHAR work_patch[LENG_BUF];
    LPCTSTR p_wp = work_patch;
    IDocument3DPtr pDocument3d(ksGetActive3dDocument(), false);

    filename = pDocument3d->GetFileName();
    
    std::filesystem::path dir_patch;
    {
        const size_t newsizew = (filename.GetLength() + 1) * 2;
        wchar_t* n2stringw = new wchar_t[newsizew];
        wcscpy_s(n2stringw, newsizew, filename);
        dir_patch = std::filesystem::path(n2stringw);
        delete[]n2stringw;
    }
    dir_patch.remove_filename();
#if DEBUG_CH_DIR
    LibMessage(dir_patch.c_str());
#endif // DEBUG_CH_DIR

    CString lib_patch(CString(dir_patch.c_str()) + lib);
    {
        const size_t newsizew = (lib_patch.GetLength() + 1) * 2;
        wchar_t* n2stringw = new wchar_t[newsizew];
        wcscpy_s(n2stringw, newsizew, lib_patch);
        dir_patch = std::filesystem::path(n2stringw);
        delete[]n2stringw;
    }

#if DEBUG_CH_DIR
    LibMessage(dir_patch.c_str());
#endif // DEBUG_CH_DIR

    std::filesystem::directory_entry dir_en(dir_patch);

    if (dir_en.exists())
    {
        if (dir_en.is_directory())
        {
#if DEBUG_CH_DIR
            LibMessage(L"Dir exsist");
#endif // DEBUG_CH_DIR
            return true;
        }
        else
        {
#if  DEBUG_CH_DIR
            LibMessage(L"Dir not exsist");
#endif // DEBUG_CH_DIR
            return false;
        }

        return false;
    }
    else
    {

#if  DEBUG_CH_DIR
        LibMessage(L"Dir not exsist");
#endif // DEBUG_CH_DIR

        return false;
    }

    return true;
}

bool Shpeel::_upload_list(ksAPI7::IPropertyListPtr& p_property_list, CString lib)
{
#if  DEBUG_CH_DIR
    LibMessage(L"_upload_list");
#endif // DEBUG_CH_DIR
    p_property_list->ClearList();
    CString filename;
    const size_t LENG_BUF = 256;
    WCHAR work_patch[LENG_BUF];
    LPCTSTR p_wp = work_patch;
    IDocument3DPtr pDocument3d(ksGetActive3dDocument(), false);

    filename = pDocument3d->GetFileName();
    std::filesystem::path dir_patch;
    {
        const size_t newsizew = (filename.GetLength() + 1) * 2;
        wchar_t* n2stringw = new wchar_t[newsizew];
        wcscpy_s(n2stringw, newsizew, filename);
        dir_patch = std::filesystem::path(n2stringw);
        delete[]n2stringw;
    }
    dir_patch.remove_filename();
#if DEBUG_CH_DIR
    LibMessage(dir_patch.c_str());
#endif // DEBUG_CH_DIR
    CString lib_patch(CString(dir_patch.c_str()) + lib);
    {
        const size_t newsizew = (lib_patch.GetLength() + 1) * 2;
        wchar_t* n2stringw = new wchar_t[newsizew];
        wcscpy_s(n2stringw, newsizew, lib_patch);
        dir_patch = std::filesystem::path(n2stringw);
        delete[]n2stringw;
    }
    dir_patch += L"\\";
#if  DEBUG_CH_DIR
    LibMessage(dir_patch.c_str());
#endif // DEBUG_CH_DIR
    std::filesystem::directory_iterator it(dir_patch); 
    for (auto const& dir_entry : it)
    {
        CString file(dir_entry.path().filename().wstring().c_str());
        //#if  DEBUG_CH_DIR
        //            LibMessage(file);
        //#endif // DEBUG_CH_DIR
        if (file.Find(L".m3d") >= 0)
        {
            p_property_list->Add((LPCWSTR)file);
        }
    }


    return true;
}
using namespace ksAPI7;

CString get_value_from_list(Shpeel& shpeel, long id_control)
{
    CString str;
    IPropertyControlPtr ctrl;
    _variant_t val_t;
    switch (id_control)
    {

    case ID_CHOSE_DETAIL:
    {

        ctrl = shpeel.curentCollection->GetItem(id_control);
        if (ctrl)
        {
            val_t = ctrl->GetValue();
            str = active_file_patch();
            str += "//"; str += std::wstring(val_t.bstrVal).c_str();

            return str;
        }

        break;
    }
    default:
        break;
    }
}

unsigned int Shpeel::get_order_control(variant_t ID)
{
    for each (auto var in v_info_list)
    {
        if (var.id == ID)
            return var.order_id;
    }
    return -1;
}

#define DEBUG_LOAD_DEFAULT_PANEL 1 
#if DEBUG_LOAD_DEFAULT_PANEL
#define DEBUG_LOAD_DEFAULT_PANEL_ 1 
#endif // DEBUG_GET_VALUE_FROM_LIST

int Shpeel::load_default_panel()
{
    int h = 150, w = 100, z = 20;
#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_
    IPartPtr part = GetPart();
    
#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_
    try
    {
        IEntityPtr p_obj(new IEntityPtr(part->NewEntity(o3d_sketch), false /*AddRef*/));//�������� �������� �� ������ ������� ����� ������ 

    }
    catch (const std::exception&e)
    {
        LibMessage(_T("Test_123"), 0);
    }
#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_

    // ������� ��������� ������� ��������� XOY
    IEntityPtr basePlane(part->GetDefaultEntity(o3d_planeXOY), false /*AddRef*/);
#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_
    ISketchDefinitionPtr sketchDefinition = new ISketchDefinitionPtr(IUnknownPtr((basePlane)->GetDefinition(), false /*AddRef*/));
    IEntityPtr entitySketch; 
    // ��������� ���������� ������
    (sketchDefinition)->SetPlane(basePlane); // ��������� ��������� XOY ������� ��� ������
    //(sketchDefinition)->SetAngle(0);        // ���� �������� ������

#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_
    // �������� �����
    (entitySketch)->Create();

#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_
    // ����� � ����� �������������� ������
    if ((sketchDefinition)->BeginEdit())
    {
        // ������ ����� ����� - �������
        LineSeg(0, 0, w, 0, 1);
        LineSeg(0, h, w, h, 1);
        LineSeg(0, 0, 0, h, 1);
        LineSeg(w, 0, w, h, 1);
        // ����� �� ������ �������������� ������
        (sketchDefinition)->EndEdit();
    }

#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_
    IEntityPtr entityExtrusion(part->NewEntity(o3d_bossExtrusion), false /*AddRef*/);
#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_

#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_

    if (entityExtrusion)
    {

        // ��������� ������� �������� ������������
        IBossExtrusionDefinitionPtr extrusionDefinition(IUnknownPtr(entityExtrusion->GetDefinition(), false /*AddRef*/));
        if (extrusionDefinition)
        {
            // ��������� ���������� �������� ������������

            extrusionDefinition->SetDirectionType(dtNormal);     // ����������� ������������ ( dtNormal	- ������
            // �����������, ��� ������ ������ - ������,
            // dtReverse	- �������� �����������, ��� ������ ������ - ������
            // dtBoth - � ��� �������, dtMiddlePlane �� ������� ��������� )
            // �������� ��������� ������������ � ����� �����������
            extrusionDefinition->SetSideParam(true,               // ����������� ������������ ( TRUE - ������ �����������,
                // FALSE - �������� ����������� )
                etBlind,            // ��� ������������ ( etBlind - ������ �� �������,
                // etThroughAll - ����� ��� ������, etUpToVertexTo - �� ���������� �� �������,
                // etUpToVertexFrom - �� ���������� �� �������, etUpToSurfaceTo - ��
                // ���������� �� �����������, etUpToSurfaceFrom - �� ���������� �� �����������,
                // etUpToNearSurface	- �� ��������� ����������� )
                z,                // ������� ������������
                0,                  // ���� ������
                false);            // ����������� ������ ( TRUE - ����� ������, FALSE - ����� ������ )
            // �������� ��������� ������ ������
            extrusionDefinition->SetThinParam(false,              // ������� ������������ ��������
                0,                  // ����������� ���������� ������ ������
                0,                  // ������� ������ � ������ �����������
                0);                // ������� ������ � �������� �����������
            extrusionDefinition->SetSketch(entitySketch);        // ����� �������� ������������

            // ������� �������� ������������
            entityExtrusion->Create();

            // ��������� ������ �������� � ���������� ��������� �� ��� ��������� - ������ ������ ����������       
            IEntityCollectionPtr entityCollection(part->EntityCollection(o3d_face), false /*AddRef*/);
            entityExtrusion->Update();

        }
        entityExtrusion->Update();
    }

#if DEBUG_LOAD_DEFAULT_PANEL_
    LibMessage(std::to_wstring(__LINE__).c_str(), 0);
#endif // DEBUG_LOAD_DEFAULT_PANEL_
    return 1;
}

#define DEBUG_GET_VALUE_FROM_LIST 0 
#if DEBUG_GET_VALUE_FROM_LIST
#define DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL 1 
#endif // DEBUG_GET_VALUE_FROM_LIST


CString get_value_from_list(PropertyManagerObject& shpeel, long id_control)
{
#if DEBUG_GET_VALUE_FROM_LIST
    LibMessage(_T("DEBUG_GET_VALUE_FROM_LIST:"));
#endif // DEBUG_GET_VALUE_FROM_LIST

    CString str;
    IPropertyControlPtr ctrl;
    _variant_t var_t;
    switch (id_control)
    {

    case ID_CHOSE_DETAIL:
    {
        var_t = shpeel.get_order_control(ID_CHOSE_DETAIL);
        shpeel.curentCollection->get_Item(var_t, &ctrl);
        
//#if DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
//        LibMessage(_T("DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL:")); 
//        LibMessage(ctrl->GetValue().bstrVal);
//#endif // DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
        if (ctrl)
        {
            var_t = ctrl->GetValue();
            str = active_file_patch();
#if DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
            LibMessage(_T("active_file_patch():"));
            LibMessage(str);
#endif // DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
            str += patch_resure_detales;
            str += std::wstring(var_t.bstrVal).c_str();

            return str;
        }
        else
        {
#if DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
            LibMessage(_T("DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL: ����������� ID_CHOSE_DETAIL �������"));
#endif // DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL

        }

        break;
    }
    default:
        break;
    }
    return str;
}

CString get_value_from_list(Shpeel* shpeel, long id_control)
{
#if DEBUG_GET_VALUE_FROM_LIST
    LibMessage(_T("DEBUG_GET_VALUE_FROM_LIST:"));
#endif // DEBUG_GET_VALUE_FROM_LIST

    CString str;
    IPropertyControlPtr ctrl;
    _variant_t var_t;
    switch (id_control)
    {

    case ID_CHOSE_DETAIL:
    {
        var_t = shpeel->get_order_control(ID_CHOSE_DETAIL);
        shpeel->curentCollection->get_Item(var_t, &ctrl);
        
//#if DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
//        LibMessage(_T("DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL:")); 
//        LibMessage(ctrl->GetValue().bstrVal);
//#endif // DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
        if (ctrl)
        {
            var_t = ctrl->GetValue();
            str = active_file_patch();
#if DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
            LibMessage(_T("active_file_patch():"));
            LibMessage(str);
#endif // DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
            str += patch_resure_detales;
            str += std::wstring(var_t.bstrVal).c_str();

            return str;
        }
        else
        {
#if DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL
            LibMessage(_T("DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL: ����������� ID_CHOSE_DETAIL �������"));
#endif // DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL

        }

        break;
    }
    default:
        break;
    }
    return str;
}
