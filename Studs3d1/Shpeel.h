#pragma once
#include <ldefin2d.h>
#include "BaseEvent.h"

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
    ksAPI7::IPropertyControlsPtr curentCollection; // Контролы текущей закладки панели свойств 
    PropertyManagerObject();
    virtual ~PropertyManagerObject();

    // Менеджер свойств ##################################################################
      // Событие prButtonClick - Нажатие кнопки.
    virtual void OnButtonClick(long buttonID) {}
    // Событие prChangeControlValue - Изменение значения контрола
    virtual void OnChangeControlValue(long ctrlID, const VARIANT& newVal) {}

    // Инициализация параметров процесса 
    bool InitProcessParam(long toolBarID, SpecPropertyToolBarEnum toolBarType, long firstTabID = 0);

    // Добавление закладки
    bool CreateTab(long tabID, BOOL visible = TRUE, BOOL active = FALSE);
    // Добавление комбобокса вещественных значений 
    ksAPI7::IPropertyListPtr        CreateRealList(double minVal = 0, double maxVal = 0);
    // Добавление комбобокса        строковых значений 
    ksAPI7::IPropertyListPtr        CreateStringList();
    // Добавление комбобокса        целых значений 
    ksAPI7::IPropertyListPtr        CreateIntList();
    // Добавление сепаратора 
    ksAPI7::IPropertySeparatorPtr   CreateSeparator(SeparatorTypeEnum type);
    // Добавление CheckBox-а 
    ksAPI7::IPropertyCheckBoxPtr    CreateCheckBox(bool checked);
    // Добавление группы кнопок 
    ksAPI7::IPropertyMultiButtonPtr CreateMultiButton(ButtonTypeEnum type);
    // Добавление кнопоки в группу
    void AddButton(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, bool cheched = false, bool enable = true);
    // Добавление кнопоки в группу
    void AddButton2(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, long bmpID, bool cheched = false, bool enable = true);

    // Получить указатель на Control по его идентификатору (для изменения параметров контрола)
    ksAPI7::IPropertyControlPtr  GetPropertyControl(int ctrlID);
    // Изменить состояние контрола
    void SetControlEnable(long ctrlID, bool enabled = true);

    // Накачка контрола общими параметрами
    void InitPropertyControl(ksAPI7::IPropertyControl* control, long ctrlID, UINT hint = 0, UINT tips = 0,
        BOOL enable = TRUE, PropertyControlNameVisibility nameVisibility = ksNameAlwaysVisible,
        BOOL visible = TRUE);

    // Виртуальная функция наполнения контролов элемента
    virtual void ShowControls() {}
    // Перерисовать фантом
    virtual void RedrawPhantom() {}
    virtual void get_part() {}
    virtual IPartPtr& GetPart() {
        IPartPtr i = nullptr;
        return i;
    }
    // Количество параметров выводимых в гриде 
    virtual int  ParamCount() { return 0; }
    // Заполнение грида параметров 
    virtual void ShowParam() {}
    // Идентификатор слайда
    virtual long GetSlideID() { return 0; }

    // Функции для заполнения грида параметров        
    // Добавление строкового значения
    void AddStringToGrig(long paramID, LPCTSTR value);
    // Добавление числового  значения        
    void AddDoubleToGrig(long paramID, double  value);
    // ksFilterObjects    -Фильтрация объектов
    virtual BOOL FilterObject(LPDISPATCH Object) { return false; };
    // ksProcess3DCreateTakeObject    -Событие создания объекта в - --подчиненном режиме.
    virtual BOOL TakeObject(LPUNKNOWN Object) { return false; };
    // ksPlacementChanged   - Изменено положения объекта
    virtual BOOL PlacementChanged(LPDISPATCH Object) { return false; };
    // Завершить процесс редактирования
    virtual BOOL EndProcess();
    virtual void UnadviseEvents();

    // Менеджер свойств ##################################################################
};

