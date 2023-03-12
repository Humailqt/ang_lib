////////////////////////////////////////////////////////////////////////////////
//
// PropertyManagerObject - Класс для обслуживания панели свойств 
// PropertyManagerEvent  - Класс событий менеджера свойств 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __PROPMEN_H
#define __PROPMEN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <ldefin2d.h>
#include "BaseEvent.h"
#ifndef _ACTIVE_FILE_PATCH
#include "ResourceDir.h"
#endif
#include <math.h>
#include <charconv>
//-------------------------------------------------------------------------------
// Функции для выдачи сообщений библиотеки
// ---
int LibMessage( int     strId, int flags ); // Сообщение загружается из ресурсов
int LibMessage( LPCTSTR str,   int flags ); // Сообщение передается строкой 


//-------------------------------------------------------------------------------
// Для перехвата COM исключений
// ---
void DumpError(_com_error& e);              // Вывод текста исключения 
void ShowError();                           // Вывод сообщений при самостоятельном анализе HRESULT 

#ifndef InsertPartPtr
#define InsertPartPtr std::shared_ptr<InsertPart> 
#endif // !InsertPartPtr


// Получить библиотечный шрифт
_bstr_t GetLibraryIconFont();

////////////////////////////////////////////////////////////////////////////////
//
// Класс для обслуживания панели свойств 
//
// ctrlID используется как идентивикатор контрола и как
// идентификатор для загрузки  ресурсов контрола
//
////////////////////////////////////////////////////////////////////////////////
class PropertyManagerObject 
{
protected:
  int                          flagMode;         // Признак редактирования по дабл-клику
  ksAPI7::IProcessParamPtr     procParam;        // Параметры процесса          
  ksAPI7::IPropertyTabsPtr     propTabs;         // Закладки
  ksAPI7::IPropertyGridPtr     paramGrid;        // Грид для вывода дополнительных параметров элемента 
  ksAPI7::IPropertySlideBoxPtr slideBox;         // Окно просмотра 
  long                         rowIndex;         // Индекс строки

  
public:

  ///////////////////////////////////////////////////////////////////////////////
    bool rotated = false;
    double angle = 90;
    //double x, y, z=20;
    //double h = 20, w=20;
  ///////////////////////////////////////////////////////////////////////////////
  ksAPI7::IPropertyControlsPtr curentCollection; // Контролы текущей закладки панели свойств 
           PropertyManagerObject();
  virtual ~PropertyManagerObject();

// Менеджер свойств ##################################################################
  // Событие prButtonClick - Нажатие кнопки.
  virtual void OnButtonClick       ( long buttonID )                     {}     
  // Событие prChangeControlValue - Изменение значения контрола
  virtual void OnChangeControlValue( long ctrlID, const VARIANT& newVal ){}
  virtual InsertPartPtr get_part_info() { return NULL; }
  /////////////////////////////////////////////////////////////

  virtual int load_default_panel() { return 1; };
  std::string get_tmp_filename_tmp(IDocument3DPtr doc) { return std::string(""); };

  /////////////////////////////////////////////////////////////

  // Инициализация параметров процесса 
  bool InitProcessParam( long toolBarID, SpecPropertyToolBarEnum toolBarType, long firstTabID = 0 );
  
  // Добавление закладки
  bool CreateTab( long tabID, BOOL visible = TRUE, BOOL active = FALSE ); 
  // Добавление комбобокса вещественных значений 
  ksAPI7::IPropertyListPtr        CreateRealList   ( double minVal = 0, double maxVal = 0 );
  // Добавление комбобокса        строковых значений 
  ksAPI7::IPropertyListPtr        CreateStringList ();
  // Добавление комбобокса        целых значений 
  ksAPI7::IPropertyListPtr        CreateIntList    ();
  // Добавление сепаратора 
  ksAPI7::IPropertySeparatorPtr   CreateSeparator  ( SeparatorTypeEnum type );
  // Добавление CheckBox-а 
  ksAPI7::IPropertyCheckBoxPtr    CreateCheckBox   ( bool checked );
  // Добавление группы кнопок 
  ksAPI7::IPropertyMultiButtonPtr CreateMultiButton( ButtonTypeEnum type );
  // Добавление кнопоки в группу
  void AddButton(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, bool cheched = false,  bool enable = true );
  // Добавление кнопоки в группу
  void AddButton2(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, long bmpID, bool cheched = false,  bool enable = true );

