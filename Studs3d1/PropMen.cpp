////////////////////////////////////////////////////////////////////////////////
//
// ����� ��� ������ � ������� �������
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
#define LIB_HELP _T("Gayka.hlp") // ���� ������


//------------------------------------------------------------------------------
//
// ---
LPCTSTR DoubleToStr( double value );      // ��������������� �������, ������� �������� � ������
LPTSTR LoadStr( int id );                 // ��������������� �������, ��������� ������ �� �������
int LibMessage( LPCTSTR str, int flags ); // ��������������� �������, ����� ���������
int LibMessage( int strId, int flags );   // ��������������� �������, ����� ���������
void DumpError( _com_error& e );          // ��������������� �������, ������������ ���������� �� ������
void ShowError();                         // ��������������� �������, ������������ ���������� �� ������


//----------------------------------------------------------------------------------------------
// �����������
// ---
PropertyManagerObject::PropertyManagerObject() :
  procParam        (NULL), // ��������� ��������
  propTabs         (NULL), // ��������� ��������
  curentCollection (NULL), // ��������� ��������� 
  paramGrid        (NULL), // ���� ��� ������ �������������� ���������� �������� 
  slideBox         (NULL), // ���� ��������� 
  rowIndex         (0),    // ������ ������
  flagMode         (0)     // ������� �������������� �� ����-�����
{ 
}     


//----------------------------------------------------------------------------------------------
// ����������
// ---
PropertyManagerObject::~PropertyManagerObject() 
{  
  paramGrid        = NULL; // ���� ��� ������ �������������� ���������� �������� 
  slideBox         = NULL; // ���� ��������� 
  curentCollection = NULL; // ��������� ��������� 
  propTabs         = NULL; // ��������� ��������
  procParam        = NULL; // ��������� ��������
  rowIndex         = 0;    // ������ ������
  flagMode         = 0;    // ������� �������������� �� ����-�����
}     


//----------------------------------------------------------------------------------------------
// ������������� ���������� ��������
// ---
bool PropertyManagerObject::InitProcessParam( long toolBarID, SpecPropertyToolBarEnum toolBarType, long firstTabID ) 
{
  bool res = false;
  GetNewKompasAPI();
  if ( pNewKompasAPI )
  {
    try 
    { 
      procParam = pNewKompasAPI->CreateProcessParam();  // �������� ��������� ��������
      if ( procParam ) 
      {
        new PropertyManagerEvent( procParam, *this );   // ������������� �� ������� �������� 
        
        // �������� ��������� ������ �������
        procParam->SpecToolbar = toolBarType;           // ��� ������ �� ������ �������  
        _bstr_t tmpBstr( LoadStr( toolBarID ) );
        procParam->Caption     = tmpBstr;               // ������������� ��������� ������ ������� 
        procParam->AutoReduce  = !flagMode;             // ������������ �������

        if ( flagMode ) 
        {
          procParam->DefaultControlFix = ksAllFix;
        }

        // �������� ��������� �������� ������ �������
        propTabs = procParam->PropertyTabs;                       

        // ������� �������� ���������� ��������
        if ( firstTabID ) 
        {
          CreateTab( firstTabID, TRUE, TRUE );
          // ���������� �������� ���������� ��� ������ ���������� ��������
          ShowControls();
          
          //// ������� ���� ���������
          //slideBox = curentCollection->Add( ksControlSlideBox );            
          //slideBox->SlideType      = ksSlide;                 
          //slideBox->DrawingSlide   = (long)GetSlideID(); 
          //slideBox->ResModule      = (LONG_PTR)StepDLL.hModule;
          //slideBox->Hint           = _T("Hint ��� ������");
          //slideBox->Tips           = _T("Tips ��� ������");
          //slideBox->Id             = 10000;
          //slideBox->Name           = _T("���� ���������");
          //slideBox->NameVisibility = ksNameHorizontalVisible;
        }
   
        int  paramCount = ParamCount();                             // ����� �� �������� ���� ?
        if ( paramCount )
        {
        }
        res = true;
      }
    }
    // �������� ����������
    catch(_com_error &e)
    {
      DumpError(e); // ����� ��������� � ������
    }    
    catch( LPCTSTR mes ) 
    {
      LibMessage( mes, MB_ICONERROR | MB_OK ); // ����� ��������� � ������
    }    
  }
  return res;
}


//----------------------------------------------------------------------------------------------
// �������� �������� � ����
// ---
void PropertyManagerObject::AddStringToGrig( long paramID, LPCTSTR value ) 
{
  if ( paramGrid ) 
  {
    paramGrid->CellText[ rowIndex   ][ 1 ] = _bstr_t(value);
    paramGrid->CellText[ rowIndex++ ][ 0 ] = _bstr_t(LoadStr(paramID)); // �� ������������ ��� ��������
  }
}  


