////////////////////////////////////////////////////////////////////////////////
//
// PropertyManagerObject - ����� ��� ������������ ������ ������� 
// PropertyManagerEvent  - ����� ������� ��������� ������� 
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
// ������� ��� ������ ��������� ����������
// ---
int LibMessage( int     strId, int flags ); // ��������� ����������� �� ��������
int LibMessage( LPCTSTR str,   int flags ); // ��������� ���������� ������� 


//-------------------------------------------------------------------------------
// ��� ��������� COM ����������
// ---
void DumpError(_com_error& e);              // ����� ������ ���������� 
void ShowError();                           // ����� ��������� ��� ��������������� ������� HRESULT 

#ifndef InsertPartPtr
#define InsertPartPtr std::shared_ptr<InsertPart> 
#endif // !InsertPartPtr


// �������� ������������ �����
_bstr_t GetLibraryIconFont();

////////////////////////////////////////////////////////////////////////////////
//
// ����� ��� ������������ ������ ������� 
//
// ctrlID ������������ ��� ������������� �������� � ���
// ������������� ��� ��������  �������� ��������
//
////////////////////////////////////////////////////////////////////////////////
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

  ///////////////////////////////////////////////////////////////////////////////
    bool rotated = false;
    double angle = 90;
    //double x, y, z=20;
    //double h = 20, w=20;
  ///////////////////////////////////////////////////////////////////////////////
  ksAPI7::IPropertyControlsPtr curentCollection; // �������� ������� �������� ������ ������� 
           PropertyManagerObject();
  virtual ~PropertyManagerObject();

// �������� ������� ##################################################################
  // ������� prButtonClick - ������� ������.
  virtual void OnButtonClick       ( long buttonID )                     {}     
  // ������� prChangeControlValue - ��������� �������� ��������
  virtual void OnChangeControlValue( long ctrlID, const VARIANT& newVal ){}
  virtual InsertPartPtr get_part_info() { return NULL; }
  /////////////////////////////////////////////////////////////

  virtual int load_default_panel() { return 1; };
  std::string get_tmp_filename_tmp(IDocument3DPtr doc) { return std::string(""); };

  /////////////////////////////////////////////////////////////

  // ������������� ���������� �������� 
  bool InitProcessParam( long toolBarID, SpecPropertyToolBarEnum toolBarType, long firstTabID = 0 );
  
  // ���������� ��������
  bool CreateTab( long tabID, BOOL visible = TRUE, BOOL active = FALSE ); 
  // ���������� ���������� ������������ �������� 
  ksAPI7::IPropertyListPtr        CreateRealList   ( double minVal = 0, double maxVal = 0 );
  // ���������� ����������        ��������� �������� 
  ksAPI7::IPropertyListPtr        CreateStringList ();
  // ���������� ����������        ����� �������� 
  ksAPI7::IPropertyListPtr        CreateIntList    ();
  // ���������� ���������� 
  ksAPI7::IPropertySeparatorPtr   CreateSeparator  ( SeparatorTypeEnum type );
  // ���������� CheckBox-� 
  ksAPI7::IPropertyCheckBoxPtr    CreateCheckBox   ( bool checked );
  // ���������� ������ ������ 
  ksAPI7::IPropertyMultiButtonPtr CreateMultiButton( ButtonTypeEnum type );
  // ���������� ������� � ������
  void AddButton(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, bool cheched = false,  bool enable = true );
  // ���������� ������� � ������
  void AddButton2(ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, long bmpID, bool cheched = false,  bool enable = true );

  // �������� ��������� �� Control �� ��� �������������� (��� ��������� ���������� ��������)
  ksAPI7::IPropertyControlPtr  GetPropertyControl( int ctrlID );
  // �������� ��������� ��������
  void SetControlEnable( long ctrlID, bool enabled = true ); 

  // ������� �������� ������ �����������
  void InitPropertyControl(ksAPI7::IPropertyControl* control, long ctrlID, UINT hint = 0, UINT tips = 0,
                            BOOL enable = TRUE, PropertyControlNameVisibility nameVisibility = ksNameAlwaysVisible, 
                            BOOL visible = TRUE );

  // ����������� ������� ���������� ��������� ��������
  virtual void ShowControls (){} 
  virtual unsigned int get_order_control(variant_t ID) { return -1; };
  // ������������ ������
  virtual void RedrawPhantom(){}
  virtual void get_part(){}
  virtual IPartPtr& GetPart() { 
      IPartPtr i = NULL;
      return i; }
  // ���������� ���������� ��������� � ����� 
  virtual int  ParamCount(){ return 0; }
  // ���������� ����� ���������� 
  virtual void ShowParam (){}
  // ������������� ������
  virtual long GetSlideID(){ return 0; }

  // ������� ��� ���������� ����� ����������        
  // ���������� ���������� ��������
  void AddStringToGrig( long paramID, LPCTSTR value ); 
  // ���������� ���������  ��������        
  void AddDoubleToGrig( long paramID, double  value ); 
  // ksFilterObjects    -���������� ��������
  virtual BOOL FilterObject    (LPDISPATCH Object) { return false; };
  // ksProcess3DCreateTakeObject    -������� �������� ������� � - --����������� ������.
  virtual BOOL TakeObject      (LPUNKNOWN Object)  { return false; };
  // ksPlacementChanged   - �������� ��������� �������
  virtual BOOL PlacementChanged(LPDISPATCH Object) { return false; };
  // ��������� ������� ��������������
  virtual BOOL EndProcess();
  virtual void UnadviseEvents();

// �������� ������� ##################################################################
};

