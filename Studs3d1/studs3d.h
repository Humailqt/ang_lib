////////////////////////////////////////////////////////////////////////////////
//
// studs3d.h - Крепежный элемент на Visual C++ - шпилька
// Структуры параметров и класс шпилек
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __STUDS3D_H
#define __STUDS3D_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __PROPMEN_H
#include "PropMen.h"
#endif

#include <libhppar.h>
#include <SArray.h>
#include <PArray.h>
#include <filesystem>



#if !defined(_IFUNC)
# define _IFUNC STDMETHODCALLTYPE
#endif

//-------------------------------------------------------------------------------
// Маски настройки
// ---
#define AXIS_OFF     0x1    // FALSE - рисовать, TRUE - не рисовать
#define PITCH        0x2    // FALSE - крупный шаг, TRUE - мелкий шаг
#define SIMPLE       0x10   // FALSE - нормальная отрисовка, TRUE - упрощенная отрисовка
#define ISPOLN       0x80   // FALSE - исполнение 1, TRUE - исполнение 2
#define TAKEISPOLN   0x100  // FALSE - не учитывать исполнение, TRUE - учитывать исполнение
#define ALLST        0x4    // FALSE - резьба на b, TRUE - резьба до головки
#define KONTR        0x8    // FALSE - обычная головка, TRUE - головка под контровочное отверстие
#define SECOFF       0x10   // FALSE - сечение работает, TRUE - сечение засерить, для винтов, для трубопроводов
#define TAKEKONTR    0x20   // FALSE - не учитывать контровку, TRUE - учитыварь контровку
#define REVERCE_ON   0x20   // FALSE - включить прямую, TRUE - обратную отрисовку
#define TAKEPODG     0x40   // FALSE - не учитывать подголовку, TRUE - учитыварь бодголовку
#define LEFT_DRAW    0x40   // TRUE - рисовать слева
#define FLAGLENGTH   0x200  // FALSE - длина болта подобрана правильно, TRUE - не правильно
#define ISPOLN3      0x200  // Для штифтов FALSE - исполнений 2, TRUE - исполнений 3
#define REVERCE_DRAW 0x200  // TRUE - обратная отрисовка, FALSE - прямая отрисовка
#define SBORSHAYBA   0x400  // FALSE - гайка без шайбы, TRUE - гайка вместе с шайбой
#define PITCHOFF     0x800  // FALSE - есть крупный и мелкий шаг, TRUE - есть только крупный
#define LENLEN       0x1000 // FALSE - нормальная длина резьбы, TRUE - удлиненная
#define LENLENOFF    0x2000 // FALSE - не учитывать, TRUE - учитывать

//-------------------------------------------------------------------------------
// Дополнительные маски 
// ---
#define KEY_S_ON     0x1   // TRUE - включены дополнитеоьные размеры под ключ, FALSE - выключены
#define KOEFF_MAT_ON 0x4   // FALSE - только металл, TRUE другие материалы
#define KEY_S        0x2   // TRUE - дополнительный размер под ключ, FALSE - обычный размер под ключ 
#define KEY_S_GRAY   0x8   // TRUE - греить "Дополнитеоьные размеры под ключ"
#define DRAW_ST_OF   0x10  // FALSE - стержень у болта рисуется TRUE - рисовать только головку

//-------------------------------------------------------------------------------
// Насроечная информация
// ---
#define COUNT_MASSA      1000 // Количество деталей масса которых лежит в ГОСТ-е
#define STANDART_SECTION 25   // Номер раздела и подраздела спецификации
#define SPC_NAME          5    // Колонка наименования

#define EPSILON          0.001  

#define STUDS_FILE       _T("stud.l3d")           // Библиотека моделей
#define STUDS_INPUT_PATH _T("|Шпильки|Шпилька_1") // Путь внутри библиотеки моделей

#define SPCOBJ_CREATE    TRUE // Признак подключения объекта к спецификации
#define PARAM_COUNT      5    // Количество параметров выводимых в гриде 

#define MAX_LENGTH 1000       // Максимальная длина стержня
#define MIN_LENGTH 0          // Минимальная длина стержня

