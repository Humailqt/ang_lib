#pragma once

#include "stdafx.h"

void draw_panel(IDocument3DPtr doc, IPart *part/*, CStringW str*/);



void draw_panel(IDocument3DPtr doc, IPart *part/*,CStringW str*/)
{
    //GetControlExpression()
    ksAPI7::IPropertyControlsPtr d(doc);
    //VARIANT var; 
    //var.intVal = ID_CHOSE_DETAIL;
    //ksAPI7::IPropertyControl **iprop;
    //d->get_Item(var, iprop);
    //LibMessage((*iprop)->GetValue().bstrVal);
    //LibMessage(_T("draw_panel"));
    part->SetFileName(L"D:/ang_lib/ang_detile/d.m3d"/*(LPWSTR)str.operator LPCWSTR*/);
    part->Update();
    
}

