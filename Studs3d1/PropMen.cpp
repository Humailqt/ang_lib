////////////////////////////////////////////////////////////////////////////////
//
// Класс для работы с панелью свойств
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"

#ifndef __ATLCONV_H__
#include "ATLCONV.H"
#endif

#ifndef __PROPMEN_H
#include "PropMen.h"
#endif

#ifndef __LIBTOOL_H
#include <libtool.h>
#endif 

#include <string>

#include <objbase.h>
#include <initguid.h> //  -  Definitions for controlling GUID initialization
// Include after compobj.h to enable GUID initialization.  This
//              must be done once per exe/dll.
// After this file, include one or more of the GUID definition files.

#include <libhppar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//
// ---
extern AFX_EXTENSION_MODULE StepDLL;


//------------------------------------------------------------------------------
//
// ---
extern ksAPI7::IApplicationPtr pNewKompasAPI;
extern void GetNewKompasAPI();
extern unsigned int WINAPI LIBRARYID();

//------------------------------------------------------------------------------
//
// ---
#define LIB_HELP _T("Gayka.hlp") // Файл помощи


//------------------------------------------------------------------------------
//
// ---
LPCTSTR DoubleToStr( double value );      // Вспомогательная функция, перевод значения в строку
LPTSTR LoadStr( int id );                 // Вспомогательная функция, загрузить строку из ресурса
int LibMessage( LPCTSTR str, int flags ); // Вспомогательная функция, вывод сообщения
int LibMessage( int strId, int flags );   // Вспомогательная функция, вывод сообщения
void DumpError( _com_error& e );          // Вспомогательная функция, возвращающая информацию об ошибке
void ShowError();                         // Вспомогательная функция, возвращающая информацию об ошибке


//----------------------------------------------------------------------------------------------
// Конструктор
// ---
PropertyManagerObject::PropertyManagerObject() :
  procParam        (NULL), // Параметры процесса
  propTabs         (NULL), // Коллекция закладок
  curentCollection (NULL), // Коллекция контролов 
  paramGrid        (NULL), // Грид для вывода дополнительных параметров элемента 
  slideBox         (NULL), // Окно просмотра 
  rowIndex         (0),    // Индекс строки
  flagMode         (0)     // Признак редактирования по дабл-клику
{ 
}     


//----------------------------------------------------------------------------------------------
// Деструктор
// ---
PropertyManagerObject::~PropertyManagerObject() 
{  
  paramGrid        = NULL; // Грид для вывода дополнительных параметров элемента 
  slideBox         = NULL; // Окно просмотра 
  curentCollection = NULL; // Коллекция контролов 
  propTabs         = NULL; // Коллекция закладок
  procParam        = NULL; // Параметры процесса
  rowIndex         = 0;    // Индекс строки
  flagMode         = 0;    // Признак редактирования по дабл-клику
}     


