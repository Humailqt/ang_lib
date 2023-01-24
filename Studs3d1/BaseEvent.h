///////////////////////////////////////////////////////////////////////////////
//
// EventsList - ������ �����������
// BaseEvent - ������� ���� ��� ������������ ������� � �������������
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __BASEEVENT_H
#define __BASEEVENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
//
// ������� ���� ��� ����������������� ������������ �������
//
/////////////////////////////////////////////////////////////////////////////
class BaseEvent : public CCmdTarget
{
protected:
  static CObList&    m_EventList; // ������ ������������ �������
  DWORD               m_dwCookie; // ������������� ����������
  IID		         m_iidEvents; // IID ���������� �������
  LPCONNECTIONPOINT	   m_pConnPt; // ���������� 
  LPUNKNOWN         m_pContainer; // �������� �������

public:
  // �����������
  BaseEvent(LPUNKNOWN pObject, IID iidEvents);
  // ����������
  virtual ~BaseEvent();
  
  static  void  TerminateEvents(void);  // �������� ��� �������
  static  void  DestroyList();          // ������� ������      
  void          RemoveThis();           // ������� ���� �� ������   
  void          Unadvise();             // ���������� �� ��������� �������
  void          Disconnect();           // ��������������
  virtual void  Clear();
  int           Advise();               // ����������� �� ��������� �������
  
  static  void TerminateEvents(LPUNKNOWN container);

protected:
  BEGIN_INTERFACE_PART(EventHandler, IDispatch)
  INIT_INTERFACE_PART(BaseEvent, EventHandler)
  STDMETHOD(GetTypeInfoCount)(unsigned int*);
  STDMETHOD(GetTypeInfo)(unsigned int, LCID, ITypeInfo**);
  STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR*, unsigned int, LCID, DISPID*);
  STDMETHOD(Invoke)(DISPID, REFIID, LCID, unsigned short, DISPPARAMS*, VARIANT*, EXCEPINFO*, unsigned int*);
  END_INTERFACE_PART(EventHandler)
  
  DECLARE_INTERFACE_MAP()
  DECLARE_EVENTSINK_MAP()
};

////////////////////////////////////////////////////////////////////////////////
#endif 
