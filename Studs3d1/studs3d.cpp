////////////////////////////////////////////////////////////////////////////////
//
// studs3d.cpp - Крепежный элемент на Visual C++ - шпилька
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxdllx.h>
#include <string>
#include "resource.h"


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

#define show_info(info) set_info(info); show(__LINE__); 

//----------------------------------------------------------------------------------------------
// Вспомогательная функция, перевод значения в строку
// ---
LPCTSTR DoubleToStr( double value ) 
{
  static TCHAR res[20];
  _stprintf_s( res, _T("%g"), value );
  return res;
}


//------------------------------------------------------------------------------
// Вспомогательная функция, загрузить строку из ресурса
// ---
LPTSTR LoadStr( int id ) 
{ 
  static TCHAR buf[512]; 
  // Конвертировать строку в соответствии с текущим словарем
  ksConvertLangStrExT( StepDLL.hModule, id, buf, 512 );
  return buf;
}


//------------------------------------------------------------------------------
// Вспомогательная функция, загрузить строку из ресурса
// ---
LPTSTR _LoadStr( int id ) 
{ 
  static TCHAR buf[512]; 
  // Конвертировать строку в соответствии с текущим словарем
  LoadString( StepDLL.hModule, id, buf, 512 );
  return buf;
}


//------------------------------------------------------------------------------
// Вспомогательная функция, вывод сообщения
// ---
extern int LibMessage( LPCTSTR str, int flags = 0 ) 
{
  int res = 0;

  if ( str && str[0] )                  // Строка передана
  {
    int enabse = IsEnableTaskAccess();  // Проверка доступа
    if ( enabse )                       // Если доступ к задаче разрешен
      EnableTaskAccess(0);              // Запрещаем доступ

    // Текст сообщения  заголовок параметры
    res = MessageBox( (HWND) GetHWindow(), str, LoadStr(IDR_LIBID), flags );

    if ( enabse )                       // Если доступ к задаче был запрещен
      EnableTaskAccess(1);              // Разрешаем доступ к задаче
  }
  return res;
}


//------------------------------------------------------------------------------
// Вспомогательная функция, вывод сообщения
// ---
 int LibMessage( int strId, int flags = 0)
{
  return LibMessage( LoadStr(strId), flags );
}


//------------------------------------------------------------------------------
// Вспомогательная функция, возвращающая информацию об ошибке
// ---
void DumpError(_com_error& e)
{
  _bstr_t bstrSource(e.Source());
  _bstr_t bstrDescription(e.Description());
  CString str;
  str.Format( _T("Номер ошибки= %08lx"),   e.Error());
  str += _T("\nСообщение:");       
  str += e.ErrorMessage();
  str += _T("\nИсточник:");        
  str += (LPCTSTR)bstrSource;
  str += _T("\nОписание ошибки:"); 
  str += (LPCTSTR)bstrDescription;
  LibMessage( str, MB_OK | MB_ICONERROR);	
}


//-------------------------------------------------------------------------------
// Вспомогательная функция, получить доступ к новому API
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

      HINSTANCE hAppAuto = LoadLibrary( filename ); // Идентификатор kAPI7.dll
      if(  hAppAuto ) 
      {
        // Указатель на функцию возвращающую интерфейс KompasApplication  
        typedef LPDISPATCH ( WINAPI *FCreateKompasApplication )(); 
				 
        FCreateKompasApplication pCreateKompasApplication = 
          (FCreateKompasApplication)GetProcAddress( hAppAuto, "CreateKompasApplication" );	
        if ( pCreateKompasApplication )
          pNewKompasAPI = IDispatchPtr( pCreateKompasApplication(), false/*AddRef*/ ); // Получаем интерфейс Application
        FreeLibrary( hAppAuto );  
      }
    }
  }
}

//-------------------------------------------------------------------------------
// Стандартная точка входа
// Инициализация и завершение DLL
// ---
extern "C" int APIENTRY
DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
{
  UNREFERENCED_PARAMETER( lpReserved );

  if ( dwReason == DLL_PROCESS_ATTACH )
  {
    TRACE0( "DLL Initializing!" );
    //////////////////////////////////


    //////////////////////////////////

    if ( !AfxInitExtensionModule( StepDLL, hInstance ) )
      return 0;

    new CDynLinkLibrary( StepDLL );
  }
  else if ( dwReason == DLL_PROCESS_DETACH )
  {

    BaseEvent::TerminateEvents();
    BaseEvent::DestroyList(); // Удалить список подписчиков
    if ( pNewKompasAPI )
      pNewKompasAPI = NULL;    // Освободить Компас

    TRACE0( "DLL Terminating!" );

    AfxTermExtensionModule( StepDLL );
  }
  return 1;
}


//-------------------------------------------------------------------------------
// Задать идентификатор ресурсов
// ---
unsigned int WINAPI LIBRARYID()
{
  return IDR_LIBID;
}


//-------------------------------------------------------------------------------
// Головная функция библиотеки
// ---
void WINAPI LIBRARYENTRY( unsigned int ) 
{
  GetNewKompasAPI();


  IDocument3DPtr pDocument3d( ksGetActive3dDocument(), false/*AddRef*/ ); // Получить указатель на активный документ трехмерной модели
  if ( (bool)pDocument3d && !pDocument3d->IsDetail() )
  {
      try
      {
        Shpeel* shpeel = new Shpeel();
        shpeel->patch_lib = std::filesystem::path(pDocument3d->GetFileName()).c_str();
	    if ( shpeel ) 
        {
          
	      shpeel->Draw3D();
	    }

      }
      catch (const std::exception&e)
      {
          LibMessage((CString(e.what())),0);
      }
     pDocument3d->RebuildDocument();
  }
  else
  {
    // Документ не активизирован или не является 3D-моделью
    ErrorT( LoadStr( IDS_3DDOCERROR ) ); // Выдать сообщение о ошибке
  }

  if ( ReturnResult() == etError10 ) // Вырожденный объект
	ResultNULL();
}


////////////////////////////////////////////////////////////////////////////////
//
// Внекласные функции
//
////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------
// Определяет класс точности по ГОСТу
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
// Определяет класс точности по ГОСТу
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
// Определяет длину ввинчиваемого конца по ГОСТу
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
// Строка по ГОСТу (Файлы *.loa)
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
// Строка по ГОСТу (Название)
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
// Подключение к базе
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
// Присваивает переменной с именем varName значение val
// ---
void SetVarValue( IVariableCollectionPtr& varArr, TCHAR* varName, double val ) 
{
	IVariablePtr var( varArr->GetByName( _bstr_t(varName), true, false), false/*AddRef*/ ); // Текущая переменная
  if ( var )
    var->SetValue( val ); // Сменить значение
}  


//------------------------------------------------------------------------------
// Определяет является ли объект поверхностью ( planar = true - плоскостью,
// planar = false - конической поверхностью )
// ---
bool IsSurface( IEntityPtr entity, bool planar = true, double * = NULL ) 
{
	bool res = false;
// КЕ K8+ после второго двойного клика мы щитаем, что создаем новую шпильку и все время пытаемся мэйтить ее с собой
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
// Определяет является ли объект осью
// ---
bool IsAxis( IEntityPtr entity ) 
{
  if ( entity ) 
  {
    short  type = entity->GetType();
    return type == o3d_axis2Planes              || // Ось по двум плоскостям
           type == o3d_axisOperation            || // Ось операций
           type == o3d_axis2Points              || // Ось по двум точкам
           type == o3d_axisConeFace             || // Ось конической поверхности
           type == o3d_axisEdge                 || // Ось по ребру
           type == o3d_AxisFromPointByDirection || // Ось через точку по направлению
           type == o3d_axisOX                   || // Ось X
           type == o3d_axisOY                   || // Ось Y
           type == o3d_axisOZ                   ;  // Ось Z
  }
  return false;
}


//------------------------------------------------------------------------------
// Чтение БД
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
    shpeel->d  = shpeelTmp.d;  // Диаметр резьбы
    shpeel->p1 = shpeelTmp.p1; // Шаг резьбы 
    shpeel->p2 = shpeelTmp.p2; // Шаг резьбы 
    shpeel->b1 = shpeelTmp.b1; // Ввинчиваемый конец
    shpeel->c  = shpeelTmp.c;  // Размер фаски 
  }

  return res;
}