//----------------------------------------------------------------------------------------------
// Инициализация параметров процесса
// ---
bool PropertyManagerObject::InitProcessParam( long toolBarID, SpecPropertyToolBarEnum toolBarType, long firstTabID ) 
{
  bool res = false;
  GetNewKompasAPI();
  if ( pNewKompasAPI )
  {
    try 
    { 
      procParam = pNewKompasAPI->CreateProcessParam();  // Получаем параметры процессы
      if ( procParam ) 
      {
        new PropertyManagerEvent( procParam, *this );   // Подписываемся на события процесса 
        
        // Получаем интерфейс панели свойств
        procParam->SpecToolbar = toolBarType;           // Тип кнопок на панели свойств  
        _bstr_t tmpBstr( LoadStr( toolBarID ) );
        procParam->Caption     = tmpBstr;               // Устанавливаем заголовок панели свойств 
        procParam->AutoReduce  = !flagMode;             // Автосоздание объекта

        if ( flagMode ) 
        {
          procParam->DefaultControlFix = ksAllFix;
        }

        // Получаем коллекцию закладок панели свойств
        propTabs = procParam->PropertyTabs;                       

        // Создаем закладку параметров элемента
        if ( firstTabID ) 
        {
          CreateTab( firstTabID, TRUE, TRUE );
          // Наполнение закладки контролами для вывода параметров элемента
          ShowControls();
          
          //// Создать окно просмотра
          //slideBox = curentCollection->Add( ksControlSlideBox );            
          //slideBox->SlideType      = ksSlide;                 
          //slideBox->DrawingSlide   = (long)GetSlideID(); 
          //slideBox->ResModule      = (LONG_PTR)StepDLL.hModule;
          //slideBox->Hint           = _T("Hint для слайда");
          //slideBox->Tips           = _T("Tips для слайда");
          //slideBox->Id             = 10000;
          //slideBox->Name           = _T("Окно просмотра");
          //slideBox->NameVisibility = ksNameHorizontalVisible;
        }
   
        int  paramCount = ParamCount();                             // Нужно ли выводить грид ?
        if ( paramCount )
        {
        }
        res = true;
      }
    }
    // Перехват исключений
    catch(_com_error &e)
    {
      DumpError(e); // Вывод сообщений о ошибке
    }    
    catch( LPCTSTR mes ) 
    {
      LibMessage( mes, MB_ICONERROR | MB_OK ); // Вывод сообщений о ошибке
    }    
  }
  return res;
}


//----------------------------------------------------------------------------------------------
// Добавить параметр в грид
// ---
void PropertyManagerObject::AddStringToGrig( long paramID, LPCTSTR value ) 
{
  if ( paramGrid ) 
  {
    paramGrid->CellText[ rowIndex   ][ 1 ] = _bstr_t(value);
    paramGrid->CellText[ rowIndex++ ][ 0 ] = _bstr_t(LoadStr(paramID)); // Не переставлять так задумано
  }
}  


//----------------------------------------------------------------------------------------------
// Добавить параметр в грид
// ---
void PropertyManagerObject::AddDoubleToGrig( long paramID, double value ) 
{
  if ( paramGrid ) 
  {
    paramGrid->CellText[ rowIndex   ][ 1 ] = _bstr_t( DoubleToStr(value) );  // Выводим значение
    paramGrid->CellText[ rowIndex++ ][ 0 ] = _bstr_t( LoadStr(paramID)   );  // Имя параметра
  }
}  


//----------------------------------------------------------------------------------------------
// Создать закладку
// ---
bool PropertyManagerObject::CreateTab( long tabID, BOOL visible, BOOL active ) 
{
  bool res = false;
  if ( tabID ) 
  { 
    if ( propTabs ) 
    {                          
      _bstr_t tmpBstr( LoadStr(tabID) ); 
    ksAPI7::IPropertyTabPtr propTab = propTabs->Add( tmpBstr );
      if ( propTab ) 
      {
        curentCollection = propTab->PropertyControls;
        if ( !visible )
          propTab->Visible = visible;
        propTab->Active = active;
        res = true;
      }
    }
  }
  return res;
}


//----------------------------------------------------------------------------------------------
// Создать комбобокс
// ---
void  PropertyManagerObject::InitPropertyControl( ksAPI7::IPropertyControl* control, long ctrlID, UINT hint, UINT tips, 
                                                  BOOL enable, PropertyControlNameVisibility nameVisibility, 
                                                  BOOL visible ) 
{
  if ( control ) 
  {
    _bstr_t tmpBstr( LoadStr(ctrlID) ); 
    control->Name           = tmpBstr;
    control->Id             = ctrlID;
    tmpBstr = hint ? LoadStr(hint) : _T(""); 
    control->Hint           = tmpBstr;
    tmpBstr = tips ? LoadStr(tips) : _T(""); 
    control->Tips           = tmpBstr;
    control->Enable         = enable;
    control->Visible        = visible;
    control->NameVisibility = nameVisibility;
  }
}


