////////////////////////////////////////////////////////////////////////////////
//
// studs3d.h - ��������� ������� �� Visual C++ - �������
// ��������� ���������� � ����� ������
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
// ����� ���������
// ---
#define AXIS_OFF     0x1    // FALSE - ��������, TRUE - �� ��������
#define PITCH        0x2    // FALSE - ������� ���, TRUE - ������ ���
#define SIMPLE       0x10   // FALSE - ���������� ���������, TRUE - ���������� ���������
#define ISPOLN       0x80   // FALSE - ���������� 1, TRUE - ���������� 2
#define TAKEISPOLN   0x100  // FALSE - �� ��������� ����������, TRUE - ��������� ����������
#define ALLST        0x4    // FALSE - ������ �� b, TRUE - ������ �� �������
#define KONTR        0x8    // FALSE - ������� �������, TRUE - ������� ��� ������������ ���������
#define SECOFF       0x10   // FALSE - ������� ��������, TRUE - ������� ��������, ��� ������, ��� �������������
#define TAKEKONTR    0x20   // FALSE - �� ��������� ���������, TRUE - ��������� ���������
#define REVERCE_ON   0x20   // FALSE - �������� ������, TRUE - �������� ���������
#define TAKEPODG     0x40   // FALSE - �� ��������� ����������, TRUE - ��������� ����������
#define LEFT_DRAW    0x40   // TRUE - �������� �����
#define FLAGLENGTH   0x200  // FALSE - ����� ����� ��������� ���������, TRUE - �� ���������
#define ISPOLN3      0x200  // ��� ������� FALSE - ���������� 2, TRUE - ���������� 3
#define REVERCE_DRAW 0x200  // TRUE - �������� ���������, FALSE - ������ ���������
#define SBORSHAYBA   0x400  // FALSE - ����� ��� �����, TRUE - ����� ������ � ������
#define PITCHOFF     0x800  // FALSE - ���� ������� � ������ ���, TRUE - ���� ������ �������
#define LENLEN       0x1000 // FALSE - ���������� ����� ������, TRUE - ����������
#define LENLENOFF    0x2000 // FALSE - �� ���������, TRUE - ���������

//-------------------------------------------------------------------------------
// �������������� ����� 
// ---
#define KEY_S_ON     0x1   // TRUE - �������� �������������� ������� ��� ����, FALSE - ���������
#define KOEFF_MAT_ON 0x4   // FALSE - ������ ������, TRUE ������ ���������
#define KEY_S        0x2   // TRUE - �������������� ������ ��� ����, FALSE - ������� ������ ��� ���� 
#define KEY_S_GRAY   0x8   // TRUE - ������ "�������������� ������� ��� ����"
#define DRAW_ST_OF   0x10  // FALSE - �������� � ����� �������� TRUE - �������� ������ �������

//-------------------------------------------------------------------------------
// ���������� ����������
// ---
#define COUNT_MASSA      1000 // ���������� ������� ����� ������� ����� � ����-�
#define STANDART_SECTION 25   // ����� ������� � ���������� ������������
#define SPC_NAME          5    // ������� ������������

#define EPSILON          0.001  

#define STUDS_FILE       _T("stud.l3d")           // ���������� �������
#define STUDS_INPUT_PATH _T("|�������|�������_1") // ���� ������ ���������� �������

#define SPCOBJ_CREATE    TRUE // ������� ����������� ������� � ������������
#define PARAM_COUNT      5    // ���������� ���������� ��������� � ����� 

#define MAX_LENGTH 1000       // ������������ ����� �������
#define MIN_LENGTH 0          // ����������� ����� �������

////////////////////////////////////////////////////////////////////////////////
//
// ��������� ���������� � ����� ������
//
////////////////////////////////////////////////////////////////////////////////

#pragma pack( push, 1)
//------------------------------------------------------------------------------
// ��������� ���������� ������
// ---                      
struct SHPEEL 
{
  float d;          // ������� ������
  float p1;         // ��� ������
  float p2;         // ��� ������
  float b1;         // ������������ ����� 
  float c;          // ������ �����
  float l;          // ����� �������
  float b;          // ������� ����� 
  short f;          // ������� �����
  short klass;      // ����� ��������
  short gost;       // ����� �����
  short ver;        // ������ �����
  float m1;         // ����� 1 ����������
  float m2;         // ����� 2 ����������
  short indexMassa; // 0 - ������ 1 - ������ ����� 3 - ������ 2 - ������
};


