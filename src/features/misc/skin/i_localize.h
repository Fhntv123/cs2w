#pragma once
#include "../../../memory/Memory.h"
#include <stdio.h>

class i_localize {
public:
	const char* find_key(const char* key) {
		return Memory::CallVFunc<const char*, 17>(this, key, "");
	}
};