//----------------------------------------------------------------------------------------------
// Создать комбобокс
// ---
ksAPI7::IPropertyListPtr PropertyManagerObject::CreateRealList( double minVal, double maxVal )
{
  ksAPI7::IPropertyListPtr realList( NULL );
  if ( curentCollection ) 
  {
    realList = curentCollection->Add( ksControlListReal );
    if ( realList ) 
    {
      if ( minVal != maxVal )
        realList->SetValueRange( _variant_t(minVal), _variant_t(maxVal) );
      realList->ReadOnly = TRUE;
    }
  }

  return realList;
}


//----------------------------------------------------------------------------------------------
// Добавление комбобокса строковых значений 
// ---
ksAPI7::IPropertyListPtr PropertyManagerObject::CreateStringList()
{
  ksAPI7::IPropertyListPtr stringList( NULL );
  if ( curentCollection ) 
  {
    stringList = curentCollection->Add( ksControlListStr );
    if ( stringList ) 
       stringList->ReadOnly = TRUE;
  }

  return stringList;
}

//----------------------------------------------------------------------------------------------
// Добавление комбобокса целых значений 
// ---
ksAPI7::IPropertyListPtr PropertyManagerObject::CreateIntList()
{
  ksAPI7::IPropertyListPtr intList( NULL );
  if ( curentCollection ) 
  {
    intList = curentCollection->Add( ksControlListInt );
    if ( intList ) 
      intList->ReadOnly = TRUE;
  }

  return intList;
}


//----------------------------------------------------------------------------------------------
// Создать сепаратор
// ---
ksAPI7::IPropertySeparatorPtr PropertyManagerObject::CreateSeparator( SeparatorTypeEnum type )
{
  ksAPI7::IPropertySeparatorPtr rSepar( NULL );
  if ( curentCollection ) 
  {
    rSepar = curentCollection->Add( ksControlSeparator );
    if ( rSepar ) 
    {
      rSepar->SeparatorType = type;
    }
  }
  return rSepar;
}


//----------------------------------------------------------------------------------------------
// Добавление CheckBox-а 
// ---
ksAPI7::IPropertyCheckBoxPtr PropertyManagerObject::CreateCheckBox( bool checked )
{
  ksAPI7::IPropertyCheckBoxPtr check( NULL );
  if ( curentCollection ) 
  {
    check = curentCollection->Add( ksControlCheckBox );
    if ( check ) 
    {
      check->Value = _variant_t( checked );
    }
  }
  return check;
}


//----------------------------------------------------------------------------------------------
// Создать набор кнопок
// ---
ksAPI7::IPropertyMultiButtonPtr PropertyManagerObject::CreateMultiButton( ButtonTypeEnum type )
{
  ksAPI7::IPropertyMultiButtonPtr buttons( NULL );
  if ( curentCollection ) 
  {
    buttons = curentCollection->Add( ksControlMultiButton );
    if ( buttons ) 
    {
      buttons->ButtonsType = type;
      //buttons->ResModule   = (LONG_PTR)StepDLL.hModule;
    }
  }
  return buttons;
}


//----------------------------------------------------------------------------------------------------------------------
// Добавить кнопку в набор
// ---
void PropertyManagerObject::AddButton( ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, bool cheched, bool enable )
{
  AddButton2( buttons, btnID, btnID, cheched, enable );
}


//-----------------------------------------------------------------------------------------------------------------------------------
// Добавить кнопку в набор
// ---
void PropertyManagerObject::AddButton2( ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, long bmpID, bool cheched, bool enable )
{
  if ( buttons ) 
  {
    buttons->AddButton( btnID, _variant_t(bmpID), -1 );
    buttons->ButtonEnable  [ btnID ] = enable;
    buttons->ButtonChecked [ btnID ] = cheched;
    _bstr_t tmpBstr( LoadStr(btnID) ); 
    buttons->ButtonTips    [ btnID ] = tmpBstr;
    buttons->ButtonIconFont[ btnID ] = GetLibraryIconFont();
  }
}


//----------------------------------------------------------------------------------------------
// Добавить кнопку в набор
// ---
BOOL PropertyManagerObject::EndProcess() 
{
  UnadviseEvents();
  return TRUE;
}