////////////////////////////////////////////////////////////////////////////////
//
//  ��������� ��� ������ � ��
//
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
// ��� ���������� �� ��� st22032.loa ... st22043.loa
// ---
struct ShpeelSTmp 
{
  short t;   // ���� ������������ ������ ��� ���
  float l;   // ����� �������
  float d;   // ������� ������
  float b;   // ������� �����
  float m1;  // ����� ���������� 1
  float m2;  // ����� ���������� 2
};


//------------------------------------------------------------------------------
// ��� ����������  ��  ��� 22032.loa ... 22043.loa
// ---
struct ShpeelTmp 
{
  short t;   // ���� ������������ ������ ��� ���
  float d;   // ������� ������
  float p1;  // ��� ������
  float p2;  // ��� ������
  float b1;  // ������������ �����
  float c;   // ������ �����
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
// ������ � ��
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
// ����� ���������
// ---
struct BaseMakroParam 
{
  float          ang;
  unsigned short flagAttr;
  short          drawType;
  BYTE           typeSwitch;  // ��� ������� ��������� ������� ����� ��������
                              // 0 - ����� � ����������� ��� 0X ( Placement )
                              // 1 - �����, ����������� ��������� � ���� 0X ������� �� ( Cursor )
};

struct info_list
{
    _variant_t id;
    unsigned int order_id;

};

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// ����� ������
//
////////////////////////////////////////////////////////////////////////////////
extern class Shpeel : public PropertyManagerObject
             , public ILibPropertyObject, public Process3DManipulatorsObject
{
private :
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


  bool                           fixingPart  = false; // ���� ����������� �������
  IMateConstraintCollectionPtr              m_mCol; // ��������� ����������

  // ��������� par � tmp ������������ ��� UserParam 
  BaseMakroParam                               par; // ����� ���������
  SHPEEL                                       tmp; // ��������� ���������� ������

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

  // �������������
  void               Init();                                          
  // ���������� ��������� �� ������ 
  bool &            GetCollectChanged() { return collectChanged; }   
  // ���������� ��������� �� ������ ������������
  unsigned short &  GetFlagAttr() { return par.flagAttr; }           
  // ��� ������� �� ���������� ������� ����������� ������ ���� � ����� ���������� � ���� ������ ���������� �������
  CString &         GetFileName();                                   
  // ����������� ����������
  void              SetParam( IPartPtr& pPart );                                      
  // ������ ������
  int               Size() { return sizeof(tmp) + sizeof(par); }     
  // �������������� ������� ������������
  reference         EditSpcObj( reference spcObj );                  
  // ������� �������� ����������� ��������� ��� ���
  bool              IsSpcObjCreate() { return SPCOBJ_CREATE; };      
  // ��������� 3D ��������
  void              Draw3D( );                          
  // �������� ��
  int               OpenBase();                                      
  // �������� ��
  void              CloseBase();                                     
  // �������� ���������
  int               GetParam();                                      
  // ��������� ������� ������������
  bool              DrawSpcObj( reference & spcObj );                                    
  // AddRef - �� ������
#define DEBUG_GetPart 1
#if DEBUG_GetPart
#define DEBUG_GetPart_ 1 
#endif // DEBUG_GET_VALUE_FROM_LIST

  IPartPtr&         GetPart()   { return m_part; }
  // AddRef - �� ������
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
  // ������� � ������ ����������� � �������� ������
  IEntityPtr        GetEntityByName( LPCTSTR name, IPartPtr& p );
  bool              GetDirectionElem() { return true; }           
  
// �������� ������� ##################################################################
  // ������� prButtonClick - ������� ������.
  virtual void      OnButtonClick( long buttonID );     
  // ������� prChangeControlValue - ��������� �������� ��������
  virtual void      OnChangeControlValue( long ctrlID, const VARIANT& newVal );
  
  // ����������� �������
  virtual void      RedrawPhantom();    
  // ����������� ������� ���������� ��������� ��������
  virtual void      ShowControls();
  // ����� ����������
  virtual void      ShowParam();  
  // ���������� ���������� ��������� � ����� 
  virtual int       ParamCount(){ return PARAM_COUNT; }
  // ������������� ������
  virtual long      GetSlideID();

  // ��������� ������ ���������
  void              FillDiametr(ksAPI7::IPropertyListPtr & diamList );
  void              FillDiametr( SArray<double> & diamList, float & curDiam ); 

  // ��������� ������ ����
  void              FillLenght(ksAPI7::IPropertyListPtr & lenghtList );
  void              FillLenght( SArray<double> & lenList, float & curDiam ); 
  // ��������� ������ ���� ������������� �����
  void              FillScrewEnding(ksAPI7::IPropertyListPtr & screwEndingList );
  void              FillScrewEnding( PArray<_bstr_t>  & screwEndingList ); 
  // ��������� ������ ������ ��������
  void              FillClass( ksAPI7::IPropertyListPtr & classList ); // ��������� ������ ������ �������� 
  void              FillClass( PArray<_bstr_t>  & classList ); 
  // ��������� ������ ������
  void              FillGost( ksAPI7::IPropertyListPtr & gostList );
  void              FillGost( PArray<_bstr_t>  & gostList ); 
  // ��������� ������ ����������  
  void              FillMaterial( ksAPI7::IPropertyListPtr & materialList );
  void              FillMaterial( PArray<_bstr_t>  & gostList     ); 

  // ������� ��������� ����������
  bool              Changed()                            { return changed; }  
  void              SetChanged( bool newChanged = true ) { changed = newChanged; } 
  // �������� ������� ##################################################################

  // ILibPropertyObject - ���� �������
  // ��� ������ �������� �� ��������� �������������
  virtual LPOLESTR _IFUNC GetGroupName();
  // �������� �������� � ������ �������
  virtual BOOL     _IFUNC GetProperty( int index, PropertyParam * param );
  // �������� ��������� ��������
  virtual BOOL     _IFUNC UpdateProperty( PropertyParam * param );
  // �������� ����������
  virtual int      _IFUNC ApplyProperty( PropertyParam * param ); 
  // ������� ������� �������� �������������� ��� ����������������� ������� /*ksOPControlExternalEdit*/  
  virtual BOOL     _IFUNC OnChoiceProperty( PropertyParam * param ) { return FALSE; }
  // �������� ��������� ��������
  virtual BOOL     _IFUNC EndEditProperty () { return TRUE; }
  // ������ ������������� �������� �� �������
  virtual int             GetPropertyID ( int index );
  // ������ ���������� ����� ��� ����������� ����������� ����� ���������� ���������
  virtual BOOL     _IFUNC GetMouseEnterLeavePoint(int propertyId, long pointIndex, LPUNKNOWN parameters) { return false; }

  // ����������� ##################################################################
  // ksRotateManipulator - ������� ������������
  virtual bool RotateManipulator         (double X0, double Y0, double Z0,double AxisZX, 
                                          double AxisZXY, double AxisZZ, double angle, bool FromEdit);
  // ksMoveManipulator   - ������������ ������������
  virtual bool MoveManipulator           (long ManipulatorId, double VX, double VY, 
                                          double VZ, double Delta, bool FromEdit);
  // ksClickManipulatorPrimitive - ���� �� ��������� ������������
  virtual bool ClickManipulatorPrimitive (long ManipulatorId, long PrimitiveType, bool DoubleClick) { return false; }
  // ksBeginDragManipulator - ������ �������������� ������������
  virtual bool BeginDragManipulator      (long ManipulatorId, long PrimitiveType);
  // ksEndDragManipulator - ��������� �������������� ������������
  virtual bool EndDragManipulator        (long ManipulatorId, long PrimitiveType);

          bool InitPoint3D( bool visible, bool fix );


protected :   
  bool CheckDir(CString lib);
  bool _upload_list(ksAPI7::IPropertyListPtr& p_property_list, CString lib);
  //CString Get_Corrent_Dir();
  // ksFilterObjects    -���������� ��������
  BOOL FilterObject( LPDISPATCH entity );
  // ksEndProcess    -��������� ��������.
  BOOL EndProcess();
  void UnadviseEvents();

  // ksEndProcess    -������� �������� ������� � - --����������� ������.
  BOOL TakeObject(LPUNKNOWN obj);
  // ksPlacementChanged    -�������� ��������� �������
  BOOL PlacementChanged(LPDISPATCH Object);
  //������� ��������� ���������� � ������������ � ��������� ������������
  int  MateProcess( LPDISPATCH entity );
};

//{{AFX_INSERT_LOCATION}}
#endif // !defined(__STUDS3D_H)
