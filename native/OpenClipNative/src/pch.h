#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define NOMINMAX // Must be before <windows.h> to avoid macro conflicts
#include "framework.h"
#endif // defined(_WIN32) || defined(_WIN64)