void PropertyManagerObject::UnadviseEvents()
{
  BaseEvent::TerminateEvents( procParam );
}


//----------------------------------------------------------------------------------------------
// Найти контрол
// ---
ksAPI7::IPropertyControlPtr PropertyManagerObject::GetPropertyControl( int ctrlID )
{
  ksAPI7::IPropertyControlPtr control(NULL);
  if ( propTabs ) 
  {
    for ( long i = 0, c = propTabs->Count; i < c && control == NULL; i++ ) 
    {
    ksAPI7::IPropertyTabPtr tab( propTabs->Item[ _variant_t(i) ] );
    ksAPI7::IPropertyControlsPtr ctrls( tab ? tab->PropertyControls : NULL );
      if ( ctrls ) 
      {
        _bstr_t name( LoadStr(ctrlID) );
        control = ctrls->Item[ _variant_t(name) ];
      }
    }
  }
  return control;
}


//----------------------------------------------------------------------------------------------
// Установить доступность контрола
// ---
void PropertyManagerObject::SetControlEnable( long ctrlID, bool enabled ) 
{
  ksAPI7::IPropertyControlPtr control(GetPropertyControl(ctrlID), false );
  if ( control ) 
    control->Enable = enabled;
}


////////////////////////////////////////////////////////////////////////////////
//
// PropertyManagerEvent - обработчик событий от документа
//
////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
//
// ---
PropertyManagerEvent::PropertyManagerEvent( LPDISPATCH manager, PropertyManagerObject& _obj ) :
  BaseEvent( manager, ksAPI7::DIID_ksPropertyManagerNotify ), obj( _obj )
{
  Advise();
}


//-------------------------------------------------------------------------------
//
// ---
PropertyManagerEvent::~PropertyManagerEvent()
{
}

//-------------------------------------------------------------------------------
// Карта сообщений
// ---
BEGIN_EVENTSINK_MAP(PropertyManagerEvent, BaseEvent)
  // 1 - prButtonClick        - Нажатие кнопки.
  ON_EVENT (PropertyManagerEvent, (unsigned int)-1, prButtonClick       , ButtonClick       , VTS_I4                )
  // 2 - prChangeControlValue - Событие изменения значения контрола
  ON_EVENT (PropertyManagerEvent, (unsigned int)-1, prChangeControlValue, ChangeControlValue, VTS_DISPATCH          )
  // 3 - prControlCommand     - Нажатие кнопки контрола
  ON_EVENT (PropertyManagerEvent, (unsigned int)-1, prControlCommand    , ControlCommand    , VTS_DISPATCH VTS_I4   )
  // 4 - CLLBACK для получения типа контекстного меню
  ON_EVENT( PropertyManagerEvent, (unsigned int)-1, prGetContextMenuType, GetContextMenuType, VTS_I4 VTS_I4 VTS_PI4 )
  // 5 - CLLBACK для накачки контекстной панели
  ON_EVENT( PropertyManagerEvent, (unsigned int)-1, prFillContextPanel  , FillContextPanel  , VTS_DISPATCH          )
END_EVENTSINK_MAP()


