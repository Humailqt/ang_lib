///////////////////////////////////////////////////////////////////////////////
//
// EventsList - Список подписчиков
// BaseEvent - Базовый клас для обработчиков событий в автоматизации
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __BASEEVENT_H
#define __BASEEVENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
//
// Базовый клас для автаматизационных обработчиков событий
//
/////////////////////////////////////////////////////////////////////////////
class BaseEvent : public CCmdTarget
{
protected:
  static CObList&    m_EventList; // Список обработчиков событий
  DWORD               m_dwCookie; // Идентификатор соединения
  IID		         m_iidEvents; // IID интерфейса событий
  LPCONNECTIONPOINT	   m_pConnPt; // Соединение 
  LPUNKNOWN         m_pContainer; // Источник событий

public:
  // Конструктор
  BaseEvent(LPUNKNOWN pObject, IID iidEvents);
  // Диструктор
  virtual ~BaseEvent();
  
  static  void  TerminateEvents(void);  // Отписать все события
  static  void  DestroyList();          // Удалить список      
  void          RemoveThis();           // Удалить себя из списка   
  void          Unadvise();             // Отписаться от получения событий
  void          Disconnect();           // Рассоединиться
  virtual void  Clear();
  int           Advise();               // Подписаться на получение событий
  
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
