#include "file.hpp"
#include "tinyfiledialogs.h"

std::string OpenFileDialog()
{
    const char* filterPatterns[] = { "*.gltf", "*.glb", "*.obj" };

    const char* result = tinyfd_openFileDialog(
        "Open Model",                    // title
        "",                              // default path
        3,                               // number of filter patterns
        filterPatterns,                  // filter patterns
        "3D Models (*.gltf, *.glb, *.obj)",  // filter description
        0                                // allow multiple selects
    );

    if (result) {
        return std::string(result);
    }

    return "";
}
