/////////////////////////////////////////////////////////////////////////////
//
// EventsList - Список подписчиков
// BaseEvent - Базовый класс для обработчиков событий в автоматизации
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <afxctl.h>
#include <afxpriv.h>

#include "BaseEvent.h"

#include <IUptr.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//-------------------------------------------------------------------------------
// Список подписчиков
// ---
CObList& BaseEvent::m_EventList = *new CObList();


/////////////////////////////////////////////////////////////////////////////
//
// Базовый класс для автоматизационных обработчиков событий
//
/////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------
// Конструктор 
// ---
BaseEvent::BaseEvent( LPUNKNOWN  pObject, IID iidEvents ) 
 : CCmdTarget (           ),  
  m_dwCookie  ( 0         ),
  m_pContainer( pObject   ), // Источник событий
  m_iidEvents ( iidEvents ), // IID
  m_pConnPt   ( NULL      ) 
{
  if ( m_pContainer ) 
    m_pContainer->AddRef();                      
  m_EventList.AddTail( this ); 
  ASSERT( !IsEqualIID(m_iidEvents, GUID_NULL) ); // Прислали тип событий
}


//-------------------------------------------------------------------------------
// Деструктор
// ---
BaseEvent::~BaseEvent()
{
  RemoveThis();
  if ( m_pContainer ) 
  {
    m_pContainer->Release();
    m_pContainer = NULL;     
  }
}


//-------------------------------------------------------------------------------
// Подписаться на получение событий
// ---
int BaseEvent::Advise() 
{
  ASSERT( m_dwCookie == 0 ); // Повторно подписываться нельзя

  // Подписаться на получение событий
  if ( m_pContainer ) 
  {
    LPCONNECTIONPOINTCONTAINER lpContainer = NULL;
    if ( SUCCEEDED(m_pContainer->QueryInterface(IID_IConnectionPointContainer, (LPVOID*)&lpContainer)) ) 
    {
      if ( lpContainer && SUCCEEDED(lpContainer->FindConnectionPoint(m_iidEvents, &m_pConnPt)) ) 
      {
        ASSERT( m_pConnPt != NULL );
        m_pConnPt->Advise( &m_xEventHandler, &m_dwCookie ); 
      }
      lpContainer->Release();
    }
  }

  // Неудачная подписка на события
  if( !m_dwCookie )
  {
    delete this;
    return 0;
  }

  return m_dwCookie;
}


//-------------------------------------------------------------------------------
// Отписаться от получения событий
// ---
void BaseEvent::Unadvise()
{
  if ( m_pConnPt != NULL )              // Подписка была
  {
    m_pConnPt->Unadvise( m_dwCookie );  // Отписаться от получения событий
    m_pConnPt->Release();              // Освободить 
    m_pConnPt  = NULL;
  } 
  m_dwCookie = 0;
}

//-------------------------------------------------------------------------------
// Удалить список
// ---
void BaseEvent::DestroyList() 
{
  if ( &m_EventList ) 
  {
    delete &m_EventList;
  }
}


//-------------------------------------------------------------------------------
// Удалить себя из списка 
// ---
void BaseEvent::RemoveThis() 
{
  // Удалим себя из списка обработчиков событий
  POSITION pos = m_EventList.Find(this);
  if ( pos ) 
  { 
    m_EventList.RemoveAt( pos );
  }
  Unadvise();                           // Отпишемся от получения событий
}


//-------------------------------------------------------------------------------
// 
// ---
void BaseEvent::Clear() 
{
  if ( m_pContainer ) 
  {
    m_pContainer->Release();
    m_pContainer = NULL;     
  }
  m_iidEvents = GUID_NULL;
}

//-------------------------------------------------------------------------------
// Рассоединиться
// ---
void BaseEvent::Disconnect()      
{
  Unadvise(); 
  Clear();
  ExternalRelease();
}

//-----------------------------------------------------------------------------
// отписать все события
// ---
void BaseEvent::TerminateEvents( void )
{
  while ( !m_EventList.IsEmpty() ) 
  {
  BaseEvent* headEvent = (BaseEvent*)m_EventList.RemoveHead();
  headEvent->Disconnect();      
  }
}


//-----------------------------------------------------------------------------
// Отписать все события
// ---
void BaseEvent::TerminateEvents( LPUNKNOWN container )
{
  if ( container ) 
  {
    INT_PTR count = m_EventList.GetCount();
    for ( INT_PTR i = count - 1; i>= 0 ; i-- )
    {
      CObject* obj = m_EventList.GetAt( m_EventList.FindIndex(i) );
      BaseEvent* event = (BaseEvent*) obj;  
      if ( event && IsSame(event->m_pContainer, container) )
        event->Disconnect();  ; // В деструкторе будет удален из списка RemoveAt(pos)
    }
  }
}