  // Получить указатель на Control по его идентификатору (для изменения параметров контрола)
  ksAPI7::IPropertyControlPtr  GetPropertyControl( int ctrlID );
  // Изменить состояние контрола
  void SetControlEnable( long ctrlID, bool enabled = true ); 

  // Накачка контрола общими параметрами
  void InitPropertyControl(ksAPI7::IPropertyControl* control, long ctrlID, UINT hint = 0, UINT tips = 0,
                            BOOL enable = TRUE, PropertyControlNameVisibility nameVisibility = ksNameAlwaysVisible, 
                            BOOL visible = TRUE );

  // Виртуальная функция наполнения контролов элемента
  virtual void ShowControls (){} 
  virtual unsigned int get_order_control(variant_t ID) { return -1; };
  // Перерисовать фантом
  virtual void RedrawPhantom(){}
  virtual void get_part(){}
  virtual IPartPtr& GetPart() { 
      IPartPtr i = NULL;
      return i; }
  // Количество параметров выводимых в гриде 
  virtual int  ParamCount(){ return 0; }
  // Заполнение грида параметров 
  virtual void ShowParam (){}
  // Идентификатор слайда
  virtual long GetSlideID(){ return 0; }

  // Функции для заполнения грида параметров        
  // Добавление строкового значения
  void AddStringToGrig( long paramID, LPCTSTR value ); 
  // Добавление числового  значения        
  void AddDoubleToGrig( long paramID, double  value ); 
  // ksFilterObjects    -Фильтрация объектов
  virtual BOOL FilterObject    (LPDISPATCH Object) { return false; };
  // ksProcess3DCreateTakeObject    -Событие создания объекта в - --подчиненном режиме.
  virtual BOOL TakeObject      (LPUNKNOWN Object)  { return false; };
  // ksPlacementChanged   - Изменено положения объекта
  virtual BOOL PlacementChanged(LPDISPATCH Object) { return false; };
  // Завершить процесс редактирования
  virtual BOOL EndProcess();
  virtual void UnadviseEvents();

// Менеджер свойств ##################################################################
};

////////////////////////////////////////////////////////////////////////////////
//
// PropertyManagerEvent - обработчик событий от менеджера панели свойств
//
////////////////////////////////////////////////////////////////////////////////
class PropertyManagerEvent : public BaseEvent
{
protected:

public:
  PropertyManagerObject&  obj; // Указатель на подписанный объект
  PropertyManagerEvent( LPDISPATCH manager, PropertyManagerObject& _obj );           
  virtual ~PropertyManagerEvent();



  void draw_panel(IDocument3DPtr doc, IPart* part/*,CStringW str*/);


public: 
  // prChangeControlValue - Событие изменения значения контрола 
  afx_msg BOOL ButtonClick       ( long buttonID );  
  // prChangeControlValue - Событие изменения значения контрола 
  afx_msg BOOL ChangeControlValue( LPDISPATCH ctrl );
  // prControlCommand - Нажатие кнопки контрола
  afx_msg BOOL ControlCommand    ( LPDISPATCH  ctrl, long buttonID );
  // CLLBACK для получения типа контекстного меню
  afx_msg BOOL GetContextMenuType( long LX, long LY, long * ContextMenuType );  // LX LY это экранные координаты
  // CLLBACK для накачки контекстной панели
  afx_msg BOOL FillContextPanel  ( LPDISPATCH ContextPanel );
  DECLARE_EVENTSINK_MAP()
};

////////////////////////////////////////////////////////////////////////////////
//
// _2_ Process3DEvent - обработчик событий от процесса 3D
//
////////////////////////////////////////////////////////////////////////////////
class Process3DEvent : public BaseEvent
{
protected:
  PropertyManagerObject& m_obj;           // Указатель на подписанный объект
  // Attributes
public:
  Process3DEvent(const ksAPI7::IProcess3DPtr& process, PropertyManagerObject& _obj, bool copyProc = false);
  virtual ~Process3DEvent();
  