////////////////////////////////////////////////////////////////////////////////
//
// Класс шпилек
//
////////////////////////////////////////////////////////////////////////////////
class Shpeel : public PropertyManagerObject
    , public ILibPropertyObject, public Process3DManipulatorsObject
{
private:
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


    bool                           fixingPart = false; // Флаг закрепления объекта
    IMateConstraintCollectionPtr              m_mCol; // Коллекция сопряжений

    // Структуры par и tmp записываемые как UserParam 
    BaseMakroParam                               par; // Макро параметры
    SHPEEL                                       tmp; // Структура параметров шпилек

    CString patch_lib;

public:
    Shpeel();
    virtual ~Shpeel();

    /* IUnknown */
    virtual unsigned long _IFUNC AddRef();
    virtual unsigned long _IFUNC Release();
    virtual HRESULT    _IFUNC QueryInterface(const GUID far& iid, void far* far* iface);
    virtual HRESULT       _IFUNC LtQueryInterface(int idType, void far* far* iface);

    // Инициализация
    void               Init();
    // Возвращает изменился ли массив 
    bool& GetCollectChanged() { return collectChanged; }
    // Возвращает создавать ли объект спецификации
    unsigned short& GetFlagAttr() { return par.flagAttr; }
    // При вставке из библиотеки моделей указывается полный путь к файлу библиотеки и путь внутри библиотеки моделей
    CString& GetFileName();
    // Редактируем переменные
    void              SetParam(IPartPtr& pPart);
    // Размер данных
    int               Size() { return sizeof(tmp) + sizeof(par); }
    // Редактирование объекта спецификации
    reference         EditSpcObj(reference spcObj);
    // Функция проверки подключения атрибутов или нет
    bool              IsSpcObjCreate() { return SPCOBJ_CREATE; };
    // Рисование 3D объектов
    void              Draw3D();
    // Открытие БД
    int               OpenBase();
    // Закрытие БД
    void              CloseBase();
    // Получить параметры
    int               GetParam();
    // Отрисовка объекта спецификации
    bool              DrawSpcObj(reference& spcObj);
    // AddRef - не делает
    IPartPtr& GetPart() { return m_part; }
    // AddRef - не делает
    IDocument3DPtr& GetDoc() { return doc; }
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
    IEntityPtr        GetEntityByName(LPCTSTR name, IPartPtr& p);
    bool              GetDirectionElem() { return true; }

    // Менеджер свойств ##################################################################
      // Событие prButtonClick - Нажатие кнопки.
    virtual void      OnButtonClick(long buttonID);
    // Событие prChangeControlValue - Изменение значения контрола
    virtual void      OnChangeControlValue(long ctrlID, const VARIANT& newVal);

    // Перерисовка фантома
    virtual void      RedrawPhantom();
    // Виртуальная функция наполнения контролов элемента
    virtual void      ShowControls();
    // Вывод параметров
    virtual void      ShowParam();
    // Количество параметров выводимых в гриде 
    virtual int       ParamCount() { return PARAM_COUNT; }
    // Идентификатор слайда
    virtual long      GetSlideID();

    // Заполняет список диаметров
    void              FillDiametr(ksAPI7::IPropertyListPtr& diamList);
    void              FillDiametr(SArray<double>& diamList, float& curDiam);

    // Заполняет список длин
    void              FillLenght(ksAPI7::IPropertyListPtr& lenghtList);
    void              FillLenght(SArray<double>& lenList, float& curDiam);
    // Заполняет список длин ввинчиваемого конца
    void              FillScrewEnding(ksAPI7::IPropertyListPtr& screwEndingList);
    void              FillScrewEnding(PArray<_bstr_t>& screwEndingList);
    // Заполняет список класса точности
    void              FillClass(ksAPI7::IPropertyListPtr& classList); // Заполняет список класса точности 
    void              FillClass(PArray<_bstr_t>& classList);
    // Заполняет список ГОСТов
    void              FillGost(ksAPI7::IPropertyListPtr& gostList);
    void              FillGost(PArray<_bstr_t>& gostList);
    // Заполняет список материалов  
    void              FillMaterial(ksAPI7::IPropertyListPtr& materialList);
    void              FillMaterial(PArray<_bstr_t>& gostList);

    // Признак изменения параметров
    bool              Changed() { return changed; }
    void              SetChanged(bool newChanged = true) { changed = newChanged; }
    // Менеджер свойств ##################################################################

    // ILibPropertyObject - Окно свойств
    // Имя группы объектов по умолчанию макроэлементы
    virtual LPOLESTR _IFUNC GetGroupName();
    // Добавить свойство в список свойств
    virtual BOOL     _IFUNC GetProperty(int index, PropertyParam* param);
    // Обновить параметры свойства
    virtual BOOL     _IFUNC UpdateProperty(PropertyParam* param);
    // Изменить свойойство
    virtual int      _IFUNC ApplyProperty(PropertyParam* param);
    // Событие запуска внешнего редактирования для пользовательского свойсва /*ksOPControlExternalEdit*/  
    virtual BOOL     _IFUNC OnChoiceProperty(PropertyParam* param) { return FALSE; }
    // Обновить параметры свойства
    virtual BOOL     _IFUNC EndEditProperty() { return TRUE; }
    // Выдать идентификатор свойства по индексу
    virtual int             GetPropertyID(int index);
    // Запрос параметров точек для визуального определения места применения параметра
    virtual BOOL     _IFUNC GetMouseEnterLeavePoint(int propertyId, long pointIndex, LPUNKNOWN parameters) { return false; }

    // Манипулятор ##################################################################
    // ksRotateManipulator - Поворот манипулятора
    virtual bool RotateManipulator(double X0, double Y0, double Z0, double AxisZX,
        double AxisZXY, double AxisZZ, double angle, bool FromEdit);
    // ksMoveManipulator   - Передвижение манипулятора
    virtual bool MoveManipulator(long ManipulatorId, double VX, double VY,
        double VZ, double Delta, bool FromEdit);
    // ksClickManipulatorPrimitive - клик по примитиву манипулятора
    virtual bool ClickManipulatorPrimitive(long ManipulatorId, long PrimitiveType, bool DoubleClick) { return false; }
    // ksBeginDragManipulator - начало перетаскивания манипулятора
    virtual bool BeginDragManipulator(long ManipulatorId, long PrimitiveType);
    // ksEndDragManipulator - окончание перетаскивания манипулятора
    virtual bool EndDragManipulator(long ManipulatorId, long PrimitiveType);

    bool InitPoint3D(bool visible, bool fix);


protected:
    bool CheckDir(CString lib);
    bool _upload_list(ksAPI7::IPropertyListPtr& p_property_list, CString lib);
    //CString Get_Corrent_Dir();
    // ksFilterObjects    -Фильтрация объектов
    BOOL FilterObject(LPDISPATCH entity);
    // ksEndProcess    -Окончание процесса.
    BOOL EndProcess();
    void UnadviseEvents();

    // ksEndProcess    -Событие создания объекта в - --подчиненном режиме.
    BOOL TakeObject(LPUNKNOWN obj);
    // ksPlacementChanged    -Изменено положения объекта
    BOOL PlacementChanged(LPDISPATCH Object);
    //Создать временное сопряжение в соответствии с указанной поверхностью
    int  MateProcess(LPDISPATCH entity);
};
