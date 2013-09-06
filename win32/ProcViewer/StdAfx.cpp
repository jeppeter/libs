// stdafx.cpp : source file that includes just the standard includes
//	ProcessViewer.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// Initialize COM just once, some Util functions do use COM stuff.
Utils::CoAutoInitializer g_AutoInitializeCOMEnv;