//----------------------------------------------------------------------------------------------
// �������� �������� � ����
// ---
void PropertyManagerObject::AddDoubleToGrig( long paramID, double value ) 
{
  if ( paramGrid ) 
  {
    paramGrid->CellText[ rowIndex   ][ 1 ] = _bstr_t( DoubleToStr(value) );  // ������� ��������
    paramGrid->CellText[ rowIndex++ ][ 0 ] = _bstr_t( LoadStr(paramID)   );  // ��� ���������
  }
}  


//----------------------------------------------------------------------------------------------
// ������� ��������
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
// ������� ���������
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
// ������� ���������
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
// ���������� ���������� ��������� �������� 
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
// ���������� ���������� ����� �������� 
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
// ������� ���������
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
// ���������� CheckBox-� 
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
// ������� ����� ������
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
// �������� ������ � �����
// ---
void PropertyManagerObject::AddButton( ksAPI7::IPropertyMultiButtonPtr& buttons, long btnID, bool cheched, bool enable )
{
  AddButton2( buttons, btnID, btnID, cheched, enable );
}


//-----------------------------------------------------------------------------------------------------------------------------------
// �������� ������ � �����
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
// �������� ������ � �����
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
// ����� �������
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
// ���������� ����������� ��������
// ---
void PropertyManagerObject::SetControlEnable( long ctrlID, bool enabled ) 
{
  ksAPI7::IPropertyControlPtr control(GetPropertyControl(ctrlID), false );
  if ( control ) 
    control->Enable = enabled;
}


////////////////////////////////////////////////////////////////////////////////
//
// PropertyManagerEvent - ���������� ������� �� ���������
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
// ����� ���������
// ---
BEGIN_EVENTSINK_MAP(PropertyManagerEvent, BaseEvent)
  // 1 - prButtonClick        - ������� ������.
  ON_EVENT (PropertyManagerEvent, (unsigned int)-1, prButtonClick       , ButtonClick       , VTS_I4                )
  // 2 - prChangeControlValue - ������� ��������� �������� ��������
  ON_EVENT (PropertyManagerEvent, (unsigned int)-1, prChangeControlValue, ChangeControlValue, VTS_DISPATCH          )
  // 3 - prControlCommand     - ������� ������ ��������
  ON_EVENT (PropertyManagerEvent, (unsigned int)-1, prControlCommand    , ControlCommand    , VTS_DISPATCH VTS_I4   )
  // 4 - CLLBACK ��� ��������� ���� ������������ ����
  ON_EVENT( PropertyManagerEvent, (unsigned int)-1, prGetContextMenuType, GetContextMenuType, VTS_I4 VTS_I4 VTS_PI4 )
  // 5 - CLLBACK ��� ������� ����������� ������
  ON_EVENT( PropertyManagerEvent, (unsigned int)-1, prFillContextPanel  , FillContextPanel  , VTS_DISPATCH          )
END_EVENTSINK_MAP()


//-----------------------------------------------------------------------------
// prChangeControlValue - ������� ��������� �������� �������� 
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

                  // ������� ��������� ������� ��������� XOY
                  IEntityPtr basePlane(cor_part->GetDefaultEntity(o3d_planeXOY), false /*AddRef*/);

                  // ��������� ���������� ������
                  sketchDefinition->SetPlane(basePlane); // ��������� ��������� XOY ������� ��� ������


                  // �������� �����

                  int w=300, h = 10;
                  // ����� � ����� �������������� ������
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
// prChangeControlValue - ������� ��������� �������� �������� 
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
// prControlCommand ������� ������ ��������
// ---
afx_msg BOOL PropertyManagerEvent::ControlCommand(LPDISPATCH ctrl, long buttonID ) 
{
  obj.OnButtonClick(buttonID); 
  obj.RedrawPhantom();  
  return true;
}


//-----------------------------------------------------------------------------
// CLLBACK ��� ��������� ���� ������������ ����
// ---
afx_msg BOOL PropertyManagerEvent::GetContextMenuType( long LX, long LY, long * ContextMenuType )  // LX LY ��� �������� ����������
{
  *ContextMenuType = ksProcessContextPanel;
  return TRUE;
}


//-----------------------------------------------------------------------------
// "CLLBACK ��� ������� ����������� ������
// ---
afx_msg BOOL PropertyManagerEvent::FillContextPanel( LPDISPATCH ContextPanel )
{
  return TRUE;
}

//------------------------------------------------------------------------------
// �������� ������������ �����
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
// ksStop     -��������� ��������
// ---
BOOL Process3DEvent::Stop()
{
  return TRUE;
}

//--------------------------------------
// ksRun     -������ ��������
// ---
BOOL Process3DEvent::Run()
{
  return TRUE;
}

//---------------------------------------------------------------
// ksFilterObjects    -���������� ��������
// ---
BOOL Process3DEvent::FilterObject(LPDISPATCH Object) 
{
  return m_obj.FilterObject(Object);  
}