//-----------------------------------------------------------------------------
// prChangeControlValue - Событие изменения значения контрола 
// ---
afx_msg BOOL PropertyManagerEvent::ChangeControlValue(LPDISPATCH  iCtrl)
{

  ksAPI7::IPropertyControlPtr control( iCtrl );
  auto part = obj.GetPart();

   
  if (control->Id == ID_CHOSE_DETAIL)
  {
      CString patch; 
      patch = get_value_from_list(obj, ID_CHOSE_DETAIL);
      if (!patch.IsEmpty())
      {
         /* LibMessage(patch, 0);*/
          int sumbolF = patch.Find(new_detile_df);
          //LibMessage(LPCTSTR(std::to_wstring(sumbolF).c_str()),0);
          if (sumbolF>=0)
          {
              auto part_info = obj.get_part_info();

              part_info->name_detail;

              auto& part = obj.GetPart();
              auto info = obj.get_part_info(); 
              part->ClearAllObj();
              obj.load_default_panel();
              part->Update();
              obj.RedrawPhantom();
              info->part = part;
              info->patch = patch;
          }
          else
          {
              auto& part = obj.GetPart();
              if (!part)
              {
                  LibMessage(_T("part empty"), 0);
              }
              part->ClearAllObj();
              part->SetFileName((LPWSTR)(LPCTSTR)patch);
              part->Update();
              obj.RedrawPhantom();
              auto info = obj.get_part_info();
              info->part = part;
              info->patch = patch;
          }

      }


  }
  else
  {
      switch (control->Id)
      {
      case(ID_H_3D_PLATE):
      {
          LibMessage(_T("ID_H_3D_PLATE"), 0);
          InsertPartPtr part_info = obj.get_part_info();
          part_info->doc->SetActive();
          auto cor_part = part_info->part;
          auto col_entity = cor_part->EntityCollection(o3d_sketch);
          auto count = col_entity->GetCount();
          LibMessage((LPCTSTR)std::to_string(count).c_str(), 0);
          IEntityPtr entitySketch = col_entity->GetByIndex(0);
          if (entitySketch)
          {
              LibMessage(_T("entitySketch"), 0);

              ISketchDefinitionPtr sketchDefinition(entitySketch->GetDefinition());
              if (sketchDefinition)
              {
                  LibMessage(_T("sketchDefinition"), 0);

                  // Получим интерфейс базовой плоскости XOY
                  IEntityPtr basePlane(cor_part->GetDefaultEntity(o3d_planeXOY), false /*AddRef*/);

                  // Установка параметров эскиза
                  sketchDefinition->SetPlane(basePlane); // Установим плоскость XOY базовой для эскиза


                  // Создадим эскиз

                  int w=300, h = 10;
                  // Войти в режим редактирования эскиза
                  if (sketchDefinition->BeginEdit())
                  {
                      LineSeg(0, 0, w, 0, 1);
                      LineSeg(0, 0, 0, h, 1);
                      LineSeg(0, h, w, h, 1);
                      LineSeg(w, 0, w, h, 1);

                      sketchDefinition->EndEdit();
                  }

              }
          }
          else
          {
              LibMessage(_T("entitySketch is empty"), 0);

          }
          break;
      }
      case(ID_W_3D_PLATE):
      {
          LibMessage(_T("ID_W_3D_PLATE"), 0);
          break;
      }
      case(ID_Z_3D_PLATE):
      {
          LibMessage(_T("ID_Z_3D_PLATE"), 0);
          break;
      }

      default:
          break;
      }
      //LibMessage(CString(LPCWSTR (std::to_wstring(control->Id).c_str())), 0);
      if (control->Id == ID_POINT_3D_X)
      {

          LibMessage(_T("ID_POINT_3D_X"), 0);

      }
      else
          if (control->Id == ID_POINT_3D_Y)
          {
              LibMessage(_T("ID_POINT_3D_Y"), 0);
          }
          else
              if (control->Id == ID_POINT_3D_Z)
              {
                  LibMessage(_T("ID_POINT_3D_Z"), 0);
              }

  }

    
  if ( control ) 
  {
   /*   if (iCtrl == ID_CHOSE_DETAIL)
      {

      }*/
    _variant_t tmpVar = control->Value;
    long ctrlID = control->Id;
    obj.OnChangeControlValue(ctrlID, tmpVar); 
    obj.RedrawPhantom();  
  }
  return true;
}


//-----------------------------------------------------------------------------
// prChangeControlValue - Событие изменения значения контрола 
// ---
afx_msg BOOL PropertyManagerEvent::ButtonClick(long buttonID)
{
  if ( buttonID == pbHelp ) 
    ksOpenHelpFileT( LIB_HELP, HELP_CONTEXT, 1);
  if (buttonID == pbEnter)
    obj.OnButtonClick(buttonID);
  return true;
}

