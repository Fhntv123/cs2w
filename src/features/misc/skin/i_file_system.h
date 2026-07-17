#pragma once
#include "../../../memory/Memory.h"
class i_file_system
{
public:
    bool exists(const char* file_name, const char* a3)
    {
        return Memory::CallVFunc<bool, 21>(this, file_name, a3);
    }
};
