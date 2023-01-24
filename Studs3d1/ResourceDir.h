#pragma once
#include <filesystem>
#define patch_lib_detail L"D:/ang_lib/ang_detile/"
#ifdef _UNICODE

#define patch_resure_detales L"ang_detile//"

#else

#define patch_resure_detales ".ang_detile//"

#endif // _UNICODE
	
#ifndef _ACTIVE_FILE_PATCH
#define _ACTIVE_FILE_PATCH 
CString active_file_patch()
{
    CString	file_name;
    IDocument3DPtr doc;
    std::filesystem::path dir_patch;

    doc = ksGetActive3dDocument();
    if (doc)
    {
        file_name = doc->GetFileName();
        {
            const size_t newsizew = (file_name.GetLength() + 1) * 2;
            wchar_t* n2stringw = new wchar_t[newsizew];
            wcscpy_s(n2stringw, newsizew, file_name);
            dir_patch = std::filesystem::path(n2stringw);
            delete[]n2stringw;

        }
        dir_patch.remove_filename();
        file_name = dir_patch.c_str();
         
        return file_name;
    }
    return file_name;
}
#endif // !_ACTIVE_FILE_PATCH


#ifndef _GETCORRENT_DIR
#define _GETCORRENT_DIR 

CString Get_Corrent_Dir()
{
    CString filename;
    const size_t LENG_BUF = 256;
    WCHAR work_patch[LENG_BUF];
    LPCTSTR p_wp = work_patch;
    IDocument3DPtr pDocument3d(ksGetActive3dDocument(), false);

    filename = pDocument3d->GetFileName();
    std::filesystem::path dir_patch;
    {
        const size_t newsizew = (filename.GetLength() + 1) * 2;
        wchar_t* n2stringw = new wchar_t[newsizew];
        wcscpy_s(n2stringw, newsizew, filename);
        dir_patch = std::filesystem::path(n2stringw);
        delete[]n2stringw;
    }
    dir_patch.remove_filename();
    
    CString lib_patch(CString(dir_patch.c_str()));

    return lib_patch;
}
#endif // !_GETCORRENT_DIR
