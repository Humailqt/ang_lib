#include "ResourceDir.h"

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