//-----------------------------------------------------------------------------
// prControlCommand Нажатие кнопки контрола
// ---
afx_msg BOOL PropertyManagerEvent::ControlCommand(LPDISPATCH ctrl, long buttonID ) 
{
  obj.OnButtonClick(buttonID); 
  obj.RedrawPhantom();  
  return true;
}


//-----------------------------------------------------------------------------
// CLLBACK для получения типа контекстного меню
// ---
afx_msg BOOL PropertyManagerEvent::GetContextMenuType( long LX, long LY, long * ContextMenuType )  // LX LY это экранные координаты
{
  *ContextMenuType = ksProcessContextPanel;
  return TRUE;
}


//-----------------------------------------------------------------------------
// "CLLBACK для накачки контекстной панели
// ---
afx_msg BOOL PropertyManagerEvent::FillContextPanel( LPDISPATCH ContextPanel )
{
  return TRUE;
}

//------------------------------------------------------------------------------
// Получить библиотечный шрифт
// ---
_bstr_t GetLibraryIconFont()
{
  static _bstr_t font;
  if ( font.length() == 0 && pNewKompasAPI )
  {
    ::ksAPI7::ILibraryManagerPtr libMng( pNewKompasAPI->LibraryManager );
    if ( libMng )
    {
      ::ksAPI7::IProceduresLibrariesPtr procLibs( libMng->ProceduresLibraries );
      if ( procLibs )
      {
        ::ksAPI7::IProceduresLibraryPtr procLib(procLibs->Item[LoadStr(LIBRARYID())]);
        if ( procLib )
          font = procLib->IconsFont;
      }
    }
  }
  return font;
}

Process3DEvent::Process3DEvent(const ksAPI7::IProcess3DPtr& process,
  PropertyManagerObject& _obj, bool copyProc) : BaseEvent(process, ksAPI7::DIID_ksProcess3DNotify)
  , m_obj(_obj)
{
  Advise();
}

Process3DEvent::~Process3DEvent()
{
  delete &m_obj;
}

//--------------------------------------
// ksStop     -Остановка процесса
// ---
BOOL Process3DEvent::Stop()
{
  return TRUE;
}

//--------------------------------------
// ksRun     -Запуск процесса
// ---
BOOL Process3DEvent::Run()
{
  return TRUE;
}

//---------------------------------------------------------------
// ksFilterObjects    -Фильтрация объектов
// ---
BOOL Process3DEvent::FilterObject(LPDISPATCH Object) 
{
  return m_obj.FilterObject(Object);  
}

//---------------------------------------------------------
// ksPlacementChanged    -Изменено положения объекта
// ---
BOOL Process3DEvent::PlacementChanged(LPDISPATCH object)
{
  return m_obj.PlacementChanged(object);
}

//--------------------------------------
// ksEndProcess    -Окончание процесса.
// ---
BOOL Process3DEvent::EndProcess()
{
  // Завершение процесса
  if ( m_obj.EndProcess() )
  {
    // Отписка от получения событий
    Disconnect();
  }
  return TRUE;
}

//-----------------------------------------------------------------------------------
// ksProcess3DCreateTakeObject    -Событие создания объекта в - --подчиненном режиме.
// ---
BOOL Process3DEvent::TakeObject(LPUNKNOWN obj)
{
  return m_obj.TakeObject(obj);
}

//--------------------------------------------------------------------------------------------------------------------------
// Карта сообщений
// ---
BEGIN_EVENTSINK_MAP(Process3DEvent, BaseEvent)
  // ksFilterObjects    -Фильтрация объектов
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DFilterObjects   , FilterObject    , VTS_DISPATCH )
  // ksPlacementChanged    -Изменено положения объекта
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DPlacementChanged, PlacementChanged, VTS_DISPATCH )
  // ksEndProcess    -Событие создания объекта в - --подчиненном режиме.
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DCreateTakeObject, TakeObject      , VTS_UNKNOWN  )
  // ksEndProcess    -Окончание процесса.
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DEndProcess      , EndProcess      , VTS_NONE     )
  // ksRun     -Запуск процесса
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DRun             , Run             , VTS_NONE     )
  // ksStop     -Остановка процесса
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DStop            , Stop            , VTS_NONE     )
END_EVENTSINK_MAP()


