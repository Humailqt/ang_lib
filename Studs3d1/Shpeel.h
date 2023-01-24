#pragma once
#include <ldefin2d.h>
#include "BaseEvent.h"

class PropertyManagerObject
{
protected:
    int                          flagMode;         // ������� �������������� �� ����-�����
    ksAPI7::IProcessParamPtr     procParam;        // ��������� ��������          
    ksAPI7::IPropertyTabsPtr     propTabs;         // ��������
    ksAPI7::IPropertyGridPtr     paramGrid;        // ���� ��� ������ �������������� ���������� �������� 
    ksAPI7::IPropertySlideBoxPtr slideBox;         // ���� ��������� 
    long                         rowIndex;         // ������ ������

public:
    ksAPI7::IPropertyControlsPtr curentCollection; // �������� ������� �������� ������ ������� 
    PropertyManagerObject();
    virtual ~PropertyManagerObject();

    // �������� ������� ##################################################################
      // ������� prButtonClick - ������� ������.
    virtual void OnButtonClick(long buttonID) {}
    // ������� prChangeControlValue - ��������� �������� ��������
    virtual void OnChangeControlValue(long ctrlID, const VARIANT& newVal) {}

    // ������������� ���������� �������� 
    bool InitProcessParam(long toolBarID, SpecPropertyToolBarEnum toolBarType, long firstTabID = 0);

    // ���������� ��������
    bool CreateTab(long tabID, BOOL visible = TRUE, BOOL active = FALSE);
    // ���������� ���������� ������������ �������� 
    ksAPI7::IPropertyListPtr        CreateRealList(double minVal = 0, double maxVal = 0);
    // ���������� ����������        ��������� �������� 
    ksAPI7::IPropertyListPtr        CreateStringList();
    // ���������� ����������        ����� �������� 
    ksAPI7::IPropertyListPtr        CreateIntList();
    // ���������� ���������� 
    ksAPI7::IPropertySeparatorPtr   CreateSeparator(SeparatorTypeEnum type);
    // ���������� CheckBox-� 
    ksAPI7::IPropertyCheckBoxPtr    CreateCheckBox(bool checked);
    // ���������� ������ ������ 
    ksAPI7::IPropertyMultiButtonPtr CreateMultiButton(ButtonTypeEnum type);
    // ���������� ������� � ������
    void AddButton(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, bool cheched = false, bool enable = true);
    // ���������� ������� � ������
    void AddButton2(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, long bmpID, bool cheched = false, bool enable = true);

    // �������� ��������� �� Control �� ��� �������������� (��� ��������� ���������� ��������)
    ksAPI7::IPropertyControlPtr  GetPropertyControl(int ctrlID);
    // �������� ��������� ��������
    void SetControlEnable(long ctrlID, bool enabled = true);

    // ������� �������� ������ �����������
    void InitPropertyControl(ksAPI7::IPropertyControl* control, long ctrlID, UINT hint = 0, UINT tips = 0,
        BOOL enable = TRUE, PropertyControlNameVisibility nameVisibility = ksNameAlwaysVisible,
        BOOL visible = TRUE);

    // ����������� ������� ���������� ��������� ��������
    virtual void ShowControls() {}
    // ������������ ������
    virtual void RedrawPhantom() {}
    virtual void get_part() {}
    virtual IPartPtr& GetPart() {
        IPartPtr i = nullptr;
        return i;
    }
    // ���������� ���������� ��������� � ����� 
    virtual int  ParamCount() { return 0; }
    // ���������� ����� ���������� 
    virtual void ShowParam() {}
    // ������������� ������
    virtual long GetSlideID() { return 0; }

    // ������� ��� ���������� ����� ����������        
    // ���������� ���������� ��������
    void AddStringToGrig(long paramID, LPCTSTR value);
    // ���������� ���������  ��������        
    void AddDoubleToGrig(long paramID, double  value);
    // ksFilterObjects    -���������� ��������
    virtual BOOL FilterObject(LPDISPATCH Object) { return false; };
    // ksProcess3DCreateTakeObject    -������� �������� ������� � - --����������� ������.
    virtual BOOL TakeObject(LPUNKNOWN Object) { return false; };
    // ksPlacementChanged   - �������� ��������� �������
    virtual BOOL PlacementChanged(LPDISPATCH Object) { return false; };
    // ��������� ������� ��������������
    virtual BOOL EndProcess();
    virtual void UnadviseEvents();

