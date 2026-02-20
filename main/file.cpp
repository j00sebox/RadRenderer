#include "file.hpp"
#include "tinyfiledialogs.h"

std::string OpenFileDialog()
{
    const char* filterPatterns[] = { "*.gltf", "*.glb", "*.obj" };
    const char* result = tinyfd_openFileDialog(
        "Open Model",                    
        "",                              
        3,                               
        filterPatterns,                  
        "3D Models (*.gltf, *.glb, *.obj)",  
        0                                
    );

    if (result)
    {
        return std::string(result);
    }

    return "";
}