//-------------------------------------------------------------------------------
//
// ---
Process3DManipulatorsEvent::Process3DManipulatorsEvent(ksAPI7::IManipulatorsPtr manipulators, Process3DManipulatorsObject& _obj, bool copyProc)
  : BaseEvent(manipulators, ksAPI7::DIID_ksProcess3DManipulatorsNotify)
  , m_obj(_obj)
{
  Advise();
}


//-------------------------------------------------------------------------------
//
// ---
Process3DManipulatorsEvent::~Process3DManipulatorsEvent()
{}


//-------------------------------------------------------------------------------
// Карта сообщений
// ---
BEGIN_EVENTSINK_MAP(Process3DManipulatorsEvent, BaseEvent)
  // 1 - ksRotateManipulator - Поворот манипулятора
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksRotateManipulator        , RotateManipulator        , VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BOOL)
  // 2 - ksMoveManipulator   - Передвижение манипулятора
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksMoveManipulator          , MoveManipulator          , VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BOOL                     )
  // 3 - ksClickManipulatorPrimitive - клик по примитиву манипулятора                                                           
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksClickManipulatorPrimitive, ClickManipulatorPrimitive, VTS_I4 VTS_I4 VTS_BOOL                                          )
  // 4 - ksBeginDragManipulator - начало перетаскивания манипулятора                                                           
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksBeginDragManipulator     , BeginDragManipulator     , VTS_I4 VTS_I4                                                   )
  // 5 - ksEndDragManipulator - окончание перетаскивания манипулятора                                                           
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksEndDragManipulator       , EndDragManipulator       , VTS_I4 VTS_I4                                                   )                             
END_EVENTSINK_MAP()


//--------------------------------------------------------------------------------------------------------------------------------
// ksRotateManipulator - Поворот манипулятора
// ---
BOOL Process3DManipulatorsEvent::RotateManipulator(long ManipulatorId,double X0, double Y0, double Z0,double AxisZX,
                                                 double AxisZXY, double AxisZZ, double angle, BOOL FromEdit)
{
  return m_obj.RotateManipulator( X0, Y0, Z0, AxisZX, AxisZXY, AxisZZ, angle, !!FromEdit);
}

//--------------------------------------------------------------------------------------------------------------------------------
// ksMoveManipulator   - Передвижение манипулятора
// ---
BOOL Process3DManipulatorsEvent::MoveManipulator(long ManipulatorId, double VX, double VY, double VZ, double Delta, BOOL FromEdit)
{
  return m_obj.MoveManipulator(ManipulatorId, VX, VY, VZ, Delta, !!FromEdit);
}

//------------------------------------------------------------------------------------------------------------------
// ksClickManipulatorPrimitive - клик по примитиву манипулятора
// ---
BOOL Process3DManipulatorsEvent::ClickManipulatorPrimitive(long ManipulatorId, long PrimitiveType, BOOL DoubleClick)
{
  return m_obj.ClickManipulatorPrimitive(ManipulatorId, PrimitiveType, !!DoubleClick);
}

//--------------------------------------------------------------------------------------------
// ksBeginDragManipulator - начало перетаскивания манипулятора
// ---
BOOL Process3DManipulatorsEvent::BeginDragManipulator(long ManipulatorId, long PrimitiveType)
{
  return m_obj.BeginDragManipulator(ManipulatorId, PrimitiveType);
}

//---------------------------------------------------------------------------------------------
// ksEndDragManipulator - окончание перетаскивания манипулятора
// ---
BOOL Process3DManipulatorsEvent::EndDragManipulator(long ManipulatorId, long PrimitiveType)
{
  return m_obj.EndDragManipulator(ManipulatorId, PrimitiveType);
}