//------------------------------------------------------------------------------
// Читать параметры стержня
// Возвращает 1 - успех 0 - не найдено записи -1 - ошибка связи с БД
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

  // ALLST - резьба до головки
  if( shpeel->gost > 22041 && !(fabs(shpeel->b) > EPSILON) ) 
  {
    shpeel->f |= ALLST;
    shpeel->f &= ~ISPOLN; // Исполнение 1 становится текущим, тк. исполнение 2 недоступно
  }
  else
    shpeel->f &=~ ALLST;
  
  return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
// Работа с БД
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Открытие БД
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
// Открытие БД
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
// Получить параметры
// ---
int Shpeel::GetParam() 
{
  return m_part ? m_part->GetUserParam( &par, Size() ) : 0;
}


//------------------------------------------------------------------------------
// Конструктор
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

  // Взяли деталь
  m_part = IPartPtr( doc->GetPart( flagMode ? pEdit_Part : pNew_Part ), false/*AddRef*/ );
  partInfo = std::shared_ptr<InsertPart>(new InsertPart) ;
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
// Конструктор копий
// ---
Shpeel::~Shpeel()
{
  ASSERT(!refCount);
  if ( openBase ) // Если БД открыта
    CloseBase();  // Закрываем старую БД
}


//------------------------------------------------------------------------------
// Инициализация
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
  tmp.indexMassa = 0; // 0 - металл 1 - алюмин сплав 3 - бронза 2 - латунь
  tmp.ver        = 1;
  tmp.b1         = 20;
  tmp.klass      = 2; // klass=B
  tmp.l          = 90;
  tmp.b          = 46;
  tmp.m1         = 245.9f;
  tmp.m2         = 228.9f;

}