  // Operations
public:
  // ksFilterObjects -Фильтрация объектов
  afx_msg BOOL FilterObject    ( LPDISPATCH  Object );
	// ksProcess3DCreateTakeObject  -Событие создания объекта в - --подчиненном режиме.
  afx_msg BOOL TakeObject      ( LPUNKNOWN   Object );
   //  ksPlacementChanged - Изменено положения объекта
  afx_msg BOOL PlacementChanged( LPDISPATCH  Object );
	// ksEndProcess    -Окончание процесса.
  afx_msg BOOL EndProcess      ();
	// ksStop     -Остановка процесса
  afx_msg BOOL Stop            ();
	// ksRun     -Запуск процесса
  afx_msg BOOL Run             ();
  DECLARE_EVENTSINK_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 


////////////////////////////////////////////////////////////////////////////////
//
/// Класс для реализации событий манипулятора
//
////////////////////////////////////////////////////////////////////////////////
class Process3DManipulatorsObject 
{
public:
  // ksRotateManipulator - Поворот манипулятора
  virtual bool RotateManipulator(double X0, double Y0, double Z0, double AxisZX, double AxisZXY,
  	                             double AxisZZ, double angle, bool FromEdit    )                     { return false; };
  // ksMoveManipulator   - Передвижение манипулятора
  virtual bool MoveManipulator          (long ManipulatorId, double VX, double VY, 
  	                                     double VZ, double Delta, bool FromEdit)                     { return false; };
  // ksClickManipulatorPrimitive - клик по примитиву манипулятора
  virtual bool ClickManipulatorPrimitive(long ManipulatorId, long PrimitiveType, bool DoubleClick)   { return false; };
  // ksBeginDragManipulator - начало перетаскивания манипулятора
  virtual bool BeginDragManipulator     (long ManipulatorId, long PrimitiveType)                     { return false; };
  // ksEndDragManipulator - окончание перетаскивания манипулятора
  virtual bool EndDragManipulator       (long ManipulatorId, long PrimitiveType)                     { return false; };
  
  virtual ~Process3DManipulatorsObject() {};
};


////////////////////////////////////////////////////////////////////////////////
//
// _3_ Process3DManipulatorsEvent - обработчик событий от манипуляторов 3D
//
////////////////////////////////////////////////////////////////////////////////
class Process3DManipulatorsEvent : public BaseEvent
{
protected:
  Process3DManipulatorsObject& m_obj;             // Указатель на подписанный объект
  // Attributes
public:
  Process3DManipulatorsEvent(const ksAPI7::IManipulatorsPtr manipulators, Process3DManipulatorsObject& _obj, bool copyProc = false);
  virtual ~Process3DManipulatorsEvent();
  
  //Operations
public:
  // ksRotateManipulator - Поворот манипулятора
  BOOL RotateManipulator         (long ManipulatorId, double X0, double Y0, double Z0, double AxisZX,double AxisZXY, double AxisZZ, double angle, BOOL FromEdit);
  // ksMoveManipulator   - Передвижение манипулятора
  BOOL MoveManipulator           (long ManipulatorId, double VX, double VY, double VZ, double Delta ,BOOL FromEdit);
  // ksClickManipulatorPrimitive - Клик по примитиву манипулятора
  BOOL ClickManipulatorPrimitive (long ManipulatorId, long PrimitiveType, BOOL DoubleClick);
  // ksBeginDragManipulator - Начало перетаскивания манипулятора
  BOOL BeginDragManipulator      (long ManipulatorId, long PrimitiveType);
  // ksEndDragManipulator   - Окончание перетаскивания манипулятора
  BOOL EndDragManipulator        (long ManipulatorId, long PrimitiveType);
  DECLARE_EVENTSINK_MAP();
};
  
class Shpeel;
class PropertyManagerObject;

CString get_value_from_list(Shpeel& shpeel, long id_control);
CString get_value_from_list(PropertyManagerObject& shpeel, long id_control);
CString get_value_from_list(Shpeel* shpeel, long id_control);