////////////////////////////////////////////////////////////////////////////////
//
// PropertyManagerEvent - ���������� ������� �� ��������� ������ �������
//
////////////////////////////////////////////////////////////////////////////////
class PropertyManagerEvent : public BaseEvent
{
protected:

public:
  PropertyManagerObject&  obj; // ��������� �� ����������� ������
  PropertyManagerEvent( LPDISPATCH manager, PropertyManagerObject& _obj );           
  virtual ~PropertyManagerEvent();



  void draw_panel(IDocument3DPtr doc, IPart* part/*,CStringW str*/);


public: 
  // prChangeControlValue - ������� ��������� �������� �������� 
  afx_msg BOOL ButtonClick       ( long buttonID );  
  // prChangeControlValue - ������� ��������� �������� �������� 
  afx_msg BOOL ChangeControlValue( LPDISPATCH ctrl );
  // prControlCommand - ������� ������ ��������
  afx_msg BOOL ControlCommand    ( LPDISPATCH  ctrl, long buttonID );
  // CLLBACK ��� ��������� ���� ������������ ����
  afx_msg BOOL GetContextMenuType( long LX, long LY, long * ContextMenuType );  // LX LY ��� �������� ����������
  // CLLBACK ��� ������� ����������� ������
  afx_msg BOOL FillContextPanel  ( LPDISPATCH ContextPanel );
  DECLARE_EVENTSINK_MAP()
};

////////////////////////////////////////////////////////////////////////////////
//
// _2_ Process3DEvent - ���������� ������� �� �������� 3D
//
////////////////////////////////////////////////////////////////////////////////
class Process3DEvent : public BaseEvent
{
protected:
  PropertyManagerObject& m_obj;           // ��������� �� ����������� ������
  // Attributes
public:
  Process3DEvent(const ksAPI7::IProcess3DPtr& process, PropertyManagerObject& _obj, bool copyProc = false);
  virtual ~Process3DEvent();
  
  // Operations
public:
  // ksFilterObjects -���������� ��������
  afx_msg BOOL FilterObject    ( LPDISPATCH  Object );
	// ksProcess3DCreateTakeObject  -������� �������� ������� � - --����������� ������.
  afx_msg BOOL TakeObject      ( LPUNKNOWN   Object );
   //  ksPlacementChanged - �������� ��������� �������
  afx_msg BOOL PlacementChanged( LPDISPATCH  Object );
	// ksEndProcess    -��������� ��������.
  afx_msg BOOL EndProcess      ();
	// ksStop     -��������� ��������
  afx_msg BOOL Stop            ();
	// ksRun     -������ ��������
  afx_msg BOOL Run             ();
  DECLARE_EVENTSINK_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 


////////////////////////////////////////////////////////////////////////////////
//
/// ����� ��� ���������� ������� ������������
//
////////////////////////////////////////////////////////////////////////////////
class Process3DManipulatorsObject 
{
public:
  // ksRotateManipulator - ������� ������������
  virtual bool RotateManipulator(double X0, double Y0, double Z0, double AxisZX, double AxisZXY,
  	                             double AxisZZ, double angle, bool FromEdit    )                     { return false; };
  // ksMoveManipulator   - ������������ ������������
  virtual bool MoveManipulator          (long ManipulatorId, double VX, double VY, 
  	                                     double VZ, double Delta, bool FromEdit)                     { return false; };
  // ksClickManipulatorPrimitive - ���� �� ��������� ������������
  virtual bool ClickManipulatorPrimitive(long ManipulatorId, long PrimitiveType, bool DoubleClick)   { return false; };
  // ksBeginDragManipulator - ������ �������������� ������������
  virtual bool BeginDragManipulator     (long ManipulatorId, long PrimitiveType)                     { return false; };
  // ksEndDragManipulator - ��������� �������������� ������������
  virtual bool EndDragManipulator       (long ManipulatorId, long PrimitiveType)                     { return false; };
  
  virtual ~Process3DManipulatorsObject() {};
};


////////////////////////////////////////////////////////////////////////////////
//
// _3_ Process3DManipulatorsEvent - ���������� ������� �� ������������� 3D
//
////////////////////////////////////////////////////////////////////////////////
class Process3DManipulatorsEvent : public BaseEvent
{
protected:
  Process3DManipulatorsObject& m_obj;             // ��������� �� ����������� ������
  // Attributes
public:
  Process3DManipulatorsEvent(const ksAPI7::IManipulatorsPtr manipulators, Process3DManipulatorsObject& _obj, bool copyProc = false);
  virtual ~Process3DManipulatorsEvent();
  
  //Operations
public:
  // ksRotateManipulator - ������� ������������
  BOOL RotateManipulator         (long ManipulatorId, double X0, double Y0, double Z0, double AxisZX,double AxisZXY, double AxisZZ, double angle, BOOL FromEdit);
  // ksMoveManipulator   - ������������ ������������
  BOOL MoveManipulator           (long ManipulatorId, double VX, double VY, double VZ, double Delta ,BOOL FromEdit);
  // ksClickManipulatorPrimitive - ���� �� ��������� ������������
  BOOL ClickManipulatorPrimitive (long ManipulatorId, long PrimitiveType, BOOL DoubleClick);
  // ksBeginDragManipulator - ������ �������������� ������������
  BOOL BeginDragManipulator      (long ManipulatorId, long PrimitiveType);
  // ksEndDragManipulator   - ��������� �������������� ������������
  BOOL EndDragManipulator        (long ManipulatorId, long PrimitiveType);
  DECLARE_EVENTSINK_MAP();
};
  
class Shpeel;
class PropertyManagerObject;

CString get_value_from_list(Shpeel& shpeel, long id_control);
CString get_value_from_list(PropertyManagerObject& shpeel, long id_control);
CString get_value_from_list(Shpeel* shpeel, long id_control);