//------------------------------------------------------------------------------
// При вставке из библиотеки моделей указывается полный путь к файлу библиотеки и путь внутри библиотеки моделей
// ---
CString& Shpeel::GetFileName() 
{        
  static TCHAR fullName[512];
  bool res = false;
  if( GetModuleFileName( StepDLL.hModule, fullName, sizeof(fullName) ) ) //подружает файл stud.l3d
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

//------------------------------------------------------------------------------
// Редактируем переменные
// ---
void Shpeel::SetParam(IPartPtr& pPart )
{
  // Редактируем внешние переменные
    if (!(bool)pPart)
    {
      

    }

}


//------------------------------------------------------------------------------
// Добавить сопряжения
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

  if (p && mCol) // Временное сопряжение
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

        // Добавляем комментарии в коллекцию
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
// Редактирование объекта спецификации
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
  

  ////////////////////////////////////////
  m_part = partInfo->part;
  
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
      // Базовый объект добавляется.
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
 
      // Изменим длину
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
 
      // Изменим ГОСТ
      uBuf = tmp.gost;
      item = columnItems->GetItem( 13 );
      if ( item )
        item->PutValue( uBuf );
 
      float massa = tmp.f & ISPOLN ? tmp.m2 : tmp.m1;
      // 0 - металл 1 - алюмин сплав 3 - бронза 2 - латунь
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
// Отрисовка объекта спецификации 
// ---
bool Shpeel::DrawSpcObj( reference & spcObj ) 
{
  set_info(partInfo->part->GetFileName());

  spcObj = 0;
  if( IsSpcObjCreate() ) 
  {
    if ( ReturnResult() == etError10 ) // Вырожденный объект
      ResultNULL();
    set_info(partInfo->part->GetFileName());

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
// Вывод параметров 
// ---
void Shpeel::ShowParam()
{
    // Перезаполняем грид
  if ( paramGrid ) 
  {
    //rowIndex = 1;

    //// "b(мм) Гаечный конец"
    //AddDoubleToGrig( ID_PROP_WRENCH_ENDING, tmp.b ); 
    //// "b0(мм) Ввинчиваемый конец"
    //AddDoubleToGrig( ID_PROP_SCREW_ENDING, tmp.b1 ); 
    //// "p(мм) Шаг резьбы"
    //float step = !(tmp.f & PITCHOFF || !(tmp.f & PITCH)) ? tmp.p1 : tmp.p2;
    //AddDoubleToGrig( ID_PROP_PITCH, step ); 
    //// "c(мм) Фаска"
    //AddDoubleToGrig( ID_PROP_FACET, tmp.c ); 
    //// "m(кг) Масса 1000 шт"
    //float massa = tmp.f & ISPOLN ? tmp.m2 : tmp.m1;
    //// 0 - металл 1 - алюмин сплав 3 - бронза 2 - латунь
    //massa = massa*( !tmp.indexMassa ? 1 : tmp.indexMassa==1 ? 0.356f : tmp.indexMassa==3 ? 0.97f : 1.08f );
    //AddDoubleToGrig( ID_PROP_MASSA_1000, massa ); 
  
    //paramGrid->RowCount = rowIndex;
    //paramGrid->UpdateParam();
  }   
}


//-------------------------------------------------------------------------------------
// Идентификатор слайда
// ---
long Shpeel::GetSlideID()
{
  if( tmp.gost > 22041 ) 
  {
    if ( tmp.f & ALLST ) // ALLST - резьба до головки
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
// Изменение значения контрола
// ---
void Shpeel::OnChangeControlValue( long ctrlID, const VARIANT& newVal ) 
{


 switch ( ctrlID ) 
  {
    case IDP_SPC_OBJ: // Объект спецификации
      par.flagAttr = !par.flagAttr;
      break;

    case ID_PROP_GOST: // ГОСТ
    { 
      CString strGost = newVal.bstrVal;
      short gost = (short)_ttoi( strGost.Left(5) );
      if ( gost && gost != tmp.gost ) 
      {


        tmp.gost = gost;
        CloseBase();
        OpenBase();
        if ( ReadShpeelBase( tmp.d, bBase, &tmp ) && ReadShpeelStBase(bBase, &tmp ) ) // Чтение БД
        {

          ksAPI7::IPropertyListPtr diamList( GetPropertyControl(ID_PROP_DIAM) );
          if( diamList )
            FillDiametr( diamList ); // Заполняет список диаметров

          ksAPI7::IPropertyListPtr lenghtList( GetPropertyControl(ID_PROP_LENGHT) );
          if( lenghtList )
            FillLenght( lenghtList ); // Заполняет список длин
        
          ksAPI7::IPropertyListPtr screwEndingList( GetPropertyControl(ID_PROP_SCREW_ENDING_LENGHT) );
          if( screwEndingList )
            // Выделяем в списке 
            screwEndingList->Value =  LoadStr( GetTypeStrForGost( tmp.gost ));

          ksAPI7::IPropertyListPtr classList( GetPropertyControl(ID_PROP_CLASS) );
          if( classList )
            // Выделяем в списке 
            classList->Value = LoadStr( GetClassStrForGost( tmp.gost ) ); 

		      ksAPI7::IPropertyMultiButtonPtr ispButtons( GetPropertyControl(ID_PROP_ISP) );
          if( ispButtons )
          {
            // Проверка возможности исполнения 2
            bool enable = tmp.f & ALLST ? false : true; // ALLST - резьба до головки
            ispButtons->ButtonEnable [ IDP_BMP_G_I2 ] = enable;
            if( !enable )
              ispButtons->ButtonChecked[ IDP_BMP_G_I1 ] = !(tmp.f & ISPOLN ); // Исполнение 1 становится текущим, тк. исполнение 2 недоступно
          }

          // Вывод параметров в гриде ( ГОСТ шпильки - новые параметры )
          ShowParam(); 

          // Перерисовать фантом и слайд
          SetChanged(); 

        }
      }
      break;
    }      

    case ID_PROP_DIAM: // Диаметр 
    { 
      float diam = (float)newVal.dblVal; 
      if ( fabs(diam - tmp.d ) > EPSILON ) 
      {
        if ( ReadShpeelBase( diam, bBase, &tmp ) && ReadShpeelStBase(bBase, &tmp ) ) // Чтение БД
        {
		      ksAPI7::IPropertyListPtr lenghtList( GetPropertyControl(ID_PROP_LENGHT) );
          if( lenghtList )
            FillLenght( lenghtList ); // Заполняет список длин
          
		      ksAPI7::IPropertyMultiButtonPtr ispButtons( GetPropertyControl(ID_PROP_ISP) );
          if( ispButtons )
          {
            // Проверка возможности исполнения 2
            bool enable = tmp.f & ALLST ? false : true; // ALLST - резьба до головки
            ispButtons->ButtonEnable [ IDP_BMP_G_I2 ] = enable;
            if( !enable )
              ispButtons->ButtonChecked[ IDP_BMP_G_I1 ] = !(tmp.f & ISPOLN ); // Исполнение 1 становится текущим, тк. исполнение 2 недоступно
          }

          // Вывод параметров в гриде ( Диаметр шпильки - новые параметры )
          ShowParam();
          // Перерисовать фантом и слайд
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

	
    case ID_PROP_LENGHT: // Длина шпильки 
    { 
      float lenght = (float)newVal.dblVal; 
      if ( fabs(lenght - tmp.l ) > EPSILON ) 
      {
        tmp.l = lenght;

        // Перерисовать фантом и слайд
        SetChanged(); 
      }
      break;
    }
    case ID_PROP_SCREW_ENDING_LENGHT: // Длина ввинчиваемого конца шпильки 
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
              // Выделяем в списке 
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
    case ID_PROP_CLASS: // Класс точности шпильки 
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
                    // Выделяем в списке 
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
    case ID_PROP_MATERIAL: // Материал шпильки  
    {
      CString strMaterial = newVal.bstrVal;    
      for ( int i = 0; i < 4; i++ )
      {
        if( strMaterial == LoadStr( IDS_MATERIAL_STEEL + i ) )  
        {
          if( i != tmp.indexMassa )
          {
            tmp.indexMassa = i;

            // Вывод параметров в гриде ( Масса )
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
// Обработка нажатия кнопок
// ---
void Shpeel::OnButtonClick( long buttonID ) 
{ 
  switch ( buttonID ) 
  { 
    case IDP_BMP_G_I1: // Исполнение 1       
      tmp.f &= ~ISPOLN;

      // Вывод параметров в гриде ( Исполнение влияет на массу ) 
      ShowParam(); 
      // Перерисовать фантом и слайд
      SetChanged(); 

      break;
       
    case IDP_BMP_G_I2: // Исполнение 2
      tmp.f |= ISPOLN; 

      // Вывод параметров в гриде ( Исполнение влияет на массу ) 
      ShowParam(); 
      // Перерисовать фантом и слайд
      SetChanged(); 

      break;

    case ID_PROP_G_STEP : // Шаг 
    { 
      tmp.f & PITCH ? tmp.f &= ~PITCH : tmp.f |= PITCH;

      // Вывод параметров в гриде ( Шаг резьбы ) 
      ShowParam();

      break; 
    }
    case ID_SIMPLE : // Упрощенно         
    {
      tmp.f & SIMPLE ? tmp.f &= ~SIMPLE : tmp.f |= SIMPLE;
      
      // Перерисовать фантом и слайд
      SetChanged();       
      
      break;
    }

    case pbEnter: // Вставка детали в сборку и окончание процесса
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
        m_process3D->RunTakeCreateObjectProcess(prPoint3D, obj, false, lost); // Запуска подпроцесса создания 3D- точки
      }
      break;
    }
    case ID_REBUILD_DETAIL:
    {
        
        IDocument3DPtr corDoc = ksGetActive3dDocument();
        //show_info(FilePatchName);
        //IDocument3DPtr mDoc(ksGet3dDocument());
        //IPartPtr part = m_part->GetPart(pTop_Part);

        dPart->SetActive();
        IPartPtr part = dPart->GetPart(pTop_Part);
        // Создадим новый эскиз
        auto col = part->EntityCollection(o3d_sketch);
        IEntityPtr entitySketch(col->GetByIndex(0), false /*AddRef*/);
        if (entitySketch)
        {

            // Получить указатель на интерфейс параметров объектов и элементов
            ISketchDefinitionPtr sketchDefinition(IUnknownPtr(entitySketch->GetDefinition(), false /*AddRef*/));
            if (sketchDefinition)
            {
                // Получим интерфейс базовой плоскости XOY
                IEntityPtr basePlane(part->GetDefaultEntity(o3d_planeXOY), false /*AddRef*/);

                // Установка параметров эскиза
                sketchDefinition->SetPlane(basePlane); // Установим плоскость XOY базовой для эскиза 

                // Создадим эскиз
                entitySketch->Create();
                //IPropertyControlPtr h = new IPropertyControlPtr(curentCollection->Item[2],false);
                //IPropertyControlPtr w = new IPropertyControlPtr(curentCollection->Item[3],false);

                //VARIANT h_var; h_var.intVal = ID_H_3D_PLATE;
                //VARIANT w_var; w_var.intVal = ID_W_3D_PLATE;
                //VARIANT z_var; z_var.intVal = ID_Z_3D_PLATE;
                // Войти в режим редактирования эскиза
                if (sketchDefinition->BeginEdit())
                {
                    ClearCurrentSketch(); part->ClearAllObj();
                    RectangleParam* rP = new RectangleParam;
                    rP->height = (h);
                    rP->width = (w);
                    rP->ang = 0;
                    rP->x = 0;
                    rP->y = 0;
                    rP->style = 1;

                    ksRectangle(rP, 0);
                    // Выйти из режима редактирования эскиза
                    sketchDefinition->EndEdit();
                }

                // Оперция выдавливани
                IEntityCollectionPtr  boss_coll = part->EntityCollection(o3d_bossExtrusion);
                IEntityPtr entityExtrusion;
                if (boss_coll->GetCount() > 0)
                {
                    for (size_t i = 0; i < boss_coll->GetCount(); i++)
                    {


                    }
                    entityExtrusion = boss_coll->GetByIndex(0);

                }
                else
                {

                    entityExtrusion = part->NewEntity(o3d_bossExtrusion);

                }


                if (entityExtrusion)
                {
                    // Интерфейс базовой операции выдавливания     

                    //IBossExtrusionDefinitionPtr extrusionDefinition(basePlane);
                    IBossExtrusionDefinitionPtr extrusionDefinition((entityExtrusion->GetDefinition()));


                    if (entityExtrusion)
                    {
                        // Интерфейс базовой операции выдавливания
                        IBossExtrusionDefinitionPtr extrusionDefinition(IUnknownPtr(entityExtrusion->GetDefinition(), false /*AddRef*/));
                        if (extrusionDefinition)
                        {
                            // Установка параметров операции выдавливания
                            extrusionDefinition->SetDirectionType(dtNormal);     // Направление выдавливания ( dtNormal	- прямое
                            // направление, для тонкой стенки - наружу,
                            // dtReverse	- обратное направление, для тонкой стенки - внутрь
                            // dtBoth - в обе стороны, dtMiddlePlane от средней плоскости )
                            // Изменить параметры выдавливания в одном направлении
                            extrusionDefinition->SetSideParam(true,               // Направление выдавливания ( TRUE - прямое направление,
                                // FALSE - обратное направление )
                                etBlind,            // Тип выдавливания ( etBlind - строго на глубину,
                                // etThroughAll - через всю деталь, etUpToVertexTo - на расстояние до вершины,
                                // etUpToVertexFrom - на расстояние за вершину, etUpToSurfaceTo - на
                                // расстояние до поверхности, etUpToSurfaceFrom - на расстояние за поверхность,
                                // etUpToNearSurface	- до ближайшей поверхности )
                                z,                // Глубина выдавливания
                                0,                  // Угол уклона
                                false);            // Направление уклона ( TRUE - уклон наружу, FALSE - уклон внутрь )
                            // Изменить параметры тонкой стенки
                            extrusionDefinition->SetThinParam(false,              // Признак тонкостенной операции
                                0,                  // Направление построения тонкой стенки
                                0,                  // Толщина стенки в прямом направлении
                                0);                // Толщина стенки в обратном направлении
                            extrusionDefinition->SetSketch(entitySketch);        // Эскиз операции выдавливания

                            // Создать операцию выдавливания
                            entityExtrusion->Create();

                            // Формирует массив объектов и возвращает указатель на его интерфейс - массив граней компонента       
                            IEntityCollectionPtr entityCollection(part->EntityCollection(o3d_face), false /*AddRef*/);
                            entityExtrusion->Update();

                        }
                        entityExtrusion->Update();

                    }
                }
            }
        }

        part->Update();
        dPart->Save();
        dPart->RebuildDocument();

        m_part->RebuildModel();
        m_part->Update();
        SetChanged();
        RedrawPhantom();
        corDoc->SetActive();

        //IDocument3DPtr partDoc = ksGetActive3dDocument();


        //m_part->BeginEdit();
        //ClearCurrentSketch();
        //RectangleParam * rp = new RectangleParam;
        //rp->x = 0; rp->y = 0; rp->height = 10; rp->width = 2; rp->style = 1; rp->ang = 0;
        //ksRectangle(rp, 0);
        //m_part->EndEdit(TRUE);

        break;
    }
  }
}


//------------------------------------------------------------------------------
// Заполняет список диаметров
// ---
void Shpeel::FillDiametr( SArray<double> & diamList, float & curDiam ) 
{
  if( openBase )
  {
    float lMin         = MAX_LENGTH; // Минимальное значение в списке
    float lMax         = MIN_LENGTH; // Максимальное значение в списке
    bool  enterInRange = false;      // Текущее значение не входит в новые значения

    ShpeelTmp2 tmpD;
    memset( &tmpD, 0, sizeof(tmpD) );

    if ( ConditionT(bBase.bg, bBase.rg1, _T("t = 1")) ) 
    {
      int i = 1;
      while( i ) // Просматриваем все записи базы данных
      {                                 
        i = ReadRecord( bBase.bg, bBase.rg1, &tmpD ); // Считываем данное
        if ( i ) // Если запись найдена
        {                                     
          // Заносим его в список
          diamList.Add( tmpD.d );

          if ( tmpD.d == curDiam )
            enterInRange = true; // Текущее значение диаметра входит в новые значения
          if ( tmpD.d < lMin )
            lMin = tmpD.d;       // Находим минимальное значение
          if ( tmpD.d > lMax )                          
            lMax = tmpD.d;       // Находим максимальное значение
        }
      }
      if ( !enterInRange )  // Если текущее значение диаметра не входит в в новые значения
        curDiam = lMin;     // Присваиваем диаметру минимальное значение
      if ( curDiam < lMin ) // Если диаметр выходит за минимальную границу новых значений
        curDiam = lMin;     // Присваиваем ему минимальное значение
      if ( tmp.d > lMax )   // Если диаметр выходит за максимальную границу новых значений
        curDiam = lMax;     // Присваиваем ему максимальное значение
    }
  } 
}


//------------------------------------------------------------------------------
// Заполняет список диаметров
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
// Заполняет список длин
// ---
void Shpeel::FillLenght( SArray<double> & lenList, float & curLen )
{
  if( openBase )
  {
    float lMin         = MAX_LENGTH; // Минимальное значение в списке
    float lMax         = MIN_LENGTH; // Максимальное значение в списке
    bool  enterInRange = false;      // Текущее значение не входит в новые значения

    ShpeelTmp3 tmpL;
    memset( &tmpL, 0, sizeof(tmpL) ); 

    TCHAR buf[128];
    _stprintf_s( buf, _T("t = 1 && d=%.1f"), tmp.d );

    if ( ConditionT(bBase.bs, bBase.rs1, buf) ) 
    {
      int i = 1;
      while( i ) // Просматриваем все записи базы данных
      {    
        i = ReadRecord( bBase.bs, bBase.rs1, &tmpL ); // Считываем данное
        if ( i ) // Если запись найдена
        {      
          // Заносим его в список
          lenList.Add( tmpL.L ); // Заносим его в список

          if ( tmpL.L == curLen )
            enterInRange = true; // Текущее значение диаметра входит в новые значения
          if ( tmpL.L < lMin )
            lMin = tmpL.L;       // Находим минимальное значение
          if ( tmpL.L > lMax )                          
            lMax = tmpL.L;       // Находим максимальное значение
        }
      }
      if ( !enterInRange ) // Если текущее значение диаметра не входит в в новые значения
        curLen = lMin;      // Присваиваем диаметру минимальное значение
      if ( curLen < lMin )  // Если диаметр выходит за минимальную границу новых значений
        curLen = lMin;      // Присваиваем ему минимальное значение
      if ( curLen > lMax )  // Если диаметр выходит за максимальную границу новых значений
        curLen = lMax;      // Присваиваем ему максимальное значение
    }
  }
}


//------------------------------------------------------------------------------
// Заполняет список длин
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
// Заполняет список длин ввинчиваемого конца
// ---
void Shpeel::FillScrewEnding( PArray<_bstr_t> & screwEndingList )
{
  for ( int i = 0; i < 6; i++ )
    screwEndingList.Add( new _bstr_t(LoadStr( IDS_STUD_1D + i )) );
}

//-------------------------------------------------------------------------------
// Заполняет список длин ввинчиваемого конца
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
// Заполняет список длин ввинчиваемого конца
// ---
void Shpeel::FillClass( PArray<_bstr_t> & classList )
{
  for ( int i = 0; i < 2; i++ )
    classList.Add( new _bstr_t(LoadStr( IDS_STUD_A + i )) );
}

//-------------------------------------------------------------------------------
// Заполняет список класса точности 
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
// Заполняет список ГОСТов
// ---
void Shpeel::FillGost( PArray<_bstr_t>  & gostList )
{
  for ( int i = 0; i < 12; i++ )
    gostList.Add( new _bstr_t(LoadStr( STR_101 + i )) );  
}

//-------------------------------------------------------------------------------
// Заполняет список ГОСТов
// ---
void Shpeel::FillGost(ksAPI7::IPropertyListPtr & gostList )
{
  PArray<_bstr_t> list;
  FillGost(list);
  _variant_t values;
  FillBSTRSafeArray(values, list);
  gostList->Add( values );
  // Выделяем в списке 
  gostList->Value = LoadStr(GetGostStr(tmp.gost));
}


//-------------------------------------------------------------------------------
// Определяет материал по indexMassa
// 0 - металл 1 - алюмин сплав 2 - латунь 3 - бронза
// ---
static short GetMaterialStr( short indexMassa )
{
  short n = 0;
  if( indexMassa >= 0 && indexMassa <= 3 )
    n = IDS_MATERIAL_STEEL + indexMassa;
  return n;
}

//-------------------------------------------------------------------------------
// Заполняет список ГОСТов
// ---
void Shpeel::FillMaterial( PArray<_bstr_t>  & materialList )
{
  for ( int i = 0; i < 4; i++ )
    materialList.Add( new _bstr_t(LoadStr( IDS_MATERIAL_STEEL + i )) );  
}


//-------------------------------------------------------------------------------
// Заполняет список материалов  
// ---
void Shpeel::FillMaterial(ksAPI7::IPropertyListPtr & materialList )
{
  PArray<_bstr_t> list;
  FillMaterial(list);
  _variant_t values;
  FillBSTRSafeArray(values, list);
  materialList->Add( values );
  // Выделяем в списке 
  materialList->Value = LoadStr(GetMaterialStr(tmp.indexMassa));
}


//-------------------------------------------------------------------------------
// Наполнить панель контролами
// ---
void Shpeel::ShowControls() 
{
  if ( curentCollection )
  {
      ksAPI7::IPropertyListPtr chose_ditail = curentCollection->Add(ksControlListStr);
      chose_ditail->Name = _T("Текущая деталь");
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

      //Комбобокс сбора размера панели 
      ksAPI7::IPropertyEditPtr edit_h = curentCollection->Add(ksControlEditReal);
      edit_h->Name = _T("Высота");
      edit_h->Id = ID_H_3D_PLATE;
      edit_h->Value = 100;
      ksAPI7::IPropertyEditPtr edit_w = curentCollection->Add(ksControlEditReal);
      edit_w->Name = _T("Ширина");
      edit_w->Id = ID_W_3D_PLATE;
      edit_w->Value = 100;

      ksAPI7::IPropertyEditPtr edit_z = curentCollection->Add(ksControlEditReal);
      edit_z->Name = _T("Толщина");
      edit_z->Id = ID_Z_3D_PLATE;
      edit_z->Value = 20;


      ksAPI7::IPropertyControlPtr rotate= curentCollection->Add(ksControlTextButton);
      rotate->Name = _T("Повернуть");
      rotate->Id = ID_ROTATE_DETAIL;


      ksAPI7::IPropertyControlPtr rebuild= curentCollection->Add(ksControlTextButton);
      rebuild->Name = _T("Перестроить");
      rebuild->Id = ID_REBUILD_DETAIL;
      

      //ksAPI7::IPropertyControlPtr rebuild = curentCollection->Add(ksControlEditLength
      //);
      //rebuild->Name = _T("Перестроить");
      //rebuild->Id = ID_W_RESIZE_DETAIL;

      //ksAPI7::IPropertyControlPtr angle = curentCollection->Add(ksControlEditAngle

      //);
      //angle->Name = _T("angle");
      //angle->Id = ID_ANGLE_ROTATE_DETAIL;
  }
  //// Комбобокс ГОСТа
  //ksAPI7::IPropertyListPtr gostList( CreateStringList() );
  //if ( gostList ) 
  //{
  //  InitPropertyControl( gostList, ID_PROP_GOST, IDS_GOST_TIPS, IDS_GOST_TIPS );
  //  FillGost( gostList ); // Заполняет список ГОСТов
  //}
  //
  //if ( openBase ) // Если БД открыта
  //  CloseBase();  // Закрываем старую БД
  //// Открываем новую БД и считываем текущие параметры
  //if ( OpenBase() )
  //{ 
  //  // Комбобокс диаметров резьбы
  //  ksAPI7::IPropertyListPtr diamList( CreateRealList() );
  //  if ( diamList ) 
  //  {
  //    InitPropertyControl( diamList, ID_PROP_DIAM, IDS_DIAM_TIPS, IDS_DIAM_TIPS );
  //    FillDiametr( diamList ); // Заполняет список диаметров
  //  }
  //  // Комбобокс длин шпильки
  //  ksAPI7::IPropertyListPtr lenghtList( CreateRealList() );
  //  if ( lenghtList ) 
  //  {
  //    InitPropertyControl( lenghtList, ID_PROP_LENGHT, IDS_LENGHT_TIPS, IDS_LENGHT_TIPS );
  //    FillLenght( lenghtList ); // Заполняет список длин
  //  }
  //  // Комбобокс длин ввинчиваемого конца
  //  ksAPI7::IPropertyListPtr screwEndingList( CreateStringList() );
  //  if ( screwEndingList ) 
  //  {
  //    InitPropertyControl( screwEndingList, ID_PROP_SCREW_ENDING_LENGHT, IDS_SCREW_ENDING_TIPS, IDS_SCREW_ENDING_TIPS );
  //    FillScrewEnding( screwEndingList ); // Заполняет список длин ввинчиваемого конца
  //  }
  //}
  //// Комбобокс класса точности
  //ksAPI7::IPropertyListPtr classList( CreateStringList() );
  //if ( classList ) 
  //{
  //  InitPropertyControl( classList, ID_PROP_CLASS, IDS_CLASS_TIPS, IDS_CLASS_TIPS );
  //  FillClass( classList ); // Заполняет список класса точности
  //}

  // Комбобокс материала
  ksAPI7::IPropertyListPtr materialList( CreateStringList() );
  if ( materialList ) 
  {
    InitPropertyControl( materialList, ID_PROP_MATERIAL, IDS_MATERIAL_TIPS, IDS_MATERIAL_TIPS );
    FillMaterial( materialList ); // Заполняет список материалов
  }
  
  //CreateSeparator(ksSeparatorDownName);

  //// Исполнение
  //ksAPI7::IPropertyMultiButtonPtr ispButtons( CreateMultiButton(ksRadioButton) );
  //if ( ispButtons ) 
  //{
  //  InitPropertyControl( ispButtons, ID_PROP_ISP, IDS_ISP_HINT, IDS_ISP_TIPS, TRUE, ksNameVerticalVisible );
  //  
  //  // Кнопка исполнение 1
  //  AddButton2( ispButtons, IDP_BMP_G_I1, IDF_BMP_G_I1, !(tmp.f & ISPOLN ) );
  //  // Проверка возможности исполнения 2
  //  bool enable = tmp.f & ALLST ? false : true; // ALLST - резьба до головки
  //  // Кнопка исполнение 2
  //  AddButton2( ispButtons, IDP_BMP_G_I2, IDF_BMP_G_I2, !!(tmp.f & ISPOLN ), enable );
  //}
  //CreateSeparator(ksSeparatorDownName);
  //// Дополнительные параметры
  //ksAPI7::IPropertyMultiButtonPtr paramButtons( CreateMultiButton(ksCheckButton) );
  //if ( paramButtons ) 
  //{
  //  InitPropertyControl( paramButtons, IDP_FLAGS, IDS_FLAGS_TIPS, IDS_FLAGS_TIPS, TRUE, ksNameVerticalVisible );
  //  // Шаг мелкий
  //  AddButton2( paramButtons, ID_PROP_G_STEP, IDF_PROP_G_STEP, !!(tmp.f & PITCH), !(tmp.f & PITCHOFF) );
  //  // Упрощенно
  //  AddButton2( paramButtons, ID_PROP_SIMPLE, IDF_PROP_SIMPLE, !!(tmp.f & SIMPLE), true );
  //}
  //// Создавать объект спецификации
  //if ( IsSpcObjCreate() ) 
  //{
  //  ksAPI7::IPropertyCheckBoxPtr checkBox( CreateCheckBox(!!par.flagAttr) );
  //  if ( checkBox ) 
  //    InitPropertyControl( checkBox, IDP_SPC_OBJ, IDS_SPC_OBJ_TIPS, IDS_SPC_OBJ_TIPS );
  //}
  //CreateTab( ID_PROP_ELEMENT_PARAM );
}


//-------------------------------------------------------------------------------------
// Перерисовать фантом, слайд, вывести новые параметры  
// ---
void Shpeel::RedrawPhantom()
{
  if( Changed() )
  {
    // Изменения фантома
    if ( m_part )
    {
      SetParam( m_part ); 
      m_part->Update();
    }
    
    // Перерисовываем слайд в окне
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
// Перерисовать фантом, слайд, вывести новые параметры  
// ---
void Shpeel::get_part()
{

}

//-------------------------------------------------------------------------------------
// Рисование 3D объектов
// ---
void Shpeel::Draw3D()
{
  if (doc && m_part) // Если есть документ и в классе и деталь 
  {
    long toolBarID  = IDP_STUDS;
    long firstTabID = IDP_STUDS;
    _bstr_t bstr;
    if (flagMode)  //получение флагов 
      GetParam();
    else
    {

      /*LibMessage((L"Studs3d:" + std::to_wstring(__LINE__)).c_str());
      bstr = GetFileName();
      m_part->SetFileName(bstr);*/
    }

    load_default_panel();
    IDocument3DPtr corrent_doc(ksGetActive3dDocument());
    //auto cor_dir = std::filesystem::path(corrent_doc->GetFileName()).remove_filename().wstring();
    //cor_dir += patch_resure_detales;
    //cor_dir += _T("Tmp.m3d");

    
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
          ksAPI7::IManipulatorsPtr manColl( m_process3D->Manipulators ); // Получаем коллекцию манипуляторов процесса
          if ( manColl )
          {

            m_baseMan = manColl->Add(ksPlacement3DManipulator); // Добавляем новый манипулятор в коллекцию 
            m_manipulator = m_baseMan;
            if (  m_manipulator )
            {

                m_baseMan->Id = IDR_BASEMANID;
              m_baseMan->Active  =  true;
              m_baseMan->Visible = false;
          
              // Определение видимости определенных примитивов манипулятора
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
  set_info(partInfo->part->GetFileName());

}


//------------------------------------------------------------------------------
// Функция фильтр
// ---
BOOL Shpeel::FilterObject( LPDISPATCH entity ) 
{
  bool res = false;  // Признак неподходящего объекта
  ksAPI7::IModelObject1Ptr Model1( entity );

  if( !Model1->GetIsExternalObject())  // Используем только внутренние объекты
  {
    IEntityPtr entity1(IUnknownPtr(ksTransferInterface(entity, ksAPI3DCom, o3d_entity),false));
    if (IsSurface(entity1) || IsSurface(entity1, false) || IsAxis(entity1)) 
      res = true;
  }
  return res; 
}


//-------------------------------------------------------------------------------------
// ksEndProcess    -Окончание процесса.
// ---
BOOL Shpeel::EndProcess()
{
  if ( fixingPart ) // Закрепляем деталь
  {
    //set_info("End process: зарепление детали ");
    //show(__LINE__);
    SetParam(m_part);
    if ( !flagMode )
    {
      //_bstr_t fileName = GetFileName();
      auto fileName = partInfo->part;
     
      doc->SetPartFromFile(fileName->GetFileName(), m_part, false );
    }
    m_part->UpdatePlacement();  // Изменение положения детали

    PArray<ksAPI7::IModelObjectPtr> R;

    FillObjects( Objects, R );

    for ( unsigned int Idx=0; Idx<R.Count(); Idx++ )
    {
      IEntityPtr next = IUnknownPtr(ksTransferInterface(*R[Idx], ksAPI3DCom, o3d_entity), false);
    // Создаем постоянное сопряжение в зависимости от поверхности
      if (IsSurface(next))
        doc->AddMateConstraint (mc_Coincidence, next , GetEntityByName(_T("Plane"), m_part), GetDirectionElem(), 2, 0);
      
      if (IsSurface(next, false))
        doc->AddMateConstraint (mc_Concentric , next , GetEntityByName(_T("Axis"),  m_part), 0, 2, 0);
      
      if (IsAxis(next))
        doc->AddMateConstraint(mc_Concentric , next , GetEntityByName(_T("Axis"),  m_part), 0, 2, 0);
    }
    // Устанавливаем признак, стандартное изделие
    m_part->SetStandardComponent(true);


    reference spcObj;

    if (DrawSpcObj(spcObj)) // Вывод объекта СП
    {
      // Редактируем параметры
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
// ksEndProcess    -Событие создания объекта в - --подчиненном режиме.
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
// ksPlacementChanged    -Изменено положения объекта
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

  if ( ModelObject && FilterObject( Object ) ) // Если поверхность под курсором подходит 
  {
    if ( ChooseMan->IsChoosen(ModelObject) )   // Проверяем, выбрана ли поверхность 
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

    ksAPI7::IPlacement3DPtr newPlace = m_process3D->Placement;  // Местоположение курсора при клике
    if (  newPlace )
    {
      double x = 0.0, y = 0.0, z = 0.0;

      newPlace->GetOrigin( &x, &y, &z );     // Получим координаты 

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
          m_baseMan->UpdatePlacement( TRUE );  // Обновим местоположение манипулятора
        }
        m_baseMan->Visible = TRUE;         // Манипулятор виден 
      }

      InitPoint3D( true/*visible*/, true/*fix*/ );

      procParam->DefaultControlFix=ksAllFix; // Зафиксируем объекты

    }
  }
  return TRUE;
}

//-------------------------------------------------------------------------------------
//Создать временное сопряжение в соответствии с указанной поверхностью
// ---
int Shpeel::MateProcess(LPDISPATCH entity)
{
  ksAPI7::IMateConstraints3DPtr mateCollection(m_process3D->MateConstraints);

  IEntityPtr entity1(IUnknownPtr(ksTransferInterface(entity, ksAPI3DCom, o3d_entity), false));

  m_mCol=IUnknownPtr(ksTransferInterface(mateCollection, ksAPI3DCom, o3d_mateConstraintCollection), false);  

  // Создание временного сопряжение в зависимости от поверхности 
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
//Инициализация 3D-точки
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
// Находит у детали поверхность с заданным именем
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
// Имя группы объектов по умолчанию макроэлементы
// ---
LPOLESTR _IFUNC Shpeel::GetGroupName()
{
  CString res(_T("Панели"));
  return res.AllocSysString();
}

//-----------------------------------------------------------------------------
// Добавить свойство в список свойств
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
        param->propertyType = ksOPControlGroup;      // Группа
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
      case ID_PROP_G_STEP: // Мелкий шаг
      case ID_PROP_SIMPLE: // Упрощенно
      {
        param->propertyType = ksOPControlListCheck;
        break; 
      }
      case ID_PROP_WRENCH_ENDING: // b(мм) Гаечный конец
      case ID_PROP_SCREW_ENDING:  // b0(мм) Ввинчиваемый конец
      case ID_PROP_PITCH:         // p(мм) Шаг резьбы
      case ID_PROP_FACET:         // c(мм) Фаска
      case ID_PROP_MASSA_1000:    // m(кг) Масса 1000 шт
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
// Обновить параметры свойства
// ---
BOOL _IFUNC Shpeel::UpdateProperty( PropertyParam * param )
{
  BOOL res = TRUE;
  if ( param )
  {
    if ( !openBase ) // Если БД открыта
      OpenBase();  // Закрываем старую БД

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
        param->enable = tmp.f & ALLST ? false : true; // ALLST - резьба до головки
        param->emptyValue = !param->enable;
        param->propertyValue.lVal = tmp.f & ISPOLN ? IDP_BMP_G_I2 : IDP_BMP_G_I1; // Исполнение 1 становится текущим, тк. исполнение 2 недоступно
        break;
      }
      case ID_PROP_G_STEP: param->propertyValue.boolVal = tmp.f & PITCH;  break; // Мелкий шаг
      case ID_PROP_SIMPLE: param->propertyValue.boolVal = tmp.f & SIMPLE; break; // Упрощенно

      case ID_PROP_WRENCH_ENDING: param->propertyValue.dblVal = tmp.b;  break; // b(мм) Гаечный конец
      case ID_PROP_SCREW_ENDING:  param->propertyValue.dblVal = tmp.b1; break; // b0(мм) Ввинчиваемый конец
      case ID_PROP_PITCH:         param->propertyValue.dblVal = !(tmp.f & PITCHOFF || !(tmp.f & PITCH)) ? tmp.p1 : tmp.p2; break; // p(мм) Шаг резьбы
      case ID_PROP_FACET:         param->propertyValue.dblVal = tmp.c;  break;// c(мм) Фаска
      case ID_PROP_MASSA_1000:    // m(кг) Масса 1000 шт
      {
        float massa = tmp.f & ISPOLN ? tmp.m2 : tmp.m1;
        // 0 - металл 1 - алюмин сплав 3 - бронза 2 - латунь
        massa = massa*( !tmp.indexMassa ? 1 : tmp.indexMassa==1 ? 0.356f : tmp.indexMassa==3 ? 0.97f : 1.08f );
        param->propertyValue.dblVal = massa;
        break;
      }
    }
  }
  return param && param->propertyId;
}


//-----------------------------------------------------------------------------
// Изменить свойство
// ---
int _IFUNC Shpeel::ApplyProperty( PropertyParam * param )
{
  BOOL res = FALSE;
  bool setParam = TRUE;
  if ( param )
  {
    if ( !openBase ) // Если БД открыта
      OpenBase();  // Закрываем старую БД
    switch ( param->propertyId )
    {
      case ID_PROP_ISP:
      {
        param->propertyValue.intVal == IDP_BMP_G_I1 ? tmp.f &= ~ISPOLN : tmp.f |= ISPOLN;
        res = TRUE;
        break;
      }

      case ID_PROP_G_STEP : // Шаг 
      { 
        param->propertyValue.boolVal ? tmp.f |= PITCH : tmp.f &= ~PITCH;
        res = TRUE;
        break;
      }
      case ID_PROP_SIMPLE : // Упрощенно         
      {
        param->propertyValue.boolVal ? tmp.f |= SIMPLE : tmp.f &= ~SIMPLE;
        res = TRUE;
        break;
      }
      break;
      case ID_PROP_DIAM: // Диаметр 
      { 
        float diam = (float)param->propertyValue.dblVal; 
        if ( fabs(diam - tmp.d ) > EPSILON ) 
          res = ReadShpeelBase( diam, bBase, &tmp ) && ReadShpeelStBase(bBase, &tmp ); // Чтение БД
      }
      case ID_PROP_LENGHT: // Длина шпильки 
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
          setParam = FALSE; // Параметры установлены в ApplyProperty
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
          res = ReadShpeelBase( tmp.d, bBase, &tmp ) && ReadShpeelStBase( bBase, &tmp ); // Чтение БД
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
// Выдать идентификатор свойства по индексу
// ---
int Shpeel::GetPropertyID ( int index )
{
  int res = 0;
  switch ( index )
  {
    case 0  : res = ID_PROP_ELEMENT_PARAM;       break; // Параметры элемента
    case 1  : res = ID_PROP_GOST;                break; // Гост
    case 2  : res = ID_PROP_DIAM;                break; // Диаметр
    case 3  : res = ID_PROP_LENGHT;              break; // Длина
    case 4  : res = ID_PROP_SCREW_ENDING_LENGHT; break; // Длина ввинчиваемого конца
    case 5  : res = ID_PROP_CLASS;               break; // Класс точности
    case 6  : res = ID_PROP_MATERIAL;            break; // Материал
    case 7  : res = ID_PROP_ISP;                 break; // Исполнение
    case 8  : res = ID_PROP_G_STEP;              break; // Мелкий шаг
    case 9  : res = ID_PROP_SIMPLE;              break; // Упрощенно

    case 10 : res = ID_PROP_OTHER_PARAM;         break; // Справочные данные
    case 11 : res = ID_PROP_WRENCH_ENDING;       break; // b(мм) Гаечный конец
    case 12 : res = ID_PROP_SCREW_ENDING;        break; // b0(мм) Ввинчиваемый конец
    case 13 : res = ID_PROP_PITCH;               break; // p(мм) Шаг резьбы
    case 14 : res = ID_PROP_FACET;               break; // c(мм) Фаска
    case 15 : res = ID_PROP_MASSA_1000;          break; // m(кг) Масса 1000 шт
  }
  return res;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// ksRotateManipulator - Поворот манипулятора
// ---
bool Shpeel::RotateManipulator(double X0, double Y0, double Z0, double AxisZX, double AxisZXY, double AxisZZ, double angle, bool FromEdit)
{
  ksAPI7::IPart7Ptr IPart7(IUnknownPtr(ksTransferInterface(m_part, ksAPI7Dual, 0), false));
  ksAPI7::IPlacement3DPtr Place(IPart7->Placement);
  
  Place->Rotate(X0, Y0, Z0, AxisZX, AxisZXY, AxisZZ, angle );
  
  IPart7->UpdatePlacement(true);
  
  ksAPI7::IPlacement3DPtr PlaceBase(m_baseMan->Placement);
  PlaceBase->InitByMatrix3D(Place->GetMatrix3D());  // Установка новой системы координат по матрице 
  m_baseMan->UpdatePlacement(true);

  InitPoint3D(true/*visible*/, true/*fix*/);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// ksMoveManipulator   - Передвижение манипулятора
// ---
bool Shpeel::MoveManipulator(long ManipulatorId, double VX, double VY, double VZ, double Delta, bool FromEdit)
{
  ksAPI7::IPart7Ptr IPart7(IUnknownPtr(ksTransferInterface(m_part, ksAPI7Dual, 0), false));

  ksAPI7::IPlacement3DPtr Place(IPart7->Placement);
 
  ksAPI7::IPlacement3DPtr PlaceBase(m_baseMan->Placement);
  double x = 0.0, y = 0.0, z = 0.0;
  Place->GetOrigin(&x, &y, &z);
  PlaceBase->SetOrigin(VX+x, VY+y, VZ+z);  // Установка новых координат манипулятора с добавлением смещения 
  
  Place->SetOrigin(VX + x, VY + y, VZ + z);
  m_baseMan->UpdatePlacement(true);
  IPart7->UpdatePlacement(true);
  InitPoint3D( true/*visible*/, true/*fix*/ );
  return true;
}


//-------------------------------------------------------------------------------------------
// ksBeginDragManipulator - Начало перетаскивания манипулятора
// ---
bool Shpeel::BeginDragManipulator(long ManipulatorId, long PrimitiveType)
{

  return true;
}


//-------------------------------------------------------------------------------------------
// ksEndDragManipulator - окончание перетаскивания манипулятора
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
// Создает соответствующий объект для работы с Hot точками
// ---
void WINAPI LibObjInterfaceEntry( int idType, unsigned int comm, void** object ) 
{
  if ( object ) 
  {
    if ( comm == 0 && idType == idd_ILibPropertyObject3D ) // Если не Hot точки обнулить
    { 
      Shpeel * obj = new Shpeel();
      obj->LtQueryInterface( idType, object );
    }
  }
}  

////-------------------------------------------------------------------------------
//// Создает соответствующий объект для работы с Hot точками
//// ---
//void WINAPI LibObjInterfaceEntry( int idType, unsigned int comm, void** object ) 
//{
//  if ( object ) 
//  {
//    if ( comm == 0 && idType == idd_ILibPropertyObject3D ) // Если не Hot точки обнулить
//    { 
//      Shpeel * obj = new Shpeel();
//      obj->LtQueryInterface( idType, object );
//    }
//  }
//}  
//



bool Shpeel::CheckDir(CString lib)
//sourse_proj папка сборки проекта 
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
    p_property_list->Add((new_detile_df));
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



int Shpeel::load_default_panel()
{
    ////////////////////////////////////////////////////// TEST BLOCK 
    IDocument3DPtr corrent_doc(ksGetActive3dDocument());
    dPart = IDocument3DPtr(ksGet3dDocument(), false/*AddRef*/);

    //auto hControl = this->GetPropertyControl(ID_H_3D_PLATE);
    //auto wControl = this->GetPropertyControl(ID_W_3D_PLATE);
    //auto zControl = this->GetPropertyControl(ID_Z_3D_PLATE);

    int h = 100/*hControl->Value.intVal*/, w = 100/*wControl->Value.intVal*/, z = 20/*zControl->Value.intVal*/;

    IPartPtr part = m_part;
    part->ClearAllObj();
    try
    {
        if (dPart)
        {
            if (dPart->Create(true,   // Признак режима редактирования документа ( TRUE - невидимый режим, FALSE - видимый режим )
                true)) // Тип документа ( TRUE - деталь, FALSE - сборка ) 
            {
                part = dPart->GetPart(pTop_Part);
                if (part)
                {
                    // Создадим новый эскиз
                    IEntityPtr entitySketch(part->NewEntity(o3d_sketch), false /*AddRef*/);
                    if (entitySketch)
                    {
                        // Получить указатель на интерфейс параметров объектов и элементов
                        ISketchDefinitionPtr sketchDefinition(IUnknownPtr(entitySketch->GetDefinition(), false /*AddRef*/));
                        if (sketchDefinition)
                        {
                            // Получим интерфейс базовой плоскости XOY
                            IEntityPtr basePlane(part->GetDefaultEntity(o3d_planeXOY), false /*AddRef*/);

                            // Установка параметров эскиза
                            sketchDefinition->SetPlane(basePlane); // Установим плоскость XOY базовой для эскиза


                            // Создадим эскиз
                            entitySketch->Create();

                            // Войти в режим редактирования эскиза
                            if (sketchDefinition->BeginEdit())
                            {
                                LineSeg(0, 0, w, 0, 1);
                                LineSeg(0, 0, 0, h, 1);
                                LineSeg(0, h, w, h, 1);
                                LineSeg(w, 0, w, h, 1);

                                sketchDefinition->EndEdit();
                            }

                            // Оперция выдавливани
                            IEntityPtr entityExtrusion(part->NewEntity(o3d_bossExtrusion), false /*AddRef*/);
                            if (entityExtrusion)
                            {
                                // Интерфейс базовой операции выдавливания
                                IBossExtrusionDefinitionPtr extrusionDefinition(IUnknownPtr(entityExtrusion->GetDefinition(), false/*AddRef*/));
                                if (extrusionDefinition)
                                {
                                    // Установка параметров операции выдавливания
                                    extrusionDefinition->SetDirectionType(dtNormal);     // Направление выдавливания ( dtNormal	- прямое
                                                                                           // направление, для тонкой стенки - наружу,
                                                                                           // dtReverse	- обратное направление, для тонкой стенки - внутрь
                                                                                           // dtBoth - в обе стороны, dtMiddlePlane от средней плоскости )
                                    // Изменить параметры выдавливания в одном направлении
                                    extrusionDefinition->SetSideParam(true,               // Направление выдавливания ( TRUE - прямое направление,
                                                                                           // FALSE - обратное направление )
                                        etBlind,            // Тип выдавливания ( etBlind - строго на глубину,
                                                            // etThroughAll - через всю деталь, etUpToVertexTo - на расстояние до вершины,
                                                            // etUpToVertexFrom - на расстояние за вершину, etUpToSurfaceTo - на
                                                            // расстояние до поверхности, etUpToSurfaceFrom - на расстояние за поверхность,
                                                            // etUpToNearSurface	- до ближайшей поверхности )
                                        z,                // Глубина выдавливания
                                        0,                  // Угол уклона
                                        false);            // Направление уклона ( TRUE - уклон наружу, FALSE - уклон внутрь )
     // Изменить параметры тонкой стенки
                                    //extrusionDefinition->SetThinParam(true,               // Признак тонкостенной операции
                                    //    dtBoth,             // Направление построения тонкой стенки
                                    //    10,                 // Толщина стенки в прямом направлении
                                    //    10);               // Толщина стенки в обратном направлении
                                    extrusionDefinition->SetSketch(entitySketch);        // Эскиз операции выдавливания

                                    // Создать операцию выдавливания
                                    entityExtrusion->Create();

                                }
                            }
                        }
                    }
                }

            }
        }

    }
    catch (_com_error& e)
    {
        DumpError(e); // Вывод сообщений о ошибке
    }
    catch (LPCTSTR mes)
    {
        LibMessage(mes, MB_ICONERROR | MB_OK); // Вывод сообщений о ошибке
    }
    auto pPatch = get_tmp_filename_tmp(corrent_doc);

    if (dPart->SetFileName((LPWSTR)(LPCWSTR)pPatch))
    {

    }
    if (dPart->Save())
    {
        //show_info("Save file");
        //show_info(pPatch);
    }
    else
    {
        //show_info("Error save file detile");
/*        show_info(pPatch);*/
    }


    m_part->SetFileName((LPWSTR)(LPCWSTR)pPatch);

    
    save_part_info(part, dPart, pPatch); //// Сохранение инф о детали 
    //show_info(pPatch);
    FilePatchName = pPatch;
    /////////////////////////////////////////////////////

    corrent_doc->SetActive();
    m_part->Update();
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
            if(str.IsEmpty())
            {
                return str;
            }
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
            LibMessage(_T("DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL: Отсутствует ID_CHOSE_DETAIL контрол"));
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
            LibMessage(_T("DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL: Отсутствует ID_CHOSE_DETAIL контрол"));
#endif // DEBUG_GET_VALUE_FROM_LIST_ID_CHOSE_DETAIL

        }

        break;
    }
    default:
        break;
    }
    return str;
}

CString Shpeel::get_tmp_filename_tmp(IDocument3DPtr doc)
{
    auto cor_dir = std::filesystem::path(doc->GetFileName()).remove_filename().wstring();
    std::wstring tmp_name = (L"Tmp");
    std::wstring tmp_ex = (L".m3d");
    int counter = 0;
    cor_dir += patch_resure_detales;
    auto test_file = cor_dir;
    std::error_code ec;
    if (std::filesystem::exists(cor_dir + tmp_name + tmp_ex, ec))
    {
        std::filesystem::path p_name_file_test((test_file + tmp_name + tmp_ex).c_str());

        while (std::filesystem::exists(p_name_file_test, ec))
        {
            tmp_name = L"Tmp";
            if (counter++ < 10000)
            {
                tmp_name += std::to_wstring(counter);
            }
            p_name_file_test = (test_file + tmp_name + tmp_ex).c_str();
            /*CString  name_file_test((test_file + tmp_name + tmp_ex).c_str());*/
            //show_info(p_name_file_test.c_str());
        }
        test_file = p_name_file_test.c_str();
        cor_dir = test_file;
    }
    CString dir_str(cor_dir.c_str());
    //show_info(dir_str);
    
    return dir_str;
}

bool Shpeel::save_part_info(IPartPtr part, IDocument3DPtr doc,CString patch_file)
{
    partInfo->patch = patch_file;
    partInfo->part = part;
    partInfo->doc = doc;
    return 1;
}

//-------------------------------------------------------------------------------
// Удалить все объекты из текущего эскиза
// ---
void ClearCurrentSketch()
{
    // Создаим итератор и удалим все существующие объекты в эскизе
    reference rIterator = CreateIterator(ALL_OBJ, 0);
    if (rIterator)
    {
        reference rObject = MoveIterator(rIterator, 'F'); // Сместить указатель на первый элемент в списке
        // В цикле сместить указатель на следующий элемент в списке пока не дойдем до последнего
        while (rObject)
        {
            // Если объект существует удалить его
            if (ExistObj(rObject))
                DeleteObj(rObject);
            // Следующий элемент в списке
            rObject = MoveIterator(rIterator, 'N');
        }
        DeleteIterator(rIterator); // Удалим итератор
    }
}