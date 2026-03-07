#pragma once
#include "SL-lib/y_base.h"
#include "SL-lib/SL_lib.h"
#ifdef _DEBUG
#pragma comment (lib,"SL-lib/debug/SL_lib_debug.lib")
#else
#pragma comment (lib,"SL-lib/release/SL_lib.lib")
#endif