//-------------------------------------------------------------------------------
// Карта интерфейса
// ---
BEGIN_INTERFACE_MAP(BaseEvent, CCmdTarget)
END_INTERFACE_MAP()


//-------------------------------------------------------------------------------
// Карта сообщений интерфейса
// ---
BEGIN_EVENTSINK_MAP(BaseEvent, CCmdTarget)
END_EVENTSINK_MAP()


//-------------------------------------------------------------------------------
// AddRef
// ---
STDMETHODIMP_(ULONG) BaseEvent::XEventHandler::AddRef()
{
  METHOD_PROLOGUE( BaseEvent, EventHandler )
  return (ULONG)pThis->ExternalAddRef();
}


//-------------------------------------------------------------------------------
// GetIDsOfNames
// ---
STDMETHODIMP BaseEvent::XEventHandler::GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames, 
                                                      unsigned int cNames, LCID lcid, 
                                                      DISPID* rgdispid )
{
  METHOD_PROLOGUE( BaseEvent, EventHandler )
  ASSERT_VALID( pThis );

  return ResultFromScode( E_NOTIMPL );
}


//-------------------------------------------------------------------------------
// GetTypeInfo
// ---
STDMETHODIMP BaseEvent::XEventHandler::GetTypeInfo( unsigned int itinfo, LCID lcid, 
                                                    ITypeInfo** pptinfo )
{
  METHOD_PROLOGUE( BaseEvent, EventHandler )
  ASSERT_VALID( pThis );
  
  return ResultFromScode( E_NOTIMPL );
}


//-------------------------------------------------------------------------------
// GetTypeInfoCount
// ---
STDMETHODIMP BaseEvent::XEventHandler::GetTypeInfoCount( unsigned int* pctinfo )
{
  METHOD_PROLOGUE( BaseEvent, EventHandler )
  *pctinfo = 0;
  return NOERROR;
}


//-------------------------------------------------------------------------------
// Пересылка событий
// ---
STDMETHODIMP BaseEvent::XEventHandler::Invoke( DISPID           dispidMember, 
                                               REFIID           riid, 
                                               LCID             lcid, 
                                               unsigned short   wFlags, 
                                               DISPPARAMS     * lpDispparams, 
                                               VARIANT        * pvarResult, 
                                               EXCEPINFO      * pexcepinfo, 
                                               unsigned int   * puArgErr )
{
  METHOD_PROLOGUE( BaseEvent, EventHandler )
  ASSERT_VALID( pThis );

  // Класс параметров сообщения 
  AFX_EVENT event( AFX_EVENT::event, dispidMember, lpDispparams, pexcepinfo, puArgErr );

  // Примечание: OnEvent в отсутствии обработчика возвращает FALSE, для правильной работы,
  //             необходимо возвращать TRUE, чтобы не блокировать действия Компас, поэтому 
  //             перед OnEvent включена проверка GetEventSinkEntry           

  BOOL eventHandled = TRUE; // Событие не обрабатывается, необходимо вернуть TRUE
  if (pThis->GetEventSinkEntry( 1, &event ) != NULL )      
    eventHandled = pThis->OnEvent( 1, &event, NULL ); // Передача сообщения дальше

  if ( pvarResult != NULL ) 
  {
    VariantClear( pvarResult );                       // Результат 
                                    
    V_VT  (pvarResult) = VT_BOOL;
    V_BOOL(pvarResult) = eventHandled;
  
  }
  return event.m_hResult;
}


//-------------------------------------------------------------------------------
// QueryInterface
// ---
STDMETHODIMP BaseEvent::XEventHandler::QueryInterface( REFIID iid, LPVOID* ppvObj )
{
  METHOD_PROLOGUE( BaseEvent, EventHandler )
  
  *ppvObj = NULL;
  if ( IID_IUnknown == iid || IID_IDispatch == iid || iid == pThis->m_iidEvents )
    *ppvObj = this;

  if ( NULL != *ppvObj )
  {
    ((LPUNKNOWN)*ppvObj)->AddRef();
    return NOERROR;
  }

  return (HRESULT)pThis->ExternalQueryInterface( &iid, ppvObj );
}


//-------------------------------------------------------------------------------
// Release
// ---
STDMETHODIMP_(ULONG) BaseEvent::XEventHandler::Release()
{
  METHOD_PROLOGUE( BaseEvent, EventHandler )
  return (ULONG)pThis->ExternalRelease();
}


