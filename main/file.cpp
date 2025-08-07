#include "file.hpp"

// Needed for Windows backslash shenanigans
std::string NormalizePath(const std::string& path) 
{
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

#ifdef _WIN32

#include <windows.h>
std::string OpenFileDialog() 
{
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "GLTF Files\0*.gltf;*.glb\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    std::string result;
    if (GetOpenFileNameA(&ofn))
    {
        result = std::string(szFile);
    }

    return NormalizePath(result);
}

#elif defined(__linux__)
#include <cstdlib>

std::string OpenFileDialog() 
{
  
}
#else
std::string OpenFileDialog() 
{
    return "";
}
#endif

