// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS
// Windows Header Files:
#include <windows.h>
#include <tchar.h>

#define IN_RING3
#include <VBox/vmm/pdmdev.h>
#include <VBox/vmm/pdmdrv.h>
#include <VBox/vmm/pdmusb.h>
#include <VBox/vmm/pgm.h>