////////////////////////////////////////////////////////////////////////////////
//
// Структуры параметров и класс шпилек
//
////////////////////////////////////////////////////////////////////////////////

#pragma pack( push, 1)
//------------------------------------------------------------------------------
// Структура параметров шпилек
// ---                      
struct SHPEEL 
{
  float d;          // Диаметр резьбы
  float p1;         // Шаг резьбы
  float p2;         // Шаг резьбы
  float b1;         // Ввинчиваемый конец 
  float c;          // Размер фаски
  float l;          // Длина шпильки
  float b;          // Гаечный конец 
  short f;          // Битовые маски
  short klass;      // Класс точности
  short gost;       // Номер ГОСТа
  short ver;        // Версия макро
  float m1;         // Масса 1 исполнение
  float m2;         // Масса 2 исполнение
  short indexMassa; // 0 - металл 1 - алюмин сплав 3 - бронза 2 - латунь
};


////////////////////////////////////////////////////////////////////////////////
//
//  Структуры для работы с БД
//
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
// Для считывания из баз st22032.loa ... st22043.loa
// ---
struct ShpeelSTmp 
{
  short t;   // Флаг использовать запись или нет
  float l;   // Длина шпильки
  float d;   // Диаметр резьбы
  float b;   // Гаечный конец
  float m1;  // Масса исполнения 1
  float m2;  // Масса исполнения 2
};


//------------------------------------------------------------------------------
// Для считывания  из  баз 22032.loa ... 22043.loa
// ---
struct ShpeelTmp 
{
  short t;   // Флаг использовать запись или нет
  float d;   // Диаметр резьбы
  float p1;  // Шаг резьбы
  float p2;  // Шаг резьбы
  float b1;  // Ввинчиваемый конец
  float c;   // Размер фаски
};

struct ShpeelTmp2 {
  short t;
  float d;
};

struct ShpeelTmp3 {
  short t;
  float L;
  float dr;
};


//------------------------------------------------------------------------------
// Работа с БД
// ---
struct ShpeelBase 
{
  reference bg, rg1, rg2;
  reference bs, rs1, rs2;

  ShpeelBase()
    : bg ( 0 )
    , rg1( 0 )
    , rg2( 0 )
    , bs ( 0 )
    , rs1( 0 )
    , rs2( 0 )
  {
  }
  ~ShpeelBase() {}
};


//-------------------------------------------------------------------------------
// Макро параметры
// ---
struct BaseMakroParam 
{
  float          ang;
  unsigned short flagAttr;
  short          drawType;
  BYTE           typeSwitch;  // Тип запроса положения базовой точки элемента
                              // 0 - точка и направление оси 0X ( Placement )
                              // 1 - точка, направление совпадает с осью 0X текущей СК ( Cursor )
};