//---------------------------------------------------------
// ksPlacementChanged    -�������� ��������� �������
// ---
BOOL Process3DEvent::PlacementChanged(LPDISPATCH object)
{
  return m_obj.PlacementChanged(object);
}

//--------------------------------------
// ksEndProcess    -��������� ��������.
// ---
BOOL Process3DEvent::EndProcess()
{
  // ���������� ��������
  if ( m_obj.EndProcess() )
  {
    // ������� �� ��������� �������
    Disconnect();
  }
  return TRUE;
}

//-----------------------------------------------------------------------------------
// ksProcess3DCreateTakeObject    -������� �������� ������� � - --����������� ������.
// ---
BOOL Process3DEvent::TakeObject(LPUNKNOWN obj)
{
  return m_obj.TakeObject(obj);
}

//--------------------------------------------------------------------------------------------------------------------------
// ����� ���������
// ---
BEGIN_EVENTSINK_MAP(Process3DEvent, BaseEvent)
  // ksFilterObjects    -���������� ��������
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DFilterObjects   , FilterObject    , VTS_DISPATCH )
  // ksPlacementChanged    -�������� ��������� �������
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DPlacementChanged, PlacementChanged, VTS_DISPATCH )
  // ksEndProcess    -������� �������� ������� � - --����������� ������.
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DCreateTakeObject, TakeObject      , VTS_UNKNOWN  )
  // ksEndProcess    -��������� ��������.
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DEndProcess      , EndProcess      , VTS_NONE     )
  // ksRun     -������ ��������
  ON_EVENT(Process3DEvent, (unsigned int)-1, ksProcess3DRun             , Run             , VTS_NONE     )
  // ksStop     -��������� ��������
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
// ����� ���������
// ---
BEGIN_EVENTSINK_MAP(Process3DManipulatorsEvent, BaseEvent)
  // 1 - ksRotateManipulator - ������� ������������
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksRotateManipulator        , RotateManipulator        , VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BOOL)
  // 2 - ksMoveManipulator   - ������������ ������������
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksMoveManipulator          , MoveManipulator          , VTS_I4 VTS_R8 VTS_R8 VTS_R8 VTS_R8 VTS_BOOL                     )
  // 3 - ksClickManipulatorPrimitive - ���� �� ��������� ������������                                                           
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksClickManipulatorPrimitive, ClickManipulatorPrimitive, VTS_I4 VTS_I4 VTS_BOOL                                          )
  // 4 - ksBeginDragManipulator - ������ �������������� ������������                                                           
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksBeginDragManipulator     , BeginDragManipulator     , VTS_I4 VTS_I4                                                   )
  // 5 - ksEndDragManipulator - ��������� �������������� ������������                                                           
  ON_EVENT(Process3DManipulatorsEvent, (unsigned int)-1, ksEndDragManipulator       , EndDragManipulator       , VTS_I4 VTS_I4                                                   )                             
END_EVENTSINK_MAP()


//--------------------------------------------------------------------------------------------------------------------------------
// ksRotateManipulator - ������� ������������
// ---
BOOL Process3DManipulatorsEvent::RotateManipulator(long ManipulatorId,double X0, double Y0, double Z0,double AxisZX,
                                                 double AxisZXY, double AxisZZ, double angle, BOOL FromEdit)
{
  return m_obj.RotateManipulator( X0, Y0, Z0, AxisZX, AxisZXY, AxisZZ, angle, !!FromEdit);
}

//--------------------------------------------------------------------------------------------------------------------------------
// ksMoveManipulator   - ������������ ������������
// ---
BOOL Process3DManipulatorsEvent::MoveManipulator(long ManipulatorId, double VX, double VY, double VZ, double Delta, BOOL FromEdit)
{
  return m_obj.MoveManipulator(ManipulatorId, VX, VY, VZ, Delta, !!FromEdit);
}

//------------------------------------------------------------------------------------------------------------------
// ksClickManipulatorPrimitive - ���� �� ��������� ������������
// ---
BOOL Process3DManipulatorsEvent::ClickManipulatorPrimitive(long ManipulatorId, long PrimitiveType, BOOL DoubleClick)
{
  return m_obj.ClickManipulatorPrimitive(ManipulatorId, PrimitiveType, !!DoubleClick);
}

//--------------------------------------------------------------------------------------------
// ksBeginDragManipulator - ������ �������������� ������������
// ---
BOOL Process3DManipulatorsEvent::BeginDragManipulator(long ManipulatorId, long PrimitiveType)
{
  return m_obj.BeginDragManipulator(ManipulatorId, PrimitiveType);
}

//---------------------------------------------------------------------------------------------
// ksEndDragManipulator - ��������� �������������� ������������
// ---
BOOL Process3DManipulatorsEvent::EndDragManipulator(long ManipulatorId, long PrimitiveType)
{
  return m_obj.EndDragManipulator(ManipulatorId, PrimitiveType);
}