    // �������� ������� ##################################################################
};

////////////////////////////////////////////////////////////////////////////////
//
// ����� ������
//
////////////////////////////////////////////////////////////////////////////////
class Shpeel : public PropertyManagerObject
    , public ILibPropertyObject, public Process3DManipulatorsObject
{
private:
    int                                     refCount;
    IDocument3DPtr                               doc; // ��������� �� ��������� �������� 3D-���������
    IPartPtr                                  m_part; // ������
    IEntityPtr                           m_takePoint; // ����� 3D
    bool                              collectChanged; // true - ������ ���������
    CString                                 fileName; // ��� �����
    int                                     flagMode; // ��� ��������������
    ShpeelBase                                 bBase; // ������ � ��
    bool                                     changed; // ������� ��������� ����������
    bool                                    openBase; // ������ � ��
    _variant_t                               Objects;


    ksAPI7::ISpecificationBaseObjectsPtr m_spcBaseObjects;

    ksAPI7::IPlacement3DManipulatorPtr m_manipulator; // ����������� 
    ksAPI7::IBaseManipulatorPtr            m_baseMan; // ������� ��������� ������������ 

    ksAPI7::IProcess3DPtr                m_process3D; // ����� �������� 3D
    ksAPI7::IProcessPtr                    m_process;


    bool                           fixingPart = false; // ���� ����������� �������
    IMateConstraintCollectionPtr              m_mCol; // ��������� ����������

    // ��������� par � tmp ������������ ��� UserParam 
    BaseMakroParam                               par; // ����� ���������
    SHPEEL                                       tmp; // ��������� ���������� ������

    CString patch_lib;

public:
    Shpeel();
    virtual ~Shpeel();

    /* IUnknown */
    virtual unsigned long _IFUNC AddRef();
    virtual unsigned long _IFUNC Release();
    virtual HRESULT    _IFUNC QueryInterface(const GUID far& iid, void far* far* iface);
    virtual HRESULT       _IFUNC LtQueryInterface(int idType, void far* far* iface);

    // �������������
    void               Init();
    // ���������� ��������� �� ������ 
    bool& GetCollectChanged() { return collectChanged; }
    // ���������� ��������� �� ������ ������������
    unsigned short& GetFlagAttr() { return par.flagAttr; }
    // ��� ������� �� ���������� ������� ����������� ������ ���� � ����� ���������� � ���� ������ ���������� �������
    CString& GetFileName();
    // ����������� ����������
    void              SetParam(IPartPtr& pPart);
    // ������ ������
    int               Size() { return sizeof(tmp) + sizeof(par); }
    // �������������� ������� ������������
    reference         EditSpcObj(reference spcObj);
    // ������� �������� ����������� ��������� ��� ���
    bool              IsSpcObjCreate() { return SPCOBJ_CREATE; };
    // ��������� 3D ��������
    void              Draw3D();
    // �������� ��
    int               OpenBase();
    // �������� ��
    void              CloseBase();
    // �������� ���������
    int               GetParam();
    // ��������� ������� ������������
    bool              DrawSpcObj(reference& spcObj);
    // AddRef - �� ������
    IPartPtr& GetPart() { return m_part; }
    // AddRef - �� ������
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
    // ������� � ������ ����������� � �������� ������
    IEntityPtr        GetEntityByName(LPCTSTR name, IPartPtr& p);
    bool              GetDirectionElem() { return true; }

    // �������� ������� ##################################################################
      // ������� prButtonClick - ������� ������.
    virtual void      OnButtonClick(long buttonID);
    // ������� prChangeControlValue - ��������� �������� ��������
    virtual void      OnChangeControlValue(long ctrlID, const VARIANT& newVal);

    // ����������� �������
    virtual void      RedrawPhantom();
    // ����������� ������� ���������� ��������� ��������
    virtual void      ShowControls();
    // ����� ����������
    virtual void      ShowParam();
    // ���������� ���������� ��������� � ����� 
    virtual int       ParamCount() { return PARAM_COUNT; }
    // ������������� ������
    virtual long      GetSlideID();

    // ��������� ������ ���������
    void              FillDiametr(ksAPI7::IPropertyListPtr& diamList);
    void              FillDiametr(SArray<double>& diamList, float& curDiam);

    // ��������� ������ ����
    void              FillLenght(ksAPI7::IPropertyListPtr& lenghtList);
    void              FillLenght(SArray<double>& lenList, float& curDiam);
    // ��������� ������ ���� ������������� �����
    void              FillScrewEnding(ksAPI7::IPropertyListPtr& screwEndingList);
    void              FillScrewEnding(PArray<_bstr_t>& screwEndingList);
    // ��������� ������ ������ ��������
    void              FillClass(ksAPI7::IPropertyListPtr& classList); // ��������� ������ ������ �������� 
    void              FillClass(PArray<_bstr_t>& classList);
    // ��������� ������ ������
    void              FillGost(ksAPI7::IPropertyListPtr& gostList);
    void              FillGost(PArray<_bstr_t>& gostList);
    // ��������� ������ ����������  
    void              FillMaterial(ksAPI7::IPropertyListPtr& materialList);
    void              FillMaterial(PArray<_bstr_t>& gostList);

    // ������� ��������� ����������
    bool              Changed() { return changed; }
    void              SetChanged(bool newChanged = true) { changed = newChanged; }
    // �������� ������� ##################################################################

    // ILibPropertyObject - ���� �������
    // ��� ������ �������� �� ��������� �������������
    virtual LPOLESTR _IFUNC GetGroupName();
    // �������� �������� � ������ �������
    virtual BOOL     _IFUNC GetProperty(int index, PropertyParam* param);
    // �������� ��������� ��������
    virtual BOOL     _IFUNC UpdateProperty(PropertyParam* param);
    // �������� ����������
    virtual int      _IFUNC ApplyProperty(PropertyParam* param);
    // ������� ������� �������� �������������� ��� ����������������� ������� /*ksOPControlExternalEdit*/  
    virtual BOOL     _IFUNC OnChoiceProperty(PropertyParam* param) { return FALSE; }
    // �������� ��������� ��������
    virtual BOOL     _IFUNC EndEditProperty() { return TRUE; }
    // ������ ������������� �������� �� �������
    virtual int             GetPropertyID(int index);
    // ������ ���������� ����� ��� ����������� ����������� ����� ���������� ���������
    virtual BOOL     _IFUNC GetMouseEnterLeavePoint(int propertyId, long pointIndex, LPUNKNOWN parameters) { return false; }

    // ����������� ##################################################################
    // ksRotateManipulator - ������� ������������
    virtual bool RotateManipulator(double X0, double Y0, double Z0, double AxisZX,
        double AxisZXY, double AxisZZ, double angle, bool FromEdit);
    // ksMoveManipulator   - ������������ ������������
    virtual bool MoveManipulator(long ManipulatorId, double VX, double VY,
        double VZ, double Delta, bool FromEdit);
    // ksClickManipulatorPrimitive - ���� �� ��������� ������������
    virtual bool ClickManipulatorPrimitive(long ManipulatorId, long PrimitiveType, bool DoubleClick) { return false; }
    // ksBeginDragManipulator - ������ �������������� ������������
    virtual bool BeginDragManipulator(long ManipulatorId, long PrimitiveType);
    // ksEndDragManipulator - ��������� �������������� ������������
    virtual bool EndDragManipulator(long ManipulatorId, long PrimitiveType);

    bool InitPoint3D(bool visible, bool fix);


protected:
    bool CheckDir(CString lib);
    bool _upload_list(ksAPI7::IPropertyListPtr& p_property_list, CString lib);
    //CString Get_Corrent_Dir();
    // ksFilterObjects    -���������� ��������
    BOOL FilterObject(LPDISPATCH entity);
    // ksEndProcess    -��������� ��������.
    BOOL EndProcess();
    void UnadviseEvents();

    // ksEndProcess    -������� �������� ������� � - --����������� ������.
    BOOL TakeObject(LPUNKNOWN obj);
    // ksPlacementChanged    -�������� ��������� �������
    BOOL PlacementChanged(LPDISPATCH Object);
    //������� ��������� ���������� � ������������ � ��������� ������������
    int  MateProcess(LPDISPATCH entity);
};