struct info_list
{
    _variant_t id;
    unsigned int order_id;

};

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// Класс шпилек
//
////////////////////////////////////////////////////////////////////////////////
extern class Shpeel : public PropertyManagerObject
             , public ILibPropertyObject, public Process3DManipulatorsObject
{
private :
  int                                     refCount;
  IDocument3DPtr                               doc; // Указатель на интерфейс текущего 3D-документа
  IPartPtr                                  m_part; // Деталь
  IEntityPtr                           m_takePoint; // Точка 3D
  bool                              collectChanged; // true - массив изменился
  CString                                 fileName; // Имя файла
  int                                     flagMode; // Тип редактирования
  ShpeelBase                                 bBase; // Работа с БД
  bool                                     changed; // Признак изменения параметров
  bool                                    openBase; // Работа с БД
  _variant_t                               Objects;


  ksAPI7::ISpecificationBaseObjectsPtr m_spcBaseObjects;

  ksAPI7::IPlacement3DManipulatorPtr m_manipulator; // Манипулятор 
  ksAPI7::IBaseManipulatorPtr            m_baseMan; // Базовый интерфейс манипулятора 

  ksAPI7::IProcess3DPtr                m_process3D; // Копия процесса 3D
  ksAPI7::IProcessPtr                    m_process;


  bool                           fixingPart  = false; // Флаг закрепления объекта
  IMateConstraintCollectionPtr              m_mCol; // Коллекция сопряжений

  // Структуры par и tmp записываемые как UserParam 
  BaseMakroParam                               par; // Макро параметры
  SHPEEL                                       tmp; // Структура параметров шпилек

public :
  CString patch_lib;
  std::vector<info_list> v_info_list;
  Shpeel();
  virtual ~Shpeel();

  /* IUnknown */
  virtual unsigned long _IFUNC AddRef();
  virtual unsigned long _IFUNC Release();
  virtual HRESULT    _IFUNC QueryInterface(const GUID far& iid, void far*far* iface);
  virtual HRESULT       _IFUNC LtQueryInterface( int idType, void far*far* iface );

  ////////////////////////////////////////////////////////////////////////////
  int load_default_panel();
  void draw_panel();
  std::wstring get_tmp_filename_tmp(IDocument3DPtr doc);

  ///////////////////////////////////////////////////////////////////////////

  // Инициализация
  void               Init();                                          
  // Возвращает изменился ли массив 
  bool &            GetCollectChanged() { return collectChanged; }   
  // Возвращает создавать ли объект спецификации
  unsigned short &  GetFlagAttr() { return par.flagAttr; }           
  // При вставке из библиотеки моделей указывается полный путь к файлу библиотеки и путь внутри библиотеки моделей
  CString &         GetFileName();                                   
  // Редактируем переменные
  void              SetParam( IPartPtr& pPart );                                      
  // Размер данных
  int               Size() { return sizeof(tmp) + sizeof(par); }     
  // Редактирование объекта спецификации
  reference         EditSpcObj( reference spcObj );                  
  // Функция проверки подключения атрибутов или нет
  bool              IsSpcObjCreate() { return SPCOBJ_CREATE; };      
  // Рисование 3D объектов
  void              Draw3D( );                          
  // Открытие БД
  int               OpenBase();                                      
  // Закрытие БД
  void              CloseBase();                                     
  // Получить параметры
  int               GetParam();                                      
  // Отрисовка объекта спецификации
  bool              DrawSpcObj( reference & spcObj );                                    
  // AddRef - не делает
#define DEBUG_GetPart 1
#if DEBUG_GetPart
#define DEBUG_GetPart_ 1 
#endif // DEBUG_GET_VALUE_FROM_LIST

  IPartPtr&         GetPart()   { return m_part; }
  // AddRef - не делает
  IDocument3DPtr&   GetDoc()    { return doc;  }
  unsigned int get_order_control(variant_t ID);
  void Shpeel::get_part();

  bool AddMate(short constraintType,
               short direction,
               short fixed,
               double val,
               IEntity* ent,
               LPCTSTR surfaceName,
               IPart* p,
               IMateConstraintCollection* mCol);
  // Находит у детали поверхность с заданным именем
  IEntityPtr        GetEntityByName( LPCTSTR name, IPartPtr& p );
  bool              GetDirectionElem() { return true; }           
  
// Менеджер свойств ##################################################################
  // Событие prButtonClick - Нажатие кнопки.
  virtual void      OnButtonClick( long buttonID );     
  // Событие prChangeControlValue - Изменение значения контрола
  virtual void      OnChangeControlValue( long ctrlID, const VARIANT& newVal );
  
  // Перерисовка фантома
  virtual void      RedrawPhantom();    
  // Виртуальная функция наполнения контролов элемента
  virtual void      ShowControls();
  // Вывод параметров
  virtual void      ShowParam();  
  // Количество параметров выводимых в гриде 
  virtual int       ParamCount(){ return PARAM_COUNT; }
  // Идентификатор слайда
  virtual long      GetSlideID();

  // Заполняет список диаметров
  void              FillDiametr(ksAPI7::IPropertyListPtr & diamList );
  void              FillDiametr( SArray<double> & diamList, float & curDiam ); 

  // Заполняет список длин
  void              FillLenght(ksAPI7::IPropertyListPtr & lenghtList );
  void              FillLenght( SArray<double> & lenList, float & curDiam ); 
  // Заполняет список длин ввинчиваемого конца
  void              FillScrewEnding(ksAPI7::IPropertyListPtr & screwEndingList );
  void              FillScrewEnding( PArray<_bstr_t>  & screwEndingList ); 
  // Заполняет список класса точности
  void              FillClass( ksAPI7::IPropertyListPtr & classList ); // Заполняет список класса точности 
  void              FillClass( PArray<_bstr_t>  & classList ); 
  // Заполняет список ГОСТов
  void              FillGost( ksAPI7::IPropertyListPtr & gostList );
  void              FillGost( PArray<_bstr_t>  & gostList ); 
  // Заполняет список материалов  
  void              FillMaterial( ksAPI7::IPropertyListPtr & materialList );
  void              FillMaterial( PArray<_bstr_t>  & gostList     ); 

  // Признак изменения параметров
  bool              Changed()                            { return changed; }  
  void              SetChanged( bool newChanged = true ) { changed = newChanged; } 
  // Менеджер свойств ##################################################################

  // ILibPropertyObject - Окно свойств
  // Имя группы объектов по умолчанию макроэлементы
  virtual LPOLESTR _IFUNC GetGroupName();
  // Добавить свойство в список свойств
  virtual BOOL     _IFUNC GetProperty( int index, PropertyParam * param );
  // Обновить параметры свойства
  virtual BOOL     _IFUNC UpdateProperty( PropertyParam * param );
  // Изменить свойойство
  virtual int      _IFUNC ApplyProperty( PropertyParam * param ); 
  // Событие запуска внешнего редактирования для пользовательского свойсва /*ksOPControlExternalEdit*/  
  virtual BOOL     _IFUNC OnChoiceProperty( PropertyParam * param ) { return FALSE; }
  // Обновить параметры свойства
  virtual BOOL     _IFUNC EndEditProperty () { return TRUE; }
  // Выдать идентификатор свойства по индексу
  virtual int             GetPropertyID ( int index );
  // Запрос параметров точек для визуального определения места применения параметра
  virtual BOOL     _IFUNC GetMouseEnterLeavePoint(int propertyId, long pointIndex, LPUNKNOWN parameters) { return false; }

  // Манипулятор ##################################################################
  // ksRotateManipulator - Поворот манипулятора
  virtual bool RotateManipulator         (double X0, double Y0, double Z0,double AxisZX, 
                                          double AxisZXY, double AxisZZ, double angle, bool FromEdit);
  // ksMoveManipulator   - Передвижение манипулятора
  virtual bool MoveManipulator           (long ManipulatorId, double VX, double VY, 
                                          double VZ, double Delta, bool FromEdit);
  // ksClickManipulatorPrimitive - клик по примитиву манипулятора
  virtual bool ClickManipulatorPrimitive (long ManipulatorId, long PrimitiveType, bool DoubleClick) { return false; }
  // ksBeginDragManipulator - начало перетаскивания манипулятора
  virtual bool BeginDragManipulator      (long ManipulatorId, long PrimitiveType);
  // ksEndDragManipulator - окончание перетаскивания манипулятора
  virtual bool EndDragManipulator        (long ManipulatorId, long PrimitiveType);

          bool InitPoint3D( bool visible, bool fix );


protected :   
  bool CheckDir(CString lib);
  bool _upload_list(ksAPI7::IPropertyListPtr& p_property_list, CString lib);
  //CString Get_Corrent_Dir();
  // ksFilterObjects    -Фильтрация объектов
  BOOL FilterObject( LPDISPATCH entity );
  // ksEndProcess    -Окончание процесса.
  BOOL EndProcess();
  void UnadviseEvents();

  // ksEndProcess    -Событие создания объекта в - --подчиненном режиме.
  BOOL TakeObject(LPUNKNOWN obj);
  // ksPlacementChanged    -Изменено положения объекта
  BOOL PlacementChanged(LPDISPATCH Object);
  //Создать временное сопряжение в соответствии с указанной поверхностью
  int  MateProcess( LPDISPATCH entity );
};

//{{AFX_INSERT_LOCATION}}
#endif // !defined(__STUDS3D